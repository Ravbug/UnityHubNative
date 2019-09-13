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
	
	static string GetPathFromDialog(const string& message);
	
private:
	void AddProject(const project& p);
	project LoadProject(const string& path);
	void SaveProjects();
	void OpenProject(const long& index);
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
	void OnRevealEditor(wxListEvent& event);
	void OnRevealInstallLocation(wxListEvent& event);
	void OnRevealProject(wxCommandEvent& event);
	void OnOpenHub(wxCommandEvent& event);
	void OnReloadEditors(wxCommandEvent& event){
		this->LoadEditorVersions();
	}
	wxDECLARE_EVENT_TABLE();
};

typedef std::function<void(const string&,const project&)> DialogCallback;
class CreateProjectDialogD : CreateProjectDialog{
public:
	CreateProjectDialogD(wxWindow* parent, const vector<editor>& versions, const DialogCallback& callback);
	void show(){
		this->ShowModal();
	}
private:
	string validateForm();
	void loadTemplates(const editor& e);
	DialogCallback callback;
	vector<editor> editors;
	
	//events
	void OnCancel(wxCommandEvent& event){
		//close and dispose self
		this->EndModal(0);
		this->Destroy();
	}
	void OnCreate(wxCommandEvent& event);
	void setProjectPath(wxCommandEvent& event);
	void OnChoiceChanged(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};
