#include "install_dlg.hpp"

InstallProgressDlg::InstallProgressDlg(wxWindow* parent, const std::string& editorInstaller, const std::vector<ComponentInstaller>& componentInstallers) : InstallProgressDlgBase(parent){
    
    std::vector<wxVariant> values;
    values.push_back(editorInstaller);
    values.push_back(0);
    values.push_back("Pending");
    
    statusList->AppendItem(values);

    // add component installers
    for(const auto& installer : componentInstallers){
        values[0] = wxVariant(installer.name);
        
        statusList->AppendItem(values);
    }
    
    // dispatch all the downloads
}
