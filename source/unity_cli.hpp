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

struct UnityCliStatus;

struct UnityCli {
	std::filesystem::path executable;

	/**
	 Finds the installed Unity CLI and checks its version. This runs the CLI, briefly blocking.
	 */
	static UnityCliStatus Probe();

	/**
	 Installs or updates the CLI with Unity's official installer, showing a progress window
	 @param done called with whether the installer succeeded
	 */
	static void Install(wxWindow* parent, std::function<void(bool)> done);

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

// What Unity CLI there is on this computer
struct UnityCliStatus {
	enum class State {
		Missing,     // not installed
		Outdated,    // installed, but too old to sign the Editor in without the Hub
		Ready,       // installed and recent enough
	};
	State state = State::Missing;
	// the installed CLI's version, empty if it is missing or its version could not be read
	std::string version;
	// the CLI to use, set only when it is Ready
	std::optional<UnityCli> cli;
};

/**
 Opens project p with editor e, without blocking. Launched directly, the Editor starts signed out
 and starts the official Hub to sign in, so this opens it through the Unity CLI when there is a
 Ready one that can open p with e. If the CLI fails, it shows why and offers to launch the
 Editor directly instead.
 @param cli the Unity CLI on this computer
 @param buildTarget the value to pass to -buildTarget, or empty for none
 */
void OpenProjectInEditor(const UnityCliStatus& cli, const project& p, const editor& e, std::string_view buildTarget = {});

/**
 Creates project p with editor e from a template, then opens it, without blocking. Through a
 Ready Unity CLI that can open p with e, the Editor creates the project in batch mode, showing a
 progress window, and the project then opens through the CLI, signed in. Otherwise the Editor
 creates and opens it directly.
 @param cli the Unity CLI on this computer
 @param templatePath the template package to create the project from
 */
void CreateProjectInEditor(wxWindow* parent, const UnityCliStatus& cli, const project& p, const editor& e, const std::filesystem::path& templatePath);
