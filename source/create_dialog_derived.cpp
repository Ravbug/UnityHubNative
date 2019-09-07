//
//  create_dialog_derived.cpp
//  mac
//
//  Created by Main on 9/6/19.
//  Copyright © 2019 Ravbug. All rights reserved.
//

#include "interface_derived.hpp"

//Declare events here
wxBEGIN_EVENT_TABLE(CreateProjectDialogD, wxDialog)
EVT_BUTTON(wxID_CANCEL,CreateProjectDialogD::OnCancel)
wxEND_EVENT_TABLE()

//Construct the dialog
CreateProjectDialogD::CreateProjectDialogD(wxWindow* parent, vector<editor>& versions) : CreateProjectDialog(parent){
	editors = versions;
	//callback = c;
	//Set combo box values to the different Unity versions passed in
	for (editor& e : editors){
		unityVersionChoice->AppendString(e.name);
	}
	//populate the template chooser
}

