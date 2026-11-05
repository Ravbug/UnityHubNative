//
//  unity_cli_ui.cpp
//  Unity Hub Native
//

#include "unity_cli_ui.hpp"
#include <wx/button.h>
#include <wx/config.h>
#include <wx/msgdlg.h>
#include <wx/richmsgdlg.h>
#include <wx/weakref.h>

namespace {
	constexpr std::string_view config_hideCliPrompt = "hideUnityCliPrompt";
}

UnityCliController::UnityCliController(wxWindow* parent, wxButton* installButton, wxButton* loginButton)
	: parent(parent), installButton(installButton), loginButton(loginButton) {}

const char* UnityCliController::InstallLabel() const {
	return status.state == UnityCliStatus::State::Outdated ? "Update CLI" : "Install CLI";
}

void UnityCliController::Refresh() {
	status = UnityCli::Probe();
	installButton->SetLabel(InstallLabel());
	installButton->Enable(!status.cli);
	loginButton->Enable(false);
	installButton->GetParent()->Layout();
	if (!status.cli) {
		return;
	}
	wxWeakRef<UnityCliController> self(this);
	status.cli->CheckSignedIn([self](bool signedIn) {
		if (self) {
			self->loginButton->Enable(!signedIn);
		}
	});
}

void UnityCliController::OfferInstall() {
	wxConfig config(config_name.data());
	if (config.ReadBool(config_hideCliPrompt.data(), false)) {
		return;
	}
	if (status.cli) {
		return;
	}

	const wxString current = status.state == UnityCliStatus::State::Outdated
		? wxString::Format("The Unity CLI on this computer (version %s) is too old for this.", status.version.empty() ? "unknown" : status.version)
		: wxString("The Unity CLI isn't installed yet.");
	// wxRichMessageDialog lays this out with wxTextSizerWrapper, which drops empty
	// lines, so a blank line has to contain a character or the gap disappears.
	const wxString paragraphGap = "\n \n";
	const wxString message =
		"Unity Hub Native can open your projects signed in to your Unity account, so the Asset Store, "
		"Package Manager and other Unity services work, without starting the official Unity Hub. "
		"It does this through Unity's official command-line tool, the Unity CLI."
		+ paragraphGap + " > " + current + paragraphGap +
		"Installing runs Unity's installer from unity.com. It puts the unity command in your user folder "
		"and adds it to your shell's PATH. After that, you sign in to your Unity account once in your browser."
		+ paragraphGap +
		"You can also do this later from the Editor Versions tab." + paragraphGap;

	wxRichMessageDialog dialog(parent, message, "Open Projects Signed In", wxYES_NO | wxICON_INFORMATION);
	dialog.SetYesNoLabels(InstallLabel(), "Not Now");
	dialog.ShowCheckBox("Don't show this again");
	// Escape (and closing the window) answers Not Now; with no Cancel button it would do nothing
	dialog.SetEscapeId(wxID_NO);
	const bool install = dialog.ShowModal() == wxID_YES;
	if (dialog.IsCheckBoxChecked()) {
		config.Write(config_hideCliPrompt.data(), true);
	}
	if (install) {
		Install();
	}
}

void UnityCliController::Install() {
	wxWeakRef<UnityCliController> self(this);
	UnityCli::Install(parent, [self](bool installed) {
		if (!self) {
			return;
		}
		self->Refresh();
		if (!installed) {
			return;
		}
		const auto& cli = self->status.cli;
		if (!cli) {
			wxMessageBox("The installer finished, but Unity Hub Native cannot find a recent enough Unity CLI.", "Cannot Install Unity CLI", wxOK | wxICON_ERROR, self->parent);
			return;
		}
		cli->CheckSignedIn([self](bool signedIn) {
			if (!self) {
				return;
			}
			if (signedIn) {
				wxMessageBox("The Unity CLI is installed and signed in. Projects now open signed in to your Unity account.", "Unity CLI Installed", wxOK | wxICON_INFORMATION, self->parent);
			}
			else if (wxMessageBox("The Unity CLI is installed. Sign in to your Unity account now, so projects open signed in?", "Unity CLI Installed", wxYES_NO | wxICON_QUESTION, self->parent) == wxYES) {
				self->SignIn();
			}
		});
	});
}

void UnityCliController::SignIn() {
	if (!status.cli) {
		Refresh();
		return;
	}
	wxWeakRef<UnityCliController> self(this);
	status.cli->SignIn(parent, [self](bool) {
		if (self) {
			self->Refresh();
		}
	});
}
