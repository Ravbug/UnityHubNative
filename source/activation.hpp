#pragma once
#include "interface.h"
#include "globals.h"

class PlusProActivationDlg : public PlusProActivationDlgBase {
	editor the_editor;
public:
	PlusProActivationDlg(wxWindow* parent, const editor& editor) : PlusProActivationDlgBase(parent), the_editor(editor) {}
	void OnActivateHit(wxCommandEvent&);
	wxDECLARE_EVENT_TABLE();
};

class PersonalActivationDlg : public PersonalActivationDlgBase {
	editor the_editor;
public:
	PersonalActivationDlg(wxWindow* parent, const editor& editor) : PersonalActivationDlgBase(parent), the_editor(editor) {}
	void OnCreateHit(wxCommandEvent&);
	void OnActivateHit(wxCommandEvent&);
	wxDECLARE_EVENT_TABLE();
};