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

using namespace std;



class MainFrameDerived : public MainFrame{
public:
	//constructor (takes no args)
	MainFrameDerived();
	
	//events
	void OnAbout(wxCommandEvent& event);
	void OnAddProject(wxCommandEvent& event);
	void OnCreateProject(wxCommandEvent& event);
	void OnRemoveProject(wxCommandEvent& event);
	void OnOpenProject(wxListEvent& event);
	void OnLocateInstall(wxCommandEvent& event);
	void OnRemoveInstallPath(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
	
	static string GetPathFromDialog(string& message);

private:
	void AddProject(project& p);
	project LoadProject(string& path);
	void SaveProjects();
	void OpenProject(long& index);
	void SaveEditorVersions();
	void LoadEditorPath(const string& path);
	
	
	//will store the list of projects
	vector<project> projects;
	vector<string> installPaths;
};
