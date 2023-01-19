#include "install_dlg.hpp"
#include "HTTP.hpp"
#include <fmt/format.h>
#include "globals.h"
#include <wx/msgdlg.h>
#include <sstream>
#include <string>
#include <charconv>
#include <array>

wxBEGIN_EVENT_TABLE(ConfigureInstallDlg, wxDialog)
EVT_TREELIST_ITEM_CHECKED(ID_MODULESELECTTREE, ConfigureInstallDlg::OnCheckedChanged)
EVT_BUTTON(ID_INSTALL_BTN, ConfigureInstallDlg::OnInstallClicked)
wxEND_EVENT_TABLE()

constexpr static const char* const ini_platform =
#if __APPLE__
    #if __aarch64__
    "osx-arm64";
    #else
    "osx";
    #endif
#elif _WIN32
"win";
#elif __linux__
"linux";
#else
"";
#error Unidentified INI platform
#endif

/**
 Formats a raw file size to a string with a unit
 @param fileSize the size of the item in bytes
 @returns unitized string, example "12 KB"
 */
std::string sizeToString(const uint64_t& fileSize){
    std::string formatted = "";
    int size = 1000;        //MB = 1000, MiB = 1024
    std::array<const char* const,5> suffix { " bytes", " KB", " MB", " GB", " TB" };
    auto max = suffix.size();

    for (int i = 0; i < max; i++)
    {
        double compare = pow(size, i);
        if (fileSize <= compare || i == max - 1)
        {
            int minus = 0;
            if (i > 0)
            {
                minus = 1;
            }
            
            if (fileSize > compare){
                minus = 0;
            }
            
            //round to 2 decimal places (except for bytes), then attach unit
            char buffer[10];
            const char* format = (i - minus == 0) ? "%.0f" : "%.2f";
            snprintf(buffer, 10, format, fileSize / pow(size,i-minus));
            formatted = std::string(buffer) + suffix[i - minus];
            break;
        }
    }

    if (formatted == ""){
        return "??";
    }
    return formatted;
}


auto parseIni (std::string_view string, uint32_t index = 0){
    decltype(ConfigureInstallDlg::inidata) inidata;
    
    // get the name of a section
    auto getSectionName = [&string](uint32_t& index){
        auto startidx = index + 1;  // advance past '['
        while (string[index] != ']' && index < string.size()){
            index++;
        }
        return std::string_view(string.data() + startidx, index - startidx);
    };
    
    auto parseSection = [&string](uint32_t& index){
        decltype(inidata)::value_type::second_type content;
        
        auto getUntilChar = [&string](uint32_t& index, const char sentinel){
            auto startidx = index;
            while (string[index] != sentinel && index < string.size()){
                index++;
            }
            return std::string_view(string.data() + startidx,index - startidx);
        };
        
        while (string[index] != '[' && index < string.size()){
            auto name = getUntilChar(index,'=');
            index++;    // advance past the '='
            auto data = getUntilChar(index,'\n');
            content[name] = data;
            index++;    // advance past the newline
        }
        return content;
    };
    
    // get the content of a section
    
    while (index < string.size()){
        std::string_view currentSectionName = getSectionName(index);
        char c = string[index];
        switch (c) {
            case '[':{
                currentSectionName = getSectionName(index);
            }
            case ']':{
                index+=2;    // advance past ']\n'
                inidata[currentSectionName] = parseSection(index);
            }
                break;
                
            default:
                break;
        }
    }
    return inidata;
};

template<typename T>
T sv_to_t(const std::string_view sv){
    T value;
    std::from_chars(sv.data(), sv.data() + sv.size(), value);
    return value;
}

