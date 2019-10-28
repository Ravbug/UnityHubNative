//
//  open_with_dlg.cpp
//
//  Copyright © 2019 Ravbug. All rights reserved.
//

#include "interface_derived.hpp"

wxBEGIN_EVENT_TABLE(OpenWithDlg, wxDialog)
EVT_BUTTON(wxID_CANCEL,OpenWithDlg::OnCancel)
EVT_BUTTON(wxID_OPEN,OpenWithDlg::OnOpen)
EVT_LIST_ITEM_SELECTED(VERSIONS_LIST,OpenWithDlg::OnSelect)
EVT_LIST_ITEM_DESELECTED(VERSIONS_LIST, OpenWithDlg::OnDeselect)
wxEND_EVENT_TABLE()

OpenWithDlg::OpenWithDlg(wxWindow* parent, const project& project, const vector<editor>& versions,  const OpenWithCallback& callback): OpenWithEditorDlgBase(parent){
	
	//fix window size
	fitWindowMinSize(this);

	//populate list ctrl in reverse so ids match
	for (unsigned long x = versions.size()-1; x >= 0; x--){
		editor e = versions[x];
		wxListItem i;
		i.SetId(0);
		string label = e.name + " - " + e.path;
		i.SetText(label);
		
		editorsListCtrl->InsertItem(i);
	}
	//update members
	editors = versions;
	p = project;
	this->callback = callback;
}

/**
 Called when the Open button in the dialog is pressed
 */
void OpenWithDlg::OnOpen(wxCommandEvent& event){
	long selectedIndex = wxListCtrl_get_selected(editorsListCtrl);
	editor e = editors[selectedIndex];
	if (selectedIndex > -1){
		callback(p,e);
		//close and dispose self
		this->EndModal(0);
		this->Destroy();
	}
}
