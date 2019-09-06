///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_HARDDISK 1000
#define wxID_FLOPPY 1001

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame
{
	private:

	protected:
		wxNotebook* notebook;
		wxPanel* projects_pane;
		wxStaticText* m_staticText2;
		wxListCtrl* projectsList;
		wxButton* add_new_proj;
		wxButton* add_existing_proj;
		wxPanel* installs_pane;
		wxStaticText* m_staticText5;
		wxStaticText* m_staticText6;
		wxListCtrl* installsList;
		wxListCtrl* installsPathsList;
		wxButton* m_button5;
		wxButton* m_button6;
		wxButton* m_button8;
		wxButton* m_button9;
		wxButton* m_button10;
		wxMenuBar* menubar;
		wxMenu* menuFile;

	public:

		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Unity Hub Native"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,320 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MainFrame();

};

