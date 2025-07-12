//
//  Unity Hub Native (Dynamic)
//
//  Copyright © 2019 Ravbug. All rights reserved.
//
#pragma once

#include <sys/stat.h>
#include <string>
#include <filesystem>
#include <vector>

//data file names
static constexpr std::string_view projectsFile = "projects.txt";
static constexpr std::string_view editorPathsFile = "editorPaths.txt";
static constexpr std::string_view templatePrefix = "com.unity.template";
static constexpr std::string_view AppVersion = "v1.58";

struct wxListCtrl;
struct wxWindow;

#if defined __APPLE__
	#include <pwd.h>
    #include <unistd.h>
	//the location to store application data
	static const std::filesystem::path datapath = std::filesystem::path(getpwuid(getuid())->pw_dir) / "Library/Application Support/UnityHubNative";

    static const std::filesystem::path cachedir = std::filesystem::path(getpwuid(getuid())->pw_dir) / "/Library/Caches/com.ravbug.UnityHubNative/";
	static constexpr std::string_view installerExt = "dmg";

	//where to find various Unity things on macOS
	static const std::filesystem::path executable = "Contents/MacOS/Unity";
    static const std::vector<std::filesystem::path> defaultInstall = {"/Applications/Unity/Hub/Editor","/Applications/Unity/"};
	//TODO: make this a preference?
	static const std::filesystem::path hubDefault = "/Applications/Unity Hub.app";
	static const std::filesystem::path templatesDir = "Contents/Resources/PackageManager/ProjectTemplates/";

	//for stream redirecting to dev/null
	static constexpr std::string_view null_device = ">/dev/null 2>&1";

#elif defined _WIN32
//naming conflicts
#define popen _popen
#define pclose _pclose
#define mkdir _mkdir
	static const std::filesystem::path homepath = getenv("HOMEPATH");
	static const std::filesystem::path homedrive = getenv("HOMEDRIVE");
	static const std::filesystem::path homedir = homedrive / homepath;

	static const std::filesystem::path datapath = homedir / std::filesystem::path("AppData\\Roaming\\UnityHubNative");

	static const std::filesystem::path cachedir = std::filesystem::temp_directory_path();
	static constexpr std::string_view installerExt = "exe";

	//where to find various Unity things on windows
	static const std::filesystem::path executable = "Editor\\Unity.exe";
    static const std::vector<std::filesystem::path> defaultInstall = { homedrive / "\\Program Files\\Unity\\Hub\\Editor"};
	
	static const std::filesystem::path hubDefault = homedrive / "\\Program Files\\Unity Hub\\Unity Hub.exe";
	static const std::filesystem::path templatesDir = "Editor\\Data\\Resources\\PackageManager\\ProjectTemplates\\";

	/**
	Scales a wxWindow to the correct size using the monitor's DPI factor (Windows only)
	This preserves the defined size of the window. To simply fit the window to contents, regardless
	of DPI, use fitWindowMinSize.
	@param window the wxWindow to scale
	*/
	void dpi_scale(wxWindow* window);

#elif defined __linux__
	#include <pwd.h>
    #include <unistd.h>
	static const std::filesystem::path datapath = std::filesystem::path(getpwuid(getuid())->pw_dir) / "UnityHubNative";
	static constexpr std::string_view null_device = ">/dev/null 2>&1";
	
	static const std::filesystem::path executable = "Editor/Unity";
    static const std::vector<std::filesystem::path> defaultInstall = {std::filesystem::path(getpwuid(getuid())->pw_dir) / "Unity/Hub/Editor"};
	//TODO: make this a preference?
	static const std::filesystem::path hubDefault = "";
	static const std::filesystem::path templatesDir = "Editor/Data/Resources/PackageManager/ProjectTemplates/";
	static const std::filesystem::path cachedir = std::filesystem::temp_directory_path();
#else
	//disalow compilation for unsupported platforms
#error You are compiling on an unsupported operating system. Currently only macOS, Windows, and Linux are supported. If you know how to support your system, submit a pull request.
#endif

/**
 Launches a shell command as a separate, non-connected process. The output of this
 command is not captured (sent to the system's null device)
 @param command the shell command to run on the system
 @note The command passed to this function must be correct for the system it is running on. If it is not correct, the function will appear to do nothing.
 */
void launch_process(const std::string& command, int flags = 0);

/**
* Open system file explorer to path
* @param path the item to show
*/
void reveal_in_explorer(const std::filesystem::path& path);

/**
 Gets the first selected item in a wxListCtrl. If the wxListCtrl is set to single selection, this method will retrive the only selected item.
 @param listCtrl pointer to the wxListCtrl to get the selected item from.
 @return index of the first selected item, or -1 if no item is selected.
 @note wxListCtrl does not have a built in method for this.
 */
long wxListCtrl_get_selected(wxListCtrl* listCtrl);

/**
Fits a wxWindow to its contents, and then sets that size as the window's minimum size
@param window the wxWindow to apply size changes
*/
void fitWindowMinSize(wxWindow* window);

//structure for representing an editor and for locating it
struct editor {
	std::string name;
	std::filesystem::path path;
	decltype(path) executablePath() const {
		return path / executable;
	}
    
    auto templatePath() const{
        return path / templatesDir;
    }

	bool operator==(const editor& other) {
		return this->name == other.name;    // many editors can share a root path
	}
};

//structure containing all the info needed to display a project
struct project {
	std::string name;
	std::string version;
	std::string modifiedDate;
	std::filesystem::path path;
	bool operator==(const project& other) const {
		return this->path == other.path;
	}
};
