//
//  interface_derived.cpp
//  mac
//
//  Created by Main on 9/5/19.
//  Copyright © 2019 Ravbug. All rights reserved.
//

#include "interface_derived.hpp"
#include <fstream>
#include <wx/msgdlg.h>
#include <wx/dirdlg.h>
#if defined _WIN32
#include "dirent.h" 
#else
#include <dirent.h>
#endif

//Declare events here
wxBEGIN_EVENT_TABLE(MainFrameDerived, wxFrame)
EVT_MENU(wxID_ABOUT, MainFrameDerived::OnAbout)
EVT_BUTTON(wxID_ADD,MainFrameDerived::OnAddProject)
EVT_BUTTON(wxID_NEW,MainFrameDerived::OnCreateProject)
EVT_BUTTON(wxID_FIND,MainFrameDerived::OnLocateInstall)
EVT_BUTTON(wxID_CLEAR,MainFrameDerived::OnRemoveInstallPath)
EVT_BUTTON(wxID_DELETE,MainFrameDerived::OnRemoveProject)
EVT_BUTTON(wxID_JUMP_TO,MainFrameDerived::OnRevealProject)
EVT_BUTTON(wxID_BACKWARD,MainFrameDerived::OnOpenHub)
EVT_BUTTON(wxID_RELOAD,MainFrameDerived::OnReloadEditors)
EVT_LIST_ITEM_ACTIVATED(wxID_HARDDISK, MainFrameDerived::OnOpenProject)
EVT_LIST_ITEM_ACTIVATED(wxID_FLOPPY,MainFrameDerived::OnRevealEditor)
EVT_LIST_ITEM_ACTIVATED(wxID_HOME,MainFrameDerived::OnRevealInstallLocation)
wxEND_EVENT_TABLE()

