#include "install_dlg.hpp"
#include "HTTP.hpp"
#include <fmt/format.h>
#include "globals.h"
#include <wx/msgdlg.h>
#include <sstream>
#include <string>
#include <charconv>
#include <array>

constexpr static const char* const ini_platform =
#if __APPLE__
"osx";
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
    std::unordered_map<std::string_view, std::unordered_map<std::string_view, std::string_view>> inidata;
    
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
        
        while (string[index] != '['){
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

ConfigureInstallDlg::ConfigureInstallDlg(wxWindow* parent, const installVersionData& version) : ConfigureEditorDlgBase(parent){
    // download the INI file
    auto iniresult = fetch(fmt::format("https://download.unity3d.com/download_unity/{}/unity-{}.ini", version.hashcode,ini_platform));
    if (iniresult.code != 200){
        wxMessageBox("Unable to get Editor version data","Install failed",wxOK | wxICON_ERROR);
        this->Close();
    }
    
    // parse the ini
   
    auto inidata = parseIni(iniresult.text);
    
    this->SetTitle(fmt::format("Install {}",inidata["Unity"]["title"]));
    
    // populate the data table
    
    for(const auto& component : inidata){
        if (component.first == "Unity"){    // don't show Unity row
            continue;
        }

        auto item = moduleSelectTree->AppendItem(moduleSelectTree->GetRootItem(), std::string(component.second.at("title")));
        
        uint64_t size = 0;

        auto sv = component.second.at("size");
        std::from_chars(sv.data(), sv.data() + sv.size(), size);
        moduleSelectTree->SetItemText(item, 1, sizeToString(size));
        
        sv = component.second.at("installedsize");
        std::from_chars(sv.data(), sv.data() + sv.size(), size);
        moduleSelectTree->SetItemText(item, 2, sizeToString(size));
    }
}
