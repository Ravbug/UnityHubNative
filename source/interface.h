///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/srchctrl.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/hyperlink.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/dataview.h>
#include <wx/treelist.h>

///////////////////////////////////////////////////////////////////////////

#define NOTEBOOK 1000
#define wxID_HARDDISK 1001
#define wxID_JUMP_TO 1002
#define OPEN_WITH 1003
#define FILTER_PROJ_ID 1004
#define wxID_FLOPPY 1005
#define ACTIV_PROPLUS 1006
#define ACTIV_PERSONAL 1007
#define wxID_RELOAD 1008
#define wxID_TOP 1009
#define PAD_CREATE 1010
#define PAD_ACTIVATE 1011
#define VERSIONS_LIST 1012
#define INSTALLVIAHUB 1013
#define PPA_ACTIVATE 1014

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame
{
	private:

	protected:
		wxNotebook* notebook;
		wxListCtrl* projectsList;
		wxButton* revealProjBtn;
		wxButton* removeProjBtn;
		wxButton* openWithBtn;
		wxSearchCtrl* projSearchCtrl;
		wxListBox* installsList;
		wxListBox* installsPathsList;
		wxButton* removeInstallPathBtn;
		wxButton* launchHubBtn;
		wxButton* removeInstallBtn;
		wxButton* activateProPlusBtn;
		wxButton* activatePersonalBtn;
		wxMenuBar* menubar;
		wxMenuItem* menuRemove;
		wxMenuItem* menuReveal;
		wxMenuItem* menuOpenWith;

	public:

		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Unity Hub Native"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,400 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MainFrame();

};

///////////////////////////////////////////////////////////////////////////////
/// Class PersonalActivationDlgBase
///////////////////////////////////////////////////////////////////////////////
class PersonalActivationDlgBase : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText14;
		wxStaticText* m_staticText15;
		wxButton* m_button24;
		wxStaticText* m_staticText16;
		wxStaticText* m_staticText17;
		wxHyperlinkCtrl* m_hyperlink2;
		wxStaticText* m_staticText19;
		wxStaticText* m_staticText20;
		wxButton* activateBtn;

	public:

		PersonalActivationDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Personal License Manual Activation"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );

		~PersonalActivationDlgBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class CreateProjectDialog
///////////////////////////////////////////////////////////////////////////////
class CreateProjectDialog : public wxDialog
{
	private:

	protected:
		wxTextCtrl* projNameTxt;
		wxTextCtrl* projLocTxt;
		wxChoice* unityVersionChoice;
		wxListCtrl* templateCtrl;

	public:

		CreateProjectDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Create a New Unity Project"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 350,230 ), long style = wxCAPTION|wxRESIZE_BORDER|wxSTAY_ON_TOP );

		~CreateProjectDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class OpenWithEditorDlgBase
///////////////////////////////////////////////////////////////////////////////
class OpenWithEditorDlgBase : public wxDialog
{
	private:

	protected:
		wxStaticText* openWithLabel;
		wxListBox* editorsListBox;
		wxButton* openBtn;

	public:

		OpenWithEditorDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Open With Specific Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxCLOSE_BOX|wxRESIZE_BORDER|wxSTAY_ON_TOP );

		~OpenWithEditorDlgBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class AddNewInstallDlgBase
///////////////////////////////////////////////////////////////////////////////
class AddNewInstallDlgBase : public wxDialog
{
	private:

	protected:
		wxBoxSizer* installSearchSizer;
		wxSearchCtrl* versionSearchCtrl;
		wxDataViewListCtrl* versionsListCtrl;
		wxDataViewColumn* versionCol;
		wxDataViewColumn* dateCol;
		wxButton* installBtn;
		wxButton* installViaHubBtn;

	public:

		AddNewInstallDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Install New Editor Version"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 300,300 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP );

		~AddNewInstallDlgBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class PlusProActivationDlgBase
///////////////////////////////////////////////////////////////////////////////
class PlusProActivationDlgBase : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText9;
		wxTextCtrl* plusProActivUsernameCtrl;
		wxStaticText* m_staticText10;
		wxTextCtrl* plusProActivPasswordCtrl;
		wxStaticText* m_staticText11;
		wxTextCtrl* plusProActivationSerialCtrl;
		wxStaticText* m_staticText13;
		wxButton* activateBtn;

	public:

		PlusProActivationDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Activate Plus/Pro License"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

		~PlusProActivationDlgBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class OutputDialogBase
///////////////////////////////////////////////////////////////////////////////
class OutputDialogBase : public wxDialog
{
	private:

	protected:
		wxTextCtrl* contentText;

	public:

		OutputDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Output"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 388,256 ), long style = wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER|wxSTAY_ON_TOP );

		~OutputDialogBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ConfigureEditorDlgBase
///////////////////////////////////////////////////////////////////////////////
class ConfigureEditorDlgBase : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText17;
		wxButton* installBtn;
		wxStaticText* totalInstallLabel;
		wxTreeListCtrl* moduleSelectTree;

	public:

		ConfigureEditorDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX );

		~ConfigureEditorDlgBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class InstallProgressDlgBase
///////////////////////////////////////////////////////////////////////////////
class InstallProgressDlgBase : public wxDialog
{
	private:

	protected:
		wxDataViewListCtrl* installProgressList;
		wxDataViewColumn* m_dataViewListColumn3;
		wxDataViewColumn* m_dataViewListColumn4;
		wxDataViewColumn* m_dataViewListColumn5;
		wxButton* cancelDoneBtn;

	public:

		InstallProgressDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );

		~InstallProgressDlgBase();

};

