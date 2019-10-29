//
//  open_with_dlg.cpp
//
//  Copyright © 2019 Ravbug. All rights reserved.
//

#include "interface_derived.hpp"

wxBEGIN_EVENT_TABLE(OpenWithDlg, wxDialog)
EVT_BUTTON(wxID_CANCEL,OpenWithDlg::OnCancel)
EVT_BUTTON(wxID_OPEN,OpenWithDlg::OnOpen)
EVT_LISTBOX(VERSIONS_LIST, OpenWithDlg::OnSelectionChange)
wxEND_EVENT_TABLE()

OpenWithDlg::OpenWithDlg(wxWindow* parent, const project& project, const vector<editor>& versions,  const OpenWithCallback& callback): OpenWithEditorDlgBase(parent){

	
	//populate list ctrl in reverse so ids match
	wxArrayString a;
	for (const editor& e : versions){
		a.Add(e.name + " - " + e.path);
	}
	editorsListBox->InsertItems( a, 0);

	//update members
	editors = versions;
	p = project;
	this->callback = callback;
	
	//set title
	openWithLabel->SetLabel("Select editor verison to open \"" + project.name + "\"");
	
	//fix window size
	fitWindowMinSize(this);
}

/**
 Called when the Open button in the dialog is pressed
 */
void OpenWithDlg::OnOpen(wxCommandEvent& event){
	long selectedIndex = editorsListBox->GetSelection();
	if (selectedIndex != wxNOT_FOUND){
		editor e = editors[selectedIndex];
		callback(p,e);
		//close and dispose self
		this->EndModal(0);
		this->Destroy();
	}
}
