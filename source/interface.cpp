///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "interface.h"

///////////////////////////////////////////////////////////////////////////

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 560,320 ), wxDefaultSize );

	wxBoxSizer* main_sizer;
	main_sizer = new wxBoxSizer( wxVERTICAL );

	notebook = new wxNotebook( this, NOTEBOOK, wxDefaultPosition, wxDefaultSize, 0 );
	wxPanel* projects_pane;
	projects_pane = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxGridBagSizer* projectManSizer;
	projectManSizer = new wxGridBagSizer( 0, 0 );
	projectManSizer->SetFlexibleDirection( wxBOTH );
	projectManSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	projectsList = new wxListCtrl( projects_pane, wxID_HARDDISK, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	projectManSizer->Add( projectsList, wxGBPosition( 1, 0 ), wxGBSpan( 1, 3 ), wxALL|wxEXPAND|wxFIXED_MINSIZE, 5 );

	wxBoxSizer* pManSizer;
	pManSizer = new wxBoxSizer( wxHORIZONTAL );

	wxButton* revealProjBtn;
	revealProjBtn = new wxButton( projects_pane, wxID_JUMP_TO, wxT("Reveal"), wxDefaultPosition, wxDefaultSize, 0 );
	pManSizer->Add( revealProjBtn, 0, wxALL, 5 );

	wxButton* removeProjBtn;
	removeProjBtn = new wxButton( projects_pane, wxID_DELETE, wxT(" Remove from list"), wxDefaultPosition, wxDefaultSize, 0 );
	pManSizer->Add( removeProjBtn, 0, wxALL, 5 );

	wxButton* add_new_proj;
	add_new_proj = new wxButton( projects_pane, wxID_NEW, wxT("Create New"), wxDefaultPosition, wxDefaultSize, 0 );
	pManSizer->Add( add_new_proj, 0, wxALL, 5 );

	wxButton* add_existing_proj;
	add_existing_proj = new wxButton( projects_pane, wxID_ADD, wxT("Add Existing"), wxDefaultPosition, wxDefaultSize, 0 );
	pManSizer->Add( add_existing_proj, 0, wxALL, 5 );

	wxButton* openWithBtn;
	openWithBtn = new wxButton( projects_pane, OPEN_WITH, wxT("Open With"), wxDefaultPosition, wxDefaultSize, 0 );
	pManSizer->Add( openWithBtn, 0, wxALL, 5 );


	projectManSizer->Add( pManSizer, wxGBPosition( 0, 0 ), wxGBSpan( 1, 2 ), wxEXPAND, 5 );


	projectManSizer->AddGrowableCol( 1 );
	projectManSizer->AddGrowableRow( 1 );

	projects_pane->SetSizer( projectManSizer );
	projects_pane->Layout();
	projectManSizer->Fit( projects_pane );
	notebook->AddPage( projects_pane, wxT("Projects"), true );
	wxPanel* installs_pane;
	installs_pane = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxGridBagSizer* MainSizer;
	MainSizer = new wxGridBagSizer( 0, 0 );
	MainSizer->SetFlexibleDirection( wxBOTH );
	MainSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText* m_staticText5;
	m_staticText5 = new wxStaticText( installs_pane, wxID_ANY, wxT("Detected Installations"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	m_staticText5->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	MainSizer->Add( m_staticText5, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	wxStaticText* m_staticText6;
	m_staticText6 = new wxStaticText( installs_pane, wxID_ANY, wxT("Install Search Paths"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	m_staticText6->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	MainSizer->Add( m_staticText6, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	installsList = new wxListBox( installs_pane, wxID_FLOPPY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxLB_NEEDED_SB|wxLB_SINGLE );
	MainSizer->Add( installsList, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );

	installsPathsList = new wxListBox( installs_pane, wxID_HOME, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxLB_NEEDED_SB|wxLB_SINGLE );
	MainSizer->Add( installsPathsList, wxGBPosition( 3, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	wxButton* m_button5;
	m_button5 = new wxButton( installs_pane, wxID_FIND, wxT("Add Location"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button5, 0, wxALL|wxEXPAND, 5 );

	wxButton* m_button6;
	m_button6 = new wxButton( installs_pane, wxID_CLEAR, wxT("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button6, 0, wxALL|wxEXPAND, 5 );


	MainSizer->Add( bSizer4, wxGBPosition( 3, 1 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	launchHubBtn = new wxButton( installs_pane, wxID_BACKWARD, wxT("Add New"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( launchHubBtn, 0, wxALL|wxEXPAND, 5 );

	removeInstallBtn = new wxButton( installs_pane, wxID_NO, wxT("Uninstall"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( removeInstallBtn, 0, wxALL|wxEXPAND, 5 );

	wxButton* reloadInstalls;
	reloadInstalls = new wxButton( installs_pane, wxID_RELOAD, wxT("Reload"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( reloadInstalls, 0, wxALL|wxEXPAND, 5 );


	MainSizer->Add( bSizer5, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );


	MainSizer->AddGrowableCol( 0 );
	MainSizer->AddGrowableRow( 1 );
	MainSizer->AddGrowableRow( 3 );

	installs_pane->SetSizer( MainSizer );
	installs_pane->Layout();
	MainSizer->Fit( installs_pane );
	notebook->AddPage( installs_pane, wxT("Editor Versions"), false );
	learn_pane = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	learnSizer = new wxGridBagSizer( 0, 0 );
	learnSizer->SetFlexibleDirection( wxBOTH );
	learnSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	backBtn = new wxButton( learn_pane, Nav_Back, wxT("<"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	learnSizer->Add( backBtn, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	titleLabel = new wxStaticText( learn_pane, wxID_ANY, wxT("Loading Page"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT|wxST_ELLIPSIZE_END|wxST_NO_AUTORESIZE );
	titleLabel->Wrap( -1 );
	learnSizer->Add( titleLabel, wxGBPosition( 0, 3 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 5 );

	forwardBtn = new wxButton( learn_pane, Nav_Forward, wxT(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	learnSizer->Add( forwardBtn, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	homeBtn = new wxButton( learn_pane, Nav_Home, wxT("Home"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	learnSizer->Add( homeBtn, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	webSizer = new wxBoxSizer( wxVERTICAL );


	learnSizer->Add( webSizer, wxGBPosition( 1, 0 ), wxGBSpan( 1, 4 ), wxEXPAND, 5 );

	wxBoxSizer* warningSizer;
	warningSizer = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* m_staticText9;
	m_staticText9 = new wxStaticText( learn_pane, wxID_ANY, wxT("Do not use to fill forms or sign into accounts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	m_staticText9->SetFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	warningSizer->Add( m_staticText9, 0, wxALL, 5 );

	openInBrowserCtrl = new wxHyperlinkCtrl( learn_pane, wxID_ANY, wxT("Open current page in default browser"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	openInBrowserCtrl->SetFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	warningSizer->Add( openInBrowserCtrl, 0, wxALL, 5 );


	learnSizer->Add( warningSizer, wxGBPosition( 2, 0 ), wxGBSpan( 1, 4 ), wxEXPAND, 5 );


	learnSizer->AddGrowableCol( 2 );
	learnSizer->AddGrowableRow( 1 );

	learn_pane->SetSizer( learnSizer );
	learn_pane->Layout();
	learnSizer->Fit( learn_pane );
	notebook->AddPage( learn_pane, wxT("Learn"), false );

	main_sizer->Add( notebook, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( main_sizer );
	this->Layout();
	menubar = new wxMenuBar( 0 );
	wxMenu* menuProject;
	menuProject = new wxMenu();
	wxMenuItem* menuCreate;
	menuCreate = new wxMenuItem( menuProject, wxID_NEW, wxString( wxT("Create New Project") ) + wxT('\t') + wxT("Ctrl-N"), wxEmptyString, wxITEM_NORMAL );
	menuProject->Append( menuCreate );

	wxMenuItem* menuAdd;
	menuAdd = new wxMenuItem( menuProject, wxID_ADD, wxString( wxT("Add Existing Project") ) + wxT('\t') + wxT("Ctrl-Shift-N"), wxEmptyString, wxITEM_NORMAL );
	menuProject->Append( menuAdd );

	wxMenuItem* menuRemove;
	menuRemove = new wxMenuItem( menuProject, wxID_DELETE, wxString( wxT("Remove Project From List") ) + wxT('\t') + wxT("Ctrl--"), wxEmptyString, wxITEM_NORMAL );
	menuProject->Append( menuRemove );

	menuProject->AppendSeparator();

	menuReveal = new wxMenuItem( menuProject, wxID_FIND, wxString( wxT("Reveal") ) + wxT('\t') + wxT("Ctrl-F"), wxEmptyString, wxITEM_NORMAL );
	menuProject->Append( menuReveal );

	wxMenuItem* menuOpenWith;
	menuOpenWith = new wxMenuItem( menuProject, wxID_PROPERTIES, wxString( wxT("Open In Different Version") ) + wxT('\t') + wxT("Ctrl-O"), wxEmptyString, wxITEM_NORMAL );
	menuProject->Append( menuOpenWith );

	wxMenuItem* menuReload;
	menuReload = new wxMenuItem( menuProject, wxID_REFRESH, wxString( wxT("Reload Data") ) + wxT('\t') + wxT("Ctrl-R"), wxEmptyString, wxITEM_NORMAL );
	menuProject->Append( menuReload );

	menubar->Append( menuProject, wxT("Project") );

	wxMenu* menuWindow;
	menuWindow = new wxMenu();
	wxMenuItem* quit_menu;
	quit_menu = new wxMenuItem( menuWindow, wxID_EXIT, wxString( wxT("Close") ) + wxT('\t') + wxT("Ctrl-W"), wxEmptyString, wxITEM_NORMAL );
	menuWindow->Append( quit_menu );

	wxMenuItem* about_menu;
	about_menu = new wxMenuItem( menuWindow, wxID_ABOUT, wxString( wxT("About Unity Hub Native") ) , wxEmptyString, wxITEM_NORMAL );
	menuWindow->Append( about_menu );

	menubar->Append( menuWindow, wxT("Window") );

	wxMenu* helpMenu;
	helpMenu = new wxMenu();
	wxMenuItem* sourceMenu;
	sourceMenu = new wxMenuItem( helpMenu, wxID_UNINDENT, wxString( wxT("GitHub Repository") ) , wxEmptyString, wxITEM_NORMAL );
	helpMenu->Append( sourceMenu );

	wxMenuItem* updateMenu;
	updateMenu = new wxMenuItem( helpMenu, wxID_TOP, wxString( wxT("Check for Updates") ) , wxEmptyString, wxITEM_NORMAL );
	helpMenu->Append( updateMenu );

	menubar->Append( helpMenu, wxT("Help") );

	this->SetMenuBar( menubar );


	this->Centre( wxBOTH );
}

MainFrame::~MainFrame()
{
}

CreateProjectDialog::CreateProjectDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 350,230 ), wxDefaultSize );

	wxGridBagSizer* createSizer;
	createSizer = new wxGridBagSizer( 0, 0 );
	createSizer->SetFlexibleDirection( wxBOTH );
	createSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText* m_staticText9;
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Project Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	createSizer->Add( m_staticText9, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	projNameTxt = new wxTextCtrl( this, wxID_ANY, wxT("New Unity Project"), wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !projNameTxt->HasFlag( wxTE_MULTILINE ) )
	{
	projNameTxt->SetMaxLength( 32 );
	}
	#else
	projNameTxt->SetMaxLength( 32 );
	#endif
	createSizer->Add( projNameTxt, wxGBPosition( 0, 1 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 5 );

	wxStaticText* m_staticText10;
	m_staticText10 = new wxStaticText( this, wxID_ANY, wxT("Location"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	createSizer->Add( m_staticText10, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	projLocTxt = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	createSizer->Add( projLocTxt, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );

	wxButton* chooseProjLocBtn;
	chooseProjLocBtn = new wxButton( this, wxID_OPEN, wxT("Choose..."), wxDefaultPosition, wxDefaultSize, 0 );
	createSizer->Add( chooseProjLocBtn, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );

	wxStaticText* m_staticText11;
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("Select Template for Version:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	createSizer->Add( m_staticText11, wxGBPosition( 2, 0 ), wxGBSpan( 1, 2 ), wxALL, 5 );

	wxArrayString unityVersionChoiceChoices;
	unityVersionChoice = new wxChoice( this, wxID_INDEX, wxDefaultPosition, wxDefaultSize, unityVersionChoiceChoices, wxCB_SORT );
	unityVersionChoice->SetSelection( 0 );
	createSizer->Add( unityVersionChoice, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	templateCtrl = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_LIST|wxLC_SINGLE_SEL );
	createSizer->Add( templateCtrl, wxGBPosition( 3, 0 ), wxGBSpan( 1, 3 ), wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	wxButton* cancelProjBtn;
	cancelProjBtn = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( cancelProjBtn, 0, wxALL, 5 );

	wxButton* createProjBtn;
	createProjBtn = new wxButton( this, wxID_FILE, wxT("Create"), wxDefaultPosition, wxDefaultSize, 0 );

	createProjBtn->SetDefault();
	bSizer6->Add( createProjBtn, 0, wxALL, 5 );


	createSizer->Add( bSizer6, wxGBPosition( 4, 1 ), wxGBSpan( 1, 2 ), wxEXPAND|wxALIGN_RIGHT, 5 );


	createSizer->AddGrowableCol( 1 );
	createSizer->AddGrowableRow( 3 );

	this->SetSizer( createSizer );
	this->Layout();

	this->Centre( wxBOTH );
}

CreateProjectDialog::~CreateProjectDialog()
{
}

OpenWithEditorDlgBase::OpenWithEditorDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 230,215 ), wxDefaultSize );

	wxGridBagSizer* openWithSizer;
	openWithSizer = new wxGridBagSizer( 0, 0 );
	openWithSizer->SetFlexibleDirection( wxBOTH );
	openWithSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	openWithLabel = new wxStaticText( this, wxID_ANY, wxT("Select Editor Version"), wxDefaultPosition, wxDefaultSize, 0 );
	openWithLabel->Wrap( -1 );
	openWithSizer->Add( openWithLabel, wxGBPosition( 0, 0 ), wxGBSpan( 1, 3 ), wxALL, 5 );

	editorsListBox = new wxListBox( this, VERSIONS_LIST, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_NEEDED_SB|wxLB_SINGLE );
	openWithSizer->Add( editorsListBox, wxGBPosition( 1, 0 ), wxGBSpan( 1, 3 ), wxALL|wxEXPAND, 5 );

	wxBoxSizer* dialogSizer;
	dialogSizer = new wxBoxSizer( wxHORIZONTAL );

	wxButton* dlgCancel;
	dlgCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	dialogSizer->Add( dlgCancel, 0, wxALL, 5 );

	openBtn = new wxButton( this, wxID_OPEN, wxT("Open"), wxDefaultPosition, wxDefaultSize, 0 );

	openBtn->SetDefault();
	dialogSizer->Add( openBtn, 0, wxALL, 5 );


	openWithSizer->Add( dialogSizer, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );


	openWithSizer->AddGrowableCol( 0 );
	openWithSizer->AddGrowableRow( 1 );

	this->SetSizer( openWithSizer );
	this->Layout();
	openWithSizer->Fit( this );

	this->Centre( wxBOTH );
}

OpenWithEditorDlgBase::~OpenWithEditorDlgBase()
{
}

AddNewInstallDlgBase::AddNewInstallDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	installSearchSizer = new wxBoxSizer( wxVERTICAL );

	versionSearchCtrl = new wxSearchCtrl( this, wxID_FIND, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NOHIDESEL|wxTE_PROCESS_ENTER );
	#ifndef __WXMAC__
	versionSearchCtrl->ShowSearchButton( true );
	#endif
	versionSearchCtrl->ShowCancelButton( false );
	installSearchSizer->Add( versionSearchCtrl, 0, wxALL|wxEXPAND, 5 );

	versionsListCtrl = new wxDataViewListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	versionCol = versionsListCtrl->AppendTextColumn( wxT("Version"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	dateCol = versionsListCtrl->AppendTextColumn( wxT("Date"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	installSearchSizer->Add( versionsListCtrl, 1, wxALL|wxEXPAND, 5 );

	installBtn = new wxButton( this, wxID_FILE, wxT("Loading..."), wxDefaultPosition, wxDefaultSize, 0 );
	installBtn->Enable( false );

	installSearchSizer->Add( installBtn, 0, wxALL|wxALIGN_RIGHT, 5 );


	this->SetSizer( installSearchSizer );
	this->Layout();

	this->Centre( wxBOTH );
}

AddNewInstallDlgBase::~AddNewInstallDlgBase()
{
}
