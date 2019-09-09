//
//  globals.cpp
//  Unity Hub Native (Dynamic)
//
//  Created by Main on 9/5/19.
//  Copyright © 2019 Ravbug. All rights reserved.
//

using namespace std;

#include <sys/stat.h>
#include <wx/listctrl.h>

//data file names
static const string projectsFile = "projects.txt";
static const string editorPathsFile = "editorPaths.txt";
static const string templatePrefix = "com.unity.template";

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
	static const string datapath = getenv("HOMEPATH") + string("\\AppData\\Roaming\\UnityHubNative");
	static const char dirsep = '\\';

	//where to find various Unity things on windows
	static const string executable = "Editor\\Unity.exe";
	static const string defaultInstall = "C:\\Program Files\\Unity\\Hub\\Editor";
	
	static const string hubDefault = "C:\\Program Files\\Unity Hub\\Unity Hub.exe";
	static const string templatesDir = "Editor\\Data\\Resources\\PackageManager\\ProjectTemplates\\libcache\\";

	static const string null_device = "";

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
inline bool file_exists(string& name){
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}

/**
 Launches a shell command as a separate, non-connected process. The output of this
 command is not captured (sent to the system's null device)
 @param command the shell command to run on the system
 @note The command passed to this function must be correct for the system it is running on. If it is not correct, the function will appear to do nothing.
 */
inline void launch_process(string& command){
	//the '&' runs the command nonblocking, and >/dev/null 2>&1 destroys output
	FILE* stream = popen(string(command + null_device + " &").c_str(),"r");
	//close stream, since Unity's output does not matter
	pclose(stream);
}

inline void reveal_in_explorer(const string& path){
#if defined __APPLE__
	string command = "open \"" + path + "\"";
	launch_process(command);
#elif defined _WIN32
#endif
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
