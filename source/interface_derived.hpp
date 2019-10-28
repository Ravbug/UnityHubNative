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
	void OpenProject(const project& p, const editor& e);
	void SaveEditorVersions();
	void LoadEditorPath(const string& path);
	void LoadEditorVersions();
	void ReloadData();
	
	//will store the list of projects
	vector<project> projects;
	vector<string> installPaths;
	vector<editor> editors;
	
	//events
	void OnAbout(wxCommandEvent& event);
	void OnAddProject(wxCommandEvent& event);
	void OnCreateProject(wxCommandEvent& event);
	void OnRemoveInstallPath(wxCommandEvent& event);
	void OnRevealProject(wxCommandEvent& event);
	void OnOpenWith(wxCommandEvent& event);
	void OnReloadData(wxCommandEvent& event){
		ReloadData();
	}
	void OnRemoveProject(wxCommandEvent& event){
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
	Called when you double click or press Enter on a cell in the ListView
	*/
	void OnOpenProject(wxListEvent& event){
		OpenProject(event.m_itemIndex);
	}
	/**
	Locates a Unity install path and adds it to the list and UI
	*/
	void OnLocateInstall(wxCommandEvent& event){
		string msg = "Select the folder containing Unity installs";
		string path = GetPathFromDialog(msg);
		if (path != ""){
			LoadEditorPath(path);
		}
	}
	
	/**
	Called when an item in the installs pane is activated. Reveals that editor in the system's file browser.
	@param event the list event created by the wxListCtrl
	*/
	void OnRevealEditor(wxListEvent& event){
		editor& e = editors[event.GetIndex()];
		string path = e.path + dirsep + e.name;
		reveal_in_explorer(path);
	}
	/**
	Called when an item in the install search paths pane is activated. Reveals that location in the system's file browser.
	@param event the list event created by the wxListCtrl
	*/
	void OnRevealInstallLocation(wxListEvent& event){
		editor& e = editors[event.GetIndex()];
		string path = e.path;
		reveal_in_explorer(path);
	}
	void OnOpenHub(wxCommandEvent& event){
		reveal_in_explorer(hubDefault);
	}
	void OnReloadEditors(wxCommandEvent& event){
		this->LoadEditorVersions();
	}
	wxDECLARE_EVENT_TABLE();
};

/**
 CreateProjectDialog derived class
 Defines the functionality for the project creation dialog
 */
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

typedef std::function<void(const project&, const editor&)> OpenWithCallback;
class OpenWithDlg : OpenWithEditorDlgBase{
public:
	OpenWithDlg(wxWindow* parent, const project& project, const vector<editor>& versions,  const OpenWithCallback& callback);
	void show(){
		this->ShowModal();
	}
private:
	vector<editor> editors;
	project p;
	OpenWithCallback callback;
	
	//events
	void OnCancel(wxCommandEvent& event){
		//close and dispose self
		this->EndModal(0);
		this->Destroy();
	}
	void OnSelect(wxListEvent& event){
		openBtn->Enable(true);
	}
	void OnDeselect(wxListEvent& event){
		openBtn->Enable(false);
	}
	
	void OnOpen(wxCommandEvent& event);
	
	wxDECLARE_EVENT_TABLE();
};
