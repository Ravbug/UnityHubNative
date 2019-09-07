//
//  interface_derived.hpp
//  mac
//
//  Created by Main on 9/5/19.
//  Copyright © 2019 Ravbug. All rights reserved.
//

#pragma once

#include "interface.h"
#include "globals.cpp"
#include <functional>

using namespace std;

class MainFrameDerived : public MainFrame{
public:
	//constructor (takes no args)
	MainFrameDerived();
	
	static string GetPathFromDialog(string& message);
	
private:
	void AddProject(project& p);
	project LoadProject(string& path);
	void SaveProjects();
	void OpenProject(long& index);
	void SaveEditorVersions();
	void LoadEditorPath(const string& path);
	void LoadEditorVersions();
	
	//will store the list of projects
	vector<project> projects;
	vector<string> installPaths;
	vector<editor> editors;
	
	//events
	void OnAbout(wxCommandEvent& event);
	void OnAddProject(wxCommandEvent& event);
	void OnCreateProject(wxCommandEvent& event);
	void OnRemoveProject(wxCommandEvent& event);
	void OnOpenProject(wxListEvent& event);
	void OnLocateInstall(wxCommandEvent& event);
	void OnRemoveInstallPath(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};

class CreateProjectDialogD : CreateProjectDialog{
public:
	CreateProjectDialogD(wxWindow* parent, vector<editor>& versions);
	void show(){
		this->ShowModal();
	}
private:
	string validateForm();
	void loadTemplates(editor& e);
	//std::function<const string&> callback;
	vector<editor> editors;
	
	//events
	void OnCancel(wxCommandEvent& event){
		//close and dispose self
		this->EndModal(0);
		delete this;
	}
	void OnCreate(wxCommandEvent& event);
	void setProjectPath(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};
