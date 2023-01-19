#include "install_dlg.hpp"
#include "HTTP.hpp"
#include <wx/msgdlg.h>
#include <fmt/format.h>
#include "globals.h"

#define PROGEVT 99999999
#define STATUSEVT 89999
#define DONEEVT 79999

wxDEFINE_EVENT(progEvt, wxCommandEvent);
wxDEFINE_EVENT(statusEvt, wxCommandEvent);
wxDEFINE_EVENT(complEvt, wxCommandEvent);

wxBEGIN_EVENT_TABLE(InstallProgressDlg, wxDialog)
EVT_BUTTON(ID_INSTALL_BTN_CANCEL, InstallProgressDlg::OnCancelClicked)
EVT_COMMAND(PROGEVT, progEvt, InstallProgressDlg::OnNumericProgressUpdate)
EVT_COMMAND(STATUSEVT, statusEvt, InstallProgressDlg::OnStatusUpdate)
EVT_COMMAND(DONEEVT, complEvt, InstallProgressDlg::OnAllCompleted)
wxEND_EVENT_TABLE()

InstallProgressDlg::InstallProgressDlg(wxWindow* parent, const ComponentInstaller& editorInstaller, const std::vector<ComponentInstaller>& componentInstallers, const decltype(baseURL)& baseURL) : baseURL(baseURL), InstallProgressDlgBase(parent){
    
    nameCol->SetWidth(wxCOL_WIDTH_AUTOSIZE);
    installThreads.reserve(componentInstallers.size() + 1);
    
    // kick off the Editor into row 0
    std::vector<wxVariant> values;
    values.push_back(editorInstaller.name);
    values.push_back(0);
    values.push_back("Queued");
    
    statusList->AppendItem(values);
    
    (installThreads.emplace_back([this, editorInstaller]{
        this->InstallComponent(editorInstaller, 0);
    })).detach();

    // add component installers into the next rows
    uint32_t row = 1;
    for(const auto& installer : componentInstallers){
        values[0] = wxVariant(installer.name);
        
        statusList->AppendItem(values);
        
        // dispatch download
        (installThreads.emplace_back([this,row, installer]{
            this->InstallComponent(installer, row);
        })).detach();
        row++;
    }
}

void InstallProgressDlg::InstallComponent(const ComponentInstaller &installer, uint32_t row){
    auto fullurl = fmt::format("{}/{}",baseURL, installer.installerURL);
    
    FunctionCallback progressCallback{[this,row](long totaldownload, long currentDownload, long, long){
        auto progress = currentDownload / static_cast<float>(totaldownload) * 100;
        if (!isnan(progress)){
            PostProgressUpdate(progress, row);
        }
    }};
    
    // download to the temp folder
    auto destpath = std::filesystem::temp_directory_path() / "UnityHubNativeDownloads" / installer.outputFileName;
    
    std::filesystem::create_directories(destpath.parent_path());
    
    PostStatusUpdate("Downloading", row);
    stream_to_file(fullurl, destpath, progressCallback);
    
    // TODO: perform install actions
    
    // TODO: clean up
    
    // update completion
    ncomplete++;
    PostStatusUpdate("Completed", row);
    
    // all done?
    if (Complete()){
        wxCommandEvent evt;
        evt.SetId(DONEEVT);
        wxPostEvent(this, evt);
    }
}

void InstallProgressDlg::CancelAll(){
    cancelCloseBtn->SetLabel("Cancelling...");
    cancelCloseBtn->Disable();
    for(auto& thread : installThreads){
        if (thread.joinable()){
            thread.join();
        }
    }
}


void InstallProgressDlg::OnCancelClicked(wxCommandEvent &){
    if (!Complete()){
        // ask if want to cancel
        wxMessageDialog prompt(this,"Any completed tasks will not be removed.","Cancel install", wxYES|wxNO|wxCENTRE);
        auto result = prompt.ShowModal();
        if (result){
            // cancel the remaining tasks
            this->Close();
        }
    }
    else{
        this->Close();
    }
}

void InstallProgressDlg::OnNumericProgressUpdate(wxCommandEvent& evt){
    auto prog = evt.GetInt();
    auto row = static_cast<int>(evt.GetExtraLong());
    
    statusList->SetValue(prog, row, 1);
}

void InstallProgressDlg::OnStatusUpdate(wxCommandEvent& evt){
    auto str = evt.GetString();
    auto row = static_cast<int>(evt.GetExtraLong());
    
    statusList->SetTextValue(str, row, 3);
}

void InstallProgressDlg::PostProgressUpdate(int prog, uint32_t row){
    wxCommandEvent event(progEvt);
    event.SetId(PROGEVT);
    event.SetInt(prog);
    event.SetExtraLong(row);
    wxPostEvent(this, event);
}

void InstallProgressDlg::PostStatusUpdate(const std::string &newStatus, uint32_t row){
    wxCommandEvent event(progEvt);
    event.SetId(STATUSEVT);
    event.SetString(newStatus);
    event.SetExtraLong(row);
    wxPostEvent(this, event);
}

void InstallProgressDlg::OnAllCompleted(wxCommandEvent &){
    cancelCloseBtn->SetLabel("close");
}

InstallProgressDlg::~InstallProgressDlg(){
    if (!Complete()){
        // stuff is in progress, need to cancel it
        CancelAll();
    }
}
