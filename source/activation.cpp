#include "interface_derived.hpp"
#include "activation.hpp"
#include "fmt/format.h"
#include <wx/process.h>
#include <wx/filedlg.h>

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
		dlg->Show();
	}
}

void MainFrameDerived::OnActivateProPlus(wxCommandEvent& event) {
	auto dlg = new PlusProActivationDlg(this, editor{});
	dlg->Show();
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
		reveal_in_explorer(wxGetCwd());
		wxSetWorkingDirectory(cwd);
	}
}

void PersonalActivationDlg::OnActivateHit(wxCommandEvent&)
{
	wxFileDialog openFileDialog(this, _("Open License file"), "", "", "ULF files (*.ulf)|*.ulf", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {}
	else {
		auto exe = the_editor.executablePath().string();
		auto cmd = fmt::format("{} -batchmode -manualLicenseFile \"{}\" -logfile",exe,openFileDialog.GetPath().ToStdString());
		wxExecute(cmd, wxEXEC_SYNC);
	}
}

void PlusProActivationDlg::OnActivateHit(wxCommandEvent&)
{
	
}
