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
#include <wx/webview.h>
#include <wx/timer.h>

#define TIMER 2001


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
	
	wxWebView* learnView = NULL;
	const string homeurl = "https://learn.unity.com";
	wxString lastURL = wxString(homeurl);
	wxTimer timeout = wxTimer(this,TIMER);
	
	//webview events
	/** Backwards button hit */
	void OnNavigateBack(wxCommandEvent& event){
		if (learnView && learnView->CanGoBack()){
			learnView->GoBack();
		}
	}
	/** Forwards button hit */
	void OnNavigateForwards(wxCommandEvent& event){
		if (learnView && learnView->CanGoForward()){
			learnView->GoForward();
		}
	}
	/** Home button hit */
	void OnNavigateHome(wxCommandEvent& event){
		if (learnView){learnView->LoadURL(homeurl);}
	}
	/** When the web view has finished loading the requested page */
	void OnNavigationComplete(wxWebViewEvent& event){
		lastURL = event.GetURL();
		titleLabel->SetLabel(lastURL);
		openInBrowserCtrl->SetURL(lastURL);
		backBtn->Enable(learnView->CanGoBack());
		forwardBtn->Enable(learnView->CanGoForward());
	}
	/** On new window requests, open them in the default browser*/
	void OnNavigationNewWindow(wxWebViewEvent& event){
		wxLaunchDefaultBrowser(event.GetURL());
	}
	/** When the timer expires, deallocate the web view*/
	void OnTimerExpire(wxTimerEvent& event){
		delete learnView;
		learnView = NULL;
	}

	//window events
	void OnAbout(wxCommandEvent& event);
	void OnAddProject(wxCommandEvent& event);
	void OnCreateProject(wxCommandEvent& event);
	void OnRemoveInstallPath(wxCommandEvent& event);
	void OnRevealProject(wxCommandEvent& event);
	void OnOpenWith(wxCommandEvent& event);
	void OnPageChanging(wxBookCtrlEvent& event);
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
	void OnRevealEditor(wxCommandEvent& event){
		int id = installsList->GetSelection();
		if (id != wxNOT_FOUND){
			editor& e = editors[id];
			string path = e.path + dirsep + e.name;
			reveal_in_explorer(path);
		}
	}
	/**
	Called when an item in the install search paths pane is activated. Reveals that location in the system's file browser.
	@param event the list event created by the wxListCtrl
	*/
	void OnRevealInstallLocation(wxCommandEvent& event){
		int id = installsPathsList->GetSelection();
		if (id != wxNOT_FOUND){
			reveal_in_explorer(installPaths[id]);
		}
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
	//enable or disable the continue button if there is a selection
	void OnSelectionChange(wxCommandEvent& event){
		openBtn->Enable(editorsListBox->GetSelection() != wxNOT_FOUND );
	}
	
	void OnOpen(wxCommandEvent& event);
	
	wxDECLARE_EVENT_TABLE();
};
