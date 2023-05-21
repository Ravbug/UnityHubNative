#include "interface_derived.hpp"
#include "activation.hpp"
#include "fmt/format.h"
#include <wx/process.h>
#include <wx/filedlg.h>
#include <fstream>
#include "output_dialog.hpp"

using namespace std;

wxBEGIN_EVENT_TABLE(PersonalActivationDlg, wxDialog)
EVT_BUTTON(PAD_CREATE, PersonalActivationDlg::OnCreateHit)
EVT_BUTTON(PAD_ACTIVATE, PersonalActivationDlg::OnActivateHit)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(PlusProActivationDlg, wxDialog)
EVT_BUTTON(PPA_ACTIVATE, PlusProActivationDlg::OnActivateHit)
wxEND_EVENT_TABLE()

void MainFrameDerived::OnActivatePersonal(wxCommandEvent& event) {
	auto selected = installsList->GetSelection();
	if (selected != -1) {
		const auto& editor = editors[selected];
		auto dlg = new PersonalActivationDlg(this, editor);
		dlg->ShowModal();
	}
}

void MainFrameDerived::OnActivateProPlus(wxCommandEvent& event) {
	auto selected = installsList->GetSelection();
	if (selected != -1) {
		const auto& editor = editors[selected];
		auto dlg = new PlusProActivationDlg(this, editor);
		dlg->ShowModal();
	}
	
}

void PersonalActivationDlg::OnCreateHit(wxCommandEvent& evt) 
{
	wxFileDialog saveFileDialog(this, _("Save Activation file"), "", "activation.alf", "ALF files (*.alf)|*.alf", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (saveFileDialog.ShowModal() == wxID_CANCEL) {}
	else {
		// pass the chosen location
		auto exe = the_editor.executablePath().string();
		auto cmd = fmt::format("{} -batchmode -createManualActivationFile -logfile",exe);
		auto cwd = wxGetCwd();
		auto root = std::filesystem::path(saveFileDialog.GetPath().ToStdString()).parent_path();
		wxSetWorkingDirectory(root.string());
		wxProcess proc(wxPROCESS_DEFAULT);
		wxExecute(cmd, wxEXEC_SYNC);
		reveal_in_explorer(std::filesystem::path(wxGetCwd().ToStdString()));
		wxSetWorkingDirectory(cwd);
	}
}

void DisplayLicenseOutput(){
    std::filesystem::path logfile
#ifdef __APPLE__
    = std::filesystem::path(wxGetHomeDir()) / "Library/Logs/Unity/Editor.log";
#elif defined _WIN32
    = std::filesystem::path(wxGetHomeDir().ToStdString()) / "AppData"/"Local"/"Unity"/"Editor"/"Editor.log";
#else
    = std::filesystem::path(wxGetHomeDir()) / ".config/unity3d/Editor.log";
#endif
    ifstream in(logfile);
    std::string output;
    std::string line;
    while (getline(in, line)){
        auto cpy = line;
        transform(cpy.begin(), cpy.end(), cpy.begin(), ::tolower);
        // filter for license-related lines
        if (cpy.find("licens") != decltype(line)::npos){
            output += line;
        }
    }
    auto dialog = new OutputDialog(nullptr, output, "Activation status");
    dialog->ShowModal();
}

void PersonalActivationDlg::OnActivateHit(wxCommandEvent&)
{
	wxFileDialog openFileDialog(this, _("Open License file"), "", "", "ULF files (*.ulf)|*.ulf", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {}
	else {
		auto exe = the_editor.executablePath().string();
		auto cmd = fmt::format("{} -batchmode -manualLicenseFile \"{}\" -logfile",exe,openFileDialog.GetPath().ToStdString());
        activateBtn->SetLabelText("Activating...");
		wxExecute(cmd, wxEXEC_SYNC);
        DisplayLicenseOutput();
		this->Close();
	}
}

void PlusProActivationDlg::OnActivateHit(wxCommandEvent&)
{
	const std::string& username = plusProActivUsernameCtrl->GetValue();
    const std::string& password = plusProActivPasswordCtrl->GetValue();
    const std::string& serial = plusProActivationSerialCtrl->GetValue();

	auto cmd = fmt::format("{} -batchmode -username {} -password {} -serial {} -quit",the_editor.executablePath().string(),username,password,serial);

    activateBtn->SetLabelText("Activating...");
	wxExecute(cmd, wxEXEC_SYNC);
    DisplayLicenseOutput();
	this->Close();
}
