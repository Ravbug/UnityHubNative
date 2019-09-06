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
	static string GetPathFromDialog(string& message);
	wxDECLARE_EVENT_TABLE();
private:
	void AddProject(project& p);
	project LoadProject(string& path);
	void SaveProjects();
	
	//will store the list of projects
	vector<project> projects;
};
