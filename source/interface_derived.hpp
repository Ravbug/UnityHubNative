//
//  interface_derived.hpp
//  mac
//
//  Created by Main on 9/5/19.
//  Copyright © 2019 Ravbug. All rights reserved.
//

#pragma once

#include "globals.h"
#include "interface.h"
#include <functional>
#include <wx/webview.h>
#include <wx/timer.h>
#include <wx/msgdlg.h>
#include <functional>
#include <deque>

#if defined __linux__
#include "wxlin.xpm"
#endif

class MainFrameDerived : public MainFrame{
public:
	//constructor (takes no args)
	MainFrameDerived();
	
	static std::string GetPathFromDialog(const std::string& message);
	
private:
	void AddProject(const project& p, const std::string& filter, bool select=false);
	project LoadProject(const std::filesystem::path& path);
	void SaveProjects();
	void OpenProject(const long& index);
	void OpenProject(const project& p, const editor& e);
	void SaveEditorVersions();
	void LoadEditorPath(const std::filesystem::path& path);
	void LoadEditorVersions();
	void ReloadData();
	void OnActivateProPlus(wxCommandEvent&);
	void OnActivatePersonal(wxCommandEvent&);
    void Filter(wxKeyEvent&);
    void LoadProjects(const std::string& filter);
    
    void OnSelectProject(wxListEvent&);
    void OnDeselectProject(wxListEvent&);
    
    void OnSelectEditor(wxCommandEvent&);
    void OnSelectEditorPath(wxCommandEvent&);

	void OnQuit(wxCommandEvent&);
    
    wxWindow* const projectActionItems[3]{
        revealProjBtn,
        removeProjBtn,
        openWithBtn
    };
    
    wxMenuItem* const projectActionMenus[3]{
        menuRemove,
        menuReveal,
        menuOpenWith
    };
    
    wxWindow* const editorActionItems[3]{
        removeInstallBtn,
        activateProPlusBtn,
        activatePersonalBtn
    };
    
    wxWindow* const editorPathActionItems[1]{
        removeInstallPathBtn
    };
	
	//will store the list of projects
	std::deque<project> projects;
	std::vector<std::filesystem::path> installPaths;
	std::vector<editor> editors;
	
	//window events
	void OnAbout(wxCommandEvent& event);
	void OnAddProject(wxCommandEvent& event);
	void OnCreateProject(wxCommandEvent& event);
	void OnRemoveInstallPath(wxCommandEvent& event);
	void OnRevealProject(wxCommandEvent& event);
	void OnOpenWith(wxCommandEvent& event);
	void OnPageChanging(wxBookCtrlEvent& event);
    void OnUninstall(wxCommandEvent&);
	void OnUpdate(wxCommandEvent& event){
		wxLaunchDefaultBrowser("https://github.com/ravbug/unityhubnative/releases/latest");
	}
	void OnSource(wxCommandEvent& event){
		wxLaunchDefaultBrowser("https://github.com/ravbug/unityhubnative/");
	}
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
		std::string msg = "Select the folder containing Unity installs";
		std::string path = GetPathFromDialog(msg);
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
			std::filesystem::path path = e.path / e.name;
            if (!std::filesystem::exists(path)){
                reveal_in_explorer(e.path);
            }
            else{
                reveal_in_explorer(path);
            }
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
    void OnOpenHub(wxCommandEvent& event);
	void OnReloadEditors(wxCommandEvent& event){
		this->LoadEditorVersions();
	}
	wxDECLARE_EVENT_TABLE();
};

/**
 CreateProjectDialog derived class
 Defines the functionality for the project creation dialog
 */
typedef std::function<void(const std::string&,const project&)> DialogCallback;
class CreateProjectDialogD : public CreateProjectDialog{
public:
	CreateProjectDialogD(wxWindow* parent, const std::vector<editor>& versions, const DialogCallback& callback);
	void show(){
		this->ShowModal();
	}
private:
	std::string validateForm();
	void loadTemplates(const editor& e);
	DialogCallback callback;
	std::vector<editor> editors;
    
    size_t GetSelectedEditorIndex();
	
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
class OpenWithDlg : public OpenWithEditorDlgBase{
public:
	OpenWithDlg(wxWindow* parent, const project& project, const std::vector<editor>& versions,  const OpenWithCallback& callback);
	void show(){
		this->ShowModal();
	}
private:
	std::vector<editor> editors;
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

class AddNewInstallDlg : public AddNewInstallDlgBase{
public:
    AddNewInstallDlg(wxWindow* parent);
    
    void Show(){
        this->ShowModal();
    }
private:
    struct version{
        std::string name, hashcode, date;
        version(const decltype(name)& name, const decltype(hashcode)& hashcode, const decltype(date)& date) : name(name), hashcode(hashcode), date(date){}
    };
    
    std::vector<version> versions;
    void GetAllVersions();
    void PopulateTable(wxCommandEvent&);
    void InstallSelected(wxCommandEvent&);
    void InstallSelectedWithHub(wxCommandEvent&);
    void Filter(wxCommandEvent&);
    void Reenable(wxCommandEvent&);
	void ExecuteProc(wxCommandEvent&);
    
    void PopulateWithFilter(const std::function<bool(const version&)>);
    wxDECLARE_EVENT_TABLE();
};
