///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
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
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/hyperlink.h>
#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/srchctrl.h>
#include <wx/dataview.h>

///////////////////////////////////////////////////////////////////////////

#define NOTEBOOK 1000
#define wxID_HARDDISK 1001
#define wxID_JUMP_TO 1002
#define OPEN_WITH 1003
#define wxID_FLOPPY 1004
#define wxID_RELOAD 1005
#define Nav_Back 1006
#define Nav_Forward 1007
#define Nav_Home 1008
#define wxID_TOP 1009
#define VERSIONS_LIST 1010

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame
{
	private:

	protected:
		wxNotebook* notebook;
		wxListCtrl* projectsList;
		wxListBox* installsList;
		wxListBox* installsPathsList;
		wxButton* launchHubBtn;
		wxButton* removeInstallBtn;
		wxPanel* learn_pane;
		wxGridBagSizer* learnSizer;
		wxButton* backBtn;
		wxStaticText* titleLabel;
		wxButton* forwardBtn;
		wxButton* homeBtn;
		wxBoxSizer* webSizer;
		wxHyperlinkCtrl* openInBrowserCtrl;
		wxMenuBar* menubar;
		wxMenuItem* menuReveal;

	public:

		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Unity Hub Native"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 560,320 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MainFrame();

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
		wxSearchCtrl* versionSearchCtrl;
		wxDataViewListCtrl* versionsListCtrl;
		wxDataViewColumn* versionCol;
		wxDataViewColumn* dateCol;
		wxButton* installBtn;

	public:

		AddNewInstallDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Install New Editor Version"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 300,200 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP );
		~AddNewInstallDlgBase();

};

