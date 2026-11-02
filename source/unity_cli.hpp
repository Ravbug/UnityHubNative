//
//  unity_cli.hpp
//  Unity Hub Native
//
//  Opens projects through the official Unity CLI (`unity open`). The CLI serves
//  the Editor's sign-in itself, so the Editor starts signed in without the
//  official Unity Hub running (or being started in the background).
//

#pragma once

#include "globals.h"
#include <functional>
#include <optional>
#include <string_view>

class wxWindow;

struct UnityCli {
	std::filesystem::path executable;
	std::string version;
	// whether this version can sign the Editor in without the Hub
	bool recentEnough = false;

	/**
	 Finds the installed Unity CLI, whatever its version
	 @return the CLI, or nullopt if it is not installed
	 */
	static std::optional<UnityCli> Locate();

	/**
	 Finds an installed Unity CLI recent enough to sign the Editor in without the Hub
	 @return the CLI, or nullopt if it is not installed or too old
	 */
	static std::optional<UnityCli> Find();

	/**
	 Whether project p should be opened with editor e through the CLI. Older Editors
	 sign in on their own, and the CLI refuses some version changes.
	 */
	static bool CanOpen(const project& p, const editor& e);

	/**
	 Installs or updates the CLI with Unity's official installer, showing a progress window
	 @param done called with whether the installer succeeded
	 */
	static void Install(wxWindow* parent, std::function<void(bool)> done);

	/**
	 Opens a project with `unity open` without blocking. If the CLI fails, shows its
	 message and offers to launch the Editor directly instead.
	 @param p the project to open
	 @param e the editor to open it with
	 @param buildTarget the value to pass to -buildTarget, or empty for none
	 @param launchDirectly opens the project without the CLI
	 */
	void Open(const project& p, const editor& e, std::string_view buildTarget, std::function<void()> launchDirectly) const;

	/**
	 Checks whether the CLI is signed in to a Unity account, without blocking
	 @param done called with true if it is signed in
	 */
	void CheckSignedIn(std::function<void(bool)> done) const;

	/**
	 Signs the CLI in through the browser (`unity auth login`), showing a progress window the user can cancel
	 @param done called with whether the user signed in
	 */
	void SignIn(wxWindow* parent, std::function<void(bool)> done) const;
};
