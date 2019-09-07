//
//  globals.cpp
//  Unity Hub Native (Dynamic)
//
//  Created by Main on 9/5/19.
//  Copyright © 2019 Ravbug. All rights reserved.
//

using namespace std;

#include <sys/stat.h>

//data file names
static const string projectsFile = "projects.txt";
static const string editorPathsFile = "editorPaths.txt";

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
	static const string executable = "Unity.app/Contents/MacOS/Unity";

	static const string defaultInstall = "/Applications/Unity/Hub/Editor";


#elif defined __WIN32__
	static const string datapath = getenv("HOMEPATH") + string("\\AppData\\Roaming\\UnityHubNative");
	static const char dirsep = '\\';

#else
	//disalow compilation for unsupported platforms
#error You are compiling on an unsupported operating system. Currently only macOS and Windows are supported. If you know how to support your system, you may add that functionality and submit a pull request.
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