ConfigureInstallDlg::ConfigureInstallDlg(wxWindow* parent, const installVersionData& version) : hashcode(version.hashcode), ConfigureEditorDlgBase(parent){
    // download the INI file
    auto iniresult = fetch_to_mem(fmt::format("https://download.unity3d.com/download_unity/{}/unity-{}.ini", version.hashcode,ini_platform));
    if (iniresult.code != 200){
        wxMessageBox("Unable to get Editor version data","Install failed",wxOK | wxICON_ERROR);
        this->Close();
    }
    
    // parse the ini
   
    // NOTE: do not modify this structure after it is set!
    initext = std::move(iniresult.text);
    inidata = parseIni(initext);
    
    this->SetTitle(fmt::format("Install {}",inidata["Unity"]["title"]));
    
    // set destination default location
    {
        auto defaultInstallLocation = defaultInstall.size() > 0 ? defaultInstall[0] : std::filesystem::path();
        wxFileName defaultInstall(defaultInstallLocation.string());
        destinationPicker->SetDirName(defaultInstall);
    }
   
    
    // populate the data table
    for(const auto& component : inidata){
        if (component.first == "Unity" || component.first == "VisualStudio" || (component.second.contains("hidden") && component.second.at("hidden") == "true")){    // don't show Unity row
            continue;
        }

        auto item = moduleSelectTree->AppendItem(moduleSelectTree->GetRootItem(), std::string(component.second.at("title")));
        
        uint64_t size = 0;

        auto sv = component.second.at("size");
        size = sv_to_t<decltype(size)>(sv);
        moduleSelectTree->SetItemText(item, 1, sizeToString(size));
        
        sv = component.second.at("installedsize");
        size = sv_to_t<decltype(size)>(sv);
        moduleSelectTree->SetItemText(item, 2, sizeToString(size));
        moduleSelectTree->SetItemData(item, new treeItemData(&component.second));   // will be freed by the tree list
    }
    
    RecalculateSize();
}

void ConfigureInstallDlg::RecalculateSize(){
    // go through all the items and compute the total download and install size
    uint64_t installedSize = 0;
    uint64_t downloadSize = 0;
    for(auto item = moduleSelectTree->GetRootItem(); item ; item = moduleSelectTree->GetNextItem(item)){
        treeItemData* data = static_cast<decltype(data)>(moduleSelectTree->GetItemData(item));
        if (!data){
            continue;
        }
        
        if (!moduleSelectTree->GetCheckedState(item)){
            continue;
        }
        
        auto sv = data->data->at("installedsize");
        installedSize += sv_to_t<decltype(installedSize)>(sv);
        
        sv = data->data->at("size");
        downloadSize += sv_to_t<decltype(downloadSize)>(sv);
    }
    // account for the Editor
    installedSize += sv_to_t<decltype(installedSize)>(inidata["Unity"]["installedsize"]);
    downloadSize += sv_to_t<decltype(downloadSize)>(inidata["Unity"]["size"]);
    
    totalInstallLabel->SetLabelText(fmt::format("Install Size: {} / Download Size: {}", sizeToString(installedSize), sizeToString(downloadSize)));
}

void ConfigureInstallDlg::OnCheckedChanged(wxTreeListEvent& evt){
    RecalculateSize();
}

void ConfigureInstallDlg::OnInstallClicked(wxCommandEvent &){
    std::vector<ComponentInstaller> componentInstallers;
    
    auto editorInstaller = inidata["Unity"]["url"];
    auto editorInstallerFilename = std::filesystem::path(editorInstaller).filename();
    
    for(auto item = moduleSelectTree->GetRootItem(); item ; item = moduleSelectTree->GetNextItem(item)){
        treeItemData* data = static_cast<decltype(data)>(moduleSelectTree->GetItemData(item));
        if (!data){
            continue;
        }
        
        if (!moduleSelectTree->GetCheckedState(item)){
            continue;
        }
        
        auto url = data->data->at("url");
        auto name = data->data->at("title");
        auto outfilename = std::filesystem::path(url).filename();
        componentInstallers.emplace_back(std::string(name), std::string(url), std::string(outfilename));
    }
    auto installProgressDlg = new InstallProgressDlg(GetParent(), {"Editor Application", std::string(editorInstaller), fmt::format("{}-{}",inidata["Unity"]["version"], std::string(editorInstallerFilename))}, componentInstallers, fmt::format("https://download.unity3d.com/download_unity/{}",hashcode));
    installProgressDlg->Show();
    this->Close();
}