//call superclass constructor
MainFrameDerived::MainFrameDerived() : MainFrame(NULL){
	//set up project list columns
	{
		string cols[] = {"Project Name","Unity Version","Last Modified","Path"};
		for (string& str : cols){
			projectsList->AppendColumn(str,wxLIST_FORMAT_CENTER);
		}
	}
	//make the data folder if it does not already exist (with readwrite for all groups)
	#if defined __APPLE__ || defined __linux__
		int status = mkdir(datapath.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	#elif defined _WIN32
		int status = mkdir(datapath.c_str());
		//on windows also make the main window background white
		this->SetBackgroundColour(*wxWHITE);
		//high DPI scaling fixes
		dpi_scale(this);
	#endif
	if (status != 0){
		//check that projects file exists in folder
		string p = string(datapath + dirsep + projectsFile);
		if (file_exists(p)){
			ifstream in;
			in.open(p);
			string line;
			//if one cannot be loaded
			vector<string> erroredProjects;
			//load each project (each path is on its own line)
			while (getline(in, line)){
				try{
					project pr = LoadProject(line);
					AddProject(pr);
				}
				catch(runtime_error& e){
					//remove project
					erroredProjects.push_back(line);
				}
			}
			//alert user if projects could not be loaded
			if (erroredProjects.size() > 0){
				//build string
				string str;
				for (string s : erroredProjects){
					str += s + "\n";
				}
				//message box
				wxMessageBox("The following projects could not be loaded. They have been removed from the list.\n\n"+str, "Loading error", wxOK | wxICON_WARNING );
				
				//save to remove the broken projects
				SaveProjects();
			}
		}
		
		//check that the installs path file exists in the folder
		p = string(datapath + dirsep + editorPathsFile);
		if (file_exists(p)){
			//load the editors
			ifstream in; in.open(p); string line;
			while (getline(in, line)){
				LoadEditorPath(line);
			}
		}
		else{
			//add default data
			LoadEditorPath(defaultInstall);
		}
	}
	//if no projects to load, the interface will be blank
}

//definitions for the events
void MainFrameDerived::OnAbout(wxCommandEvent& event)
{
	string winfix = "";
#if defined _WIN32
	winfix = "About Unity Hub Native\n\n";
#endif
	wxMessageBox(winfix + "Unity Hub Native is a custom Unity Hub, designed to be more efficient than Unity's official hub. That hub is written in Electron, so it consumes more resources in all departments (CPU, GPU, RAM, Disk, Network) than an app with its job should. \n\nThis app is not a replacement for the Unity Hub, and never will be. It is for people who do not want to wait seconds before they can launch their Unity projects, or create new ones.\n\nVisit github.com/ravbug/UnityHubNative for more information and for updates. \n\nCreated by Ravbug, written in C++. Uses the wxWidgets GUI library.", "About Unity Hub Native", wxOK | wxICON_INFORMATION );
}

void MainFrameDerived::OnAddProject(wxCommandEvent& event){
	string msg ="Select the folder containing the Unity Project";
	string path = GetPathFromDialog(msg);
	if (path != ""){
		//check that the project does not already exist
		for(project& p : projects){
			if (p.path == path){
				wxMessageBox( "This project has already been added.", "Cannot add project", wxOK | wxICON_WARNING );
				return;
			}
		}
		
		//add it to the projects list
		try{
			project p = LoadProject(path);
			AddProject(p);
		}
		catch(runtime_error& e){
			wxMessageBox(e.what(),"Unable to add project",wxOK | wxICON_ERROR);
		}
	}
}

void MainFrameDerived::OnRemoveProject(wxCommandEvent& event){
	long itemIndex = wxListCtrl_get_selected(projectsList);
	if (itemIndex > -1){
		//remove from the vector
		projects.erase(projects.begin()+itemIndex);

		//remove from the list view
		projectsList->DeleteItem(itemIndex);
		
		//update the file
		SaveProjects();
	}
	else{
		wxMessageBox("You must select a project in the list before you can remove it from the list.", "No project selected", wxOK | wxICON_WARNING );
	}
}

/**
 Locates a Unity install path and adds it to the list and UI
 */
void MainFrameDerived::OnLocateInstall(wxCommandEvent& event){
	string msg = "Select the folder containing Unity installs";
	string path = GetPathFromDialog(msg);
	if (path != ""){
		LoadEditorPath(path);
	}
}

/**
 Loads an editor search path into the app, updating the UI and the vector
 @param path the string path to laod
 */
void MainFrameDerived::LoadEditorPath(const string& path){
	//add to internal structure and to file
	installPaths.insert(installPaths.begin(), path);
	SaveEditorVersions();
	
	//add to the UI
	wxListItem i;
	i.SetId(0);
	i.SetText(path);
	
	installsPathsList->InsertItem(i);
}

void MainFrameDerived::OnRemoveInstallPath(wxCommandEvent& event){
	long itemIndex = wxListCtrl_get_selected(installsPathsList);
	if (itemIndex > -1){
		// Got the selected item index
		//remove it from the vector
		installPaths.erase(installPaths.begin()+itemIndex);
		
		//update the UI
		installsPathsList->DeleteItem(itemIndex);
		
		//commit to file
		SaveEditorVersions();
		return;
	}
	wxMessageBox( "You must select an install path in the list before you can remove it.", "No path selected", wxOK | wxICON_WARNING );
}

/**
 Called to create a new project
 */
void MainFrameDerived::OnCreateProject(wxCommandEvent& event){
	//create a dialog and show it
	DialogCallback d = [&](string str, project p){
		//add the project
		this->AddProject(p);
		
		//launch the process
		launch_process(str);
	};
	CreateProjectDialogD* dialog = new CreateProjectDialogD(this,editors,d);
	dialog->show();
}

/**
 Called when you double click or press Enter on a cell in the ListView
 */
void MainFrameDerived::OnOpenProject(wxListEvent& event){
	OpenProject(event.m_itemIndex);
}

/**
 Called when an item in the installs pane is activated. Reveals that editor in the system's file browser.
 @param event the list event created by the wxListCtrl
 */
void MainFrameDerived::OnRevealEditor(wxListEvent& event){
	editor& e = editors[event.GetIndex()];
	string path = e.path + dirsep + e.name;
	reveal_in_explorer(path);
}

/**
 Called when an item in the install search paths pane is activated. Reveals that location in the system's file browser.
 @param event the list event created by the wxListCtrl
 */
void MainFrameDerived::OnRevealInstallLocation(wxListEvent& event){
	editor& e = editors[event.GetIndex()];
	string path = e.path;
	reveal_in_explorer(path);
}

/**
 Called when the reveal project button is clicked
 */
void MainFrameDerived::OnRevealProject(wxCommandEvent& event){
	long selectedIndex = wxListCtrl_get_selected(projectsList);
	if (selectedIndex > -1){
		project& p = projects[selectedIndex];
		reveal_in_explorer(p.path);
	}
}

void MainFrameDerived::OnOpenHub(wxCommandEvent& event){
	reveal_in_explorer(hubDefault);
}

/**
 Launches Unity with a project
 @param index the integer representing which project in the projects Vector to load
 */
void MainFrameDerived::OpenProject(const long& index){
	//get the project
	project p = projects[index];
	
	for(string path : installPaths){
		string editorPath = path + dirsep + p.version + dirsep + executable;
		
		//check that the unity editor exists at that location
		if (file_exists(editorPath)){
			
			//platform specific paths
			#if defined __APPLE__
			string cmd = "\"" + editorPath + "\" -projectpath \"" + p.path + "\"";
			#elif defined _WIN32
			string cmd = WinEscapePath(editorPath) + " -projectpath " + "\"" + p.path + "\"";
			#endif
			
			//start the process
			launch_process(cmd);
			
			return;
		}
	}
	//alert user
	wxMessageBox("The editor version " + p.version + " could not be found.\n\nCheck that it is installed, and that the folder where it has been installed is listed in the Editor Versions tab, under Install Search Paths.", "Unable to start Unity", wxOK | wxICON_ERROR);
	
}

/** Brings up a folder selection dialog with a prompt
 * @param message the prompt for the user
 * @return path selected, or an empty string if nothing chosen
 */
string MainFrameDerived::GetPathFromDialog(const string& message)
{
	//present the dialog
	wxDirDialog dlg(NULL, message, "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (dlg.ShowModal() == wxID_CANCEL) {
		return string("");
	}
	//get the path and return the standard string version
	wxString path = dlg.GetPath();
	return path.ToStdString();
}

/**
 Load a project into the struct
 @param path string path to the Unity project folder
 @return project struct with fields set based on Unity project folder
 @throws Exception if a file cannot be located in the Unity project, or if the file contains unexpected data.
 @note Surround this function in a try/catch, because it throws if it cannot succeed.
 */
project MainFrameDerived::LoadProject(const string &path){
	//error if the file does not exist
	if (!file_exists(path)){
		throw runtime_error(path + " does not exist.");
	}
	
	//the name is the final part of the path
	string name = path.substr(path.find_last_of(dirsep)+1);
	
	//Load ProjectSettings/ProjectVersion.txt to get the editor version, if it exists
	string projSettings = string(path + dirsep + "ProjectSettings" + dirsep + "ProjectVersion.txt");
	if (!file_exists(projSettings)){
		throw runtime_error("No ProjectVersion.txt found at " + path + "\n\nEnsure the folder you selected is the root folder of a complete Unity project.");
	}
	
	//the first line of ProjectVersion.txt contains the editor verison as plain text
	string version;
	ifstream inFile;
	inFile.open(projSettings);
	getline(inFile,version);
	version = version.substr(17);
	
	//get the modification date
	struct stat fileInfo;
	if (stat(path.c_str(), &fileInfo) != 0) {
		throw runtime_error("Cannot get modification date. Ensure this program has access to "+path);
	}
	
	project p = {name,version,ctime(&fileInfo.st_mtime),path,};
	return p;
}

/**
 Writes the project file paths to the file in the Application directory
 The file's name is projects.txt and will be created if it is not present.
 */
void MainFrameDerived::SaveProjects(){
	ofstream file;
	file.open(datapath + dirsep + projectsFile);
	for (project& p : projects){
		file << p.path << endl;
	}
	file.close();
}

void MainFrameDerived::SaveEditorVersions(){
	ofstream file;
	file.open(datapath + dirsep + editorPathsFile);
	for (string& p : installPaths){
		file << p << endl;
	}
	file.close();
	LoadEditorVersions();
}

/**
 Adds a project to the table
 @param p the project struct to add
 @note Ensure all the fields on the struct are initialized
 */
void MainFrameDerived::AddProject(const project& p){
	//add to the vector backing the UI
	projects.insert(projects.begin(),p);
	
	//save to file
	SaveProjects();
	
	//add (painfully) to the UI
	wxListItem i;
	i.SetId(0);
	i.SetText(p.name);
	
	projectsList->InsertItem(i);
	
	i.SetText(p.version);
	i.SetColumn(1);
	projectsList->SetItem(i);
	
	i.SetText(p.modifiedDate);
	
	i.SetColumn(2);
	projectsList->SetItem(i);
	
	i.SetColumn(3);
	i.SetText(p.path);
	projectsList->SetItem(i);
	
	//resize columns
	int cols = projectsList->GetColumnCount();
	for (int i = 0; i < cols; i++){
		projectsList->SetColumnWidth(i, wxLIST_AUTOSIZE);
	}
}

/**
 Loads all installed editor versions from the Install Search Paths, and updates the list control
 */
void MainFrameDerived::LoadEditorVersions(){
	//clear list control
	installsList->ClearAll();
	
	//iterate over the search paths
	for (string& path : installPaths){
		//open the folder
		DIR* dir = opendir(path.c_str());
		struct dirent *entry = readdir(dir);
		//loop over the contents
		while (entry != NULL)
		{
			//is this a folder?
			if (entry->d_type == DT_DIR){
				//does this folder have a valid executable inside?
				string p = string(path + dirsep + entry->d_name + dirsep + executable);
				if (file_exists(p)){
					//add it to the list
					installsList->InsertItem(0,string(entry->d_name) + " - " + path);
					
					//add it to the backing datastructure
					editor e = {entry->d_name, path};
					editors.insert(editors.begin(),e);
				}
			}
			entry = readdir(dir);
		}
		//free resources when finished
		closedir(dir);
		free(entry);
	}
}
