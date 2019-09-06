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
	
	string editorPath = installsPath + dirsep + p.version + dirsep + executable;
	
	//check that the unity editor exists at that location
	if (file_exists(editorPath)){
		string cmd = editorPath + " -projectpath " + p.path;
		
		//start the process
		int status = fork();
		if (status == 0){
			//find a method that does not use system(), some antivirus don't like that
			system(cmd.c_str());
		}
	}
	else{
		//alert user
		wxMessageBox("The editor version " + p.version + " could not be found. Check that it is installed.", "Unable to start Unity", wxOK | wxICON_ERROR);
	}
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
