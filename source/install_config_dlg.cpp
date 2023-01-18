#include "install_dlg.hpp"
#include "HTTP.hpp"
#include <fmt/format.h>
#include "globals.h"
#include <wx/msgdlg.h>

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

ConfigureInstallDlg::ConfigureInstallDlg(wxWindow* parent, const installVersionData& version) : ConfigureEditorDlgBase(parent){
    // download the INI file
    auto iniresult = fetch(fmt::format("https://download.unity3d.com/download_unity/{}/unity-{}.ini", version.hashcode,ini_platform));
    if (iniresult.code != 200){
        wxMessageBox("Unable to get Editor version data","Install failed",wxOK | wxICON_ERROR);
        this->Close();
    }
}
