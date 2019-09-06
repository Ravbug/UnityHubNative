//
//  interface_derived.cpp
//  mac
//
//  Created by Main on 9/5/19.
//  Copyright © 2019 Ravbug. All rights reserved.
//

#include "interface_derived.hpp"
#include <fstream>

//Declare events here
wxBEGIN_EVENT_TABLE(MainFrameDerived, wxFrame)
EVT_MENU(wxID_ABOUT, MainFrameDerived::OnAbout)
EVT_BUTTON(wxID_ADD,MainFrameDerived::OnAddProject)
EVT_BUTTON(wxID_NEW,MainFrameDerived::OnCreateProject)
EVT_BUTTON(wxID_FIND,MainFrameDerived::OnLocateInstall)
EVT_BUTTON(wxID_CLEAR,MainFrameDerived::OnRemoveInstallPath)
EVT_LIST_ITEM_ACTIVATED(wxID_HARDDISK, MainFrameDerived::OnOpenProject)
wxEND_EVENT_TABLE()

//call superclass constructor
MainFrameDerived::MainFrameDerived() : MainFrame(NULL){
	//set up project list columns
	string cols[] = {"Project Name","Unity Version","Last Modified","Path"};
	for (string& str : cols){
		projectsList->AppendColumn(str,wxLIST_FORMAT_CENTER,wxLIST_AUTOSIZE_USEHEADER);
	}
	
	//make the data folder if it does not already exist (with readwrite for all groups)
	int status = mkdir(datapath.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (status != 0){
		//check that projects file exists in folder
		string p = string(datapath + dirsep + projectsFile);
		if (file_exists(p)){
			ifstream in;
			in.open(p);
			string line;
			//load each project (each path is on its own line)
			while (getline(in, line)){
				project pr = LoadProject(line);
				AddProject(pr);
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
	wxMessageBox( "More info coming soon", "About Unity Hub Native", wxOK | wxICON_INFORMATION );
}

void MainFrameDerived::OnAddProject(wxCommandEvent& event){
	string msg ="Select the folder containing the Unity Project";
	string path = GetPathFromDialog(msg);
	if (path != ""){
		//add it to the projects list
		project p = LoadProject(path);
		AddProject(p);
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
	//loop over the list control because it doesn't have a function to get the selected ID
	long itemIndex = -1;
	while ((itemIndex = installsPathsList->GetNextItem(itemIndex,wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != wxNOT_FOUND) {
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
	wxMessageBox( "Replace with project creation dialog", "Create Project", wxOK | wxICON_INFORMATION );
}

/**
 Called when you double click or press Enter on a cell in the ListView
 */
void MainFrameDerived::OnOpenProject(wxListEvent& event){
	OpenProject(event.m_itemIndex);
}

/**
 Launches Unity with a project
 @param index the integer representing which project in the projects Vector to load
 */
void MainFrameDerived::OpenProject(long& index){
	//get the project
	project p = projects[index];
	
	for(string path : installPaths){
		string editorPath = path + dirsep + p.version + dirsep + executable;
		
		//check that the unity editor exists at that location
		if (file_exists(editorPath)){
			string cmd = editorPath + " -projectpath " + p.path;
			
			//start the process
			//TODO: don't use fork, it is unsafe
			int status = fork();
			if (status == 0){
				//TODO: find a method that does not use system(), some antivirus don't like it
				system(cmd.c_str());
			}
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
string MainFrameDerived::GetPathFromDialog(string& message)
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
project MainFrameDerived::LoadProject(string &path){
	//error if the file does not exist
	if (!file_exists(path)){
		throw exception();
	}
	
	//the name is the final part of the path
	string name = path.substr(path.find_last_of(dirsep)+1);
	
	//Load ProjectSettings/ProjectVersion.txt to get the editor version, if it exists
	string projSettings = string(path + dirsep + "ProjectSettings" + dirsep + "ProjectVersion.txt");
	if (!file_exists(projSettings)){
		throw exception();
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
		throw exception();
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
}

/**
 Adds a project to the table
 @param p the project struct to add
 @note Ensure all the fields on the struct are initialized
 */
void MainFrameDerived::AddProject(project& p){
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
}
