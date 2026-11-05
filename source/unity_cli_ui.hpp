//
//  unity_cli_ui.hpp
//  Unity Hub Native
//
//  The main window's Unity CLI buttons and prompts: installing or updating the CLI,
//  and signing it in to the user's Unity account.
//

#pragma once

#include "unity_cli.hpp"
#include <wx/tracker.h>

class wxButton;
class wxWindow;

class UnityCliController : public wxTrackable {
public:
	/**
	 @param parent the window that the controller's dialogs belong to
	 @param installButton installs or updates the CLI
	 @param loginButton signs the CLI in
	 */
	UnityCliController(wxWindow* parent, wxButton* installButton, wxButton* loginButton);

	// The Unity CLI found by the last Refresh()
	const UnityCliStatus& Status() const {
		return status;
	}

	/**
	 Looks for the Unity CLI again, and updates the buttons. Install is available while there
	 is no recent enough CLI, Login while the CLI is not signed in.
	 */
	void Refresh();

	/**
	 Explains the Unity CLI and offers to install it, unless it is ready or the user turned this off
	 */
	void OfferInstall();

	/**
	 Installs or updates the Unity CLI, then helps the user sign in
	 */
	void Install();

	/**
	 Signs the Unity CLI in to the user's Unity account through the browser
	 */
	void SignIn();

private:
	// The label for installing the CLI, or updating it if it is too old
	const char* InstallLabel() const;

	wxWindow* parent;
	wxButton* installButton;
	wxButton* loginButton;
	UnityCliStatus status;
};
