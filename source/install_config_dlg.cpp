#include "install_dlg.hpp"
#include "HTTP.hpp"
#include <fmt/format.h>
#include "globals.h"
#include <wx/msgdlg.h>
#include <sstream>

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

auto parseIni (std::string_view string, uint32_t index = 0){
    std::unordered_map<std::string_view, std::unordered_map<std::string_view, std::string_view>> inidata;
    
    // get the name of a section
    auto getSectionName = [&string](uint32_t& index){
        auto startidx = index + 1;  // advance past '['
        while (string[index] != ']'){
            index++;
        }
        return std::string_view(string.data() + startidx, index - startidx);
    };
    
    auto parseSection = [&string](uint32_t& index){
        decltype(inidata)::value_type::second_type content;
        
        auto getUntilChar = [&string](uint32_t& index, const char sentinel){
            auto startidx = index;
            while (string[index] != sentinel){
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
}
