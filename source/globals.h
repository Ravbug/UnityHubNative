//
//  globals.cpp
//  Unity Hub Native (Dynamic)
//
//  Copyright © 2019 Ravbug. All rights reserved.
//
#pragma once
using namespace std;

#include <sys/stat.h>
#include <wx/listctrl.h>
#include <string>


//data file names
static const string projectsFile = "projects.txt";
static const string editorPathsFile = "editorPaths.txt";
static const string templatePrefix = "com.unity.template";
static const string AppVersion = "v1.36";

//structure for representing an editor and for locating it
struct editor{
	string name;
	string path;
};

#if defined __APPLE__
	#include <pwd.h>
	//the location to store application data
	static const string datapath = getpwuid(getuid())->pw_dir + string("/Library/Application Support/UnityHubNative");
	static const char dirsep = '/';

	//where to find various Unity things on macOS
	static const string executable = "Unity.app/Contents/MacOS/Unity";
	static const string defaultInstall = "/Applications/Unity/Hub/Editor";
	//TODO: make this a preference?
	static const string hubDefault = "/Applications/Unity Hub.app";
	static const string templatesDir = "Unity.app/Contents/Resources/PackageManager/ProjectTemplates/";

	//for stream redirecting to dev/null
	static const string null_device = ">/dev/null 2>&1";

#elif defined _WIN32
//naming conflicts
#define popen _popen
#define pclose _pclose
#define mkdir _mkdir
#include <windows.h>
#include <gdiplus.h>	
#include <wx/wx.h>
	static const string datapath = getenv("HOMEPATH") + string("\\AppData\\Roaming\\UnityHubNative");
	static const char dirsep = '\\';

	//where to find various Unity things on windows
	static const string executable = "Editor\\Unity.exe";
	static const string defaultInstall = "\\Program Files\\Unity\\Hub\\Editor";
	
	static const string hubDefault = "\\Program Files\\Unity Hub\\Unity Hub.exe";
	static const string templatesDir = "Editor\\Data\\Resources\\PackageManager\\ProjectTemplates\\";
	
	/**
	@returns the calculated display scale factor using GDI+
	*/
	inline float get_WIN_dpi_multiple() {
		FLOAT dpiX;
		HDC screen = GetDC(0);
		dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
		ReleaseDC(0, screen);
		return dpiX / 96;
	}

	/**
	Scales a wxWindow to the correct size using the monitor's DPI factor (Windows only)
	This preserves the defined size of the window. To simply fit the window to contents, regardless
	of DPI, use fitWindowMinSize.
	@param window the wxWindow to scale
	*/
	inline void dpi_scale(wxWindow* window) {
		//fit size to children
		window->Fit();

		//calculate the scaled min size
		float fac = get_WIN_dpi_multiple();
		float minh = window->GetMinHeight() * fac;
		float minw = window->GetMinWidth() * fac;
		//set the minimum size
		window->SetSizeHints(wxSize(minw,minh));
	}

	/**
	Replaces all instances of a string with another string, within a string
	@param str the string to operate on
	@param from the string to be replaced
	@param to the string to replace `from` with
	*/
	inline string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
		}
		return str;
	}

	/**
	Escapes Windows paths by replacing spaces ' ' with  '^ '
	@param path the windows path to escape
	@return the escaped path
	*/
	inline string WinEscapePath(string& path) {
		return ReplaceAll(path, string(" "), string("^ "));
	}

#elif defined __linux__
	#include <pwd.h>
	static const string datapath = getpwuid(getuid())->pw_dir + string("/UnityHubNative");
	static const string null_device = ">/dev/null 2>&1";
	static const char dirsep = '/';
	
	static const string executable = "Editor/Unity";
	static const string defaultInstall =  getpwuid(getuid())->pw_dir +string("/Unity/Hub/Editor");
	//TODO: make this a preference?
	static const string hubDefault = "/Applications/Unity Hub.app";
	static const string templatesDir = "Unity.app/Contents/Resources/PackageManager/ProjectTemplates/";
	
#else
	//disalow compilation for unsupported platforms
#error You are compiling on an unsupported operating system. Currently only macOS and Windows are supported. If you know how to support your system, submit a pull request.
#endif

//structure containing all the info needed to display a project
struct project{
	string name;
	string version;
	string modifiedDate;
	string path;
};

/**
 Determines if a file exists at a path using stat()
 @param name the path to the file
 @return true if the file exists, false if it does not
 */
inline bool file_exists(const string& name){
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}


/**
 Launches a shell command as a separate, non-connected process. The output of this
 command is not captured (sent to the system's null device)
 @param command the shell command to run on the system
 @note The command passed to this function must be correct for the system it is running on. If it is not correct, the function will appear to do nothing.
 */
inline void launch_process(const string& command) {
#if defined __APPLE__ || defined __linux__
	//the '&' runs the command nonblocking, and >/dev/null 2>&1 destroys output
	FILE* stream = popen(string(command + null_device + " &").c_str(), "r");
	pclose(stream);

#elif _WIN32
	//call wxExecute with the Async flag
	wxExecute(wxString(command),0);

#endif
}

inline void reveal_in_explorer(const string& path){
#if defined __APPLE__
	string command = "open \"" + path + "\"";

#elif defined __linux__
	string command = "xdg-open \"" + path + "\"";
	
#elif defined _WIN32
	//do not surround the paths in quotes, it will not work
	string command = "\\Windows\\explorer.exe \"" + path + "\"";
#endif
	launch_process(command);
}

/**
 Gets the first selected item in a wxListCtrl. If the wxListCtrl is set to single selection, this method will retrive the only selected item.
 @param listCtrl pointer to the wxListCtrl to get the selected item from.
 @return index of the first selected item, or -1 if no item is selected.
 @note wxListCtrl does not have a built in method for this.
 */
inline long wxListCtrl_get_selected(wxListCtrl* listCtrl){
	long itemIndex = -1;
	while ((itemIndex = listCtrl->GetNextItem(itemIndex,wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != wxNOT_FOUND) {
		break;
	}
	return itemIndex;
}

/**
Fits a wxWindow to its contents, and then sets that size as the window's minimum size
@param window the wxWindow to apply size changes
*/
inline void fitWindowMinSize(wxWindow* window) {
	//fit size to children
	window->Fit();

	//constrain minimum size to the minimum fitting size
	wxSize size = window->GetSize();
	window->SetSizeHints(size);
}
