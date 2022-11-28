//
//  interface_derived.cpp
//  mac
//
//  Created by Main on 9/5/19.
//  Copyright © 2019 Ravbug. All rights reserved.
//

#include "interface_derived.hpp"
#include "activation.hpp"
#include <fstream>
#include <wx/dirdlg.h>
#include <wx/aboutdlg.h>
#if defined _WIN32
#include "dirent.h" 
#else
#include <dirent.h>
#endif

#include <fmt/format.h>
#include <filesystem>
#if __APPLE__
#include "AppleUtilities.h"
#endif
using namespace std;
using namespace std::filesystem;

#define LEARN_TAB 2
#define WEBVIEW 2000
//the web view unloads after 5 minutes of page hidden
const int TIMER_LENGTH = 5 * 1000 * 60;

//Declare events here
wxBEGIN_EVENT_TABLE(MainFrameDerived, wxFrame)
EVT_MENU(wxID_ABOUT, MainFrameDerived::OnAbout)
EVT_MENU(wxID_NEW, MainFrameDerived::OnCreateProject)
EVT_MENU(wxID_ADD, MainFrameDerived::OnAddProject)
EVT_MENU(wxID_DELETE, MainFrameDerived::OnRemoveProject)
EVT_MENU(wxID_FIND, MainFrameDerived::OnRevealProject)
EVT_MENU(wxID_PROPERTIES, MainFrameDerived::OnOpenWith)
EVT_MENU(wxID_REFRESH, MainFrameDerived::OnReloadData)
EVT_MENU(wxID_UNINDENT, MainFrameDerived::OnSource)
EVT_MENU(wxID_TOP, MainFrameDerived::OnUpdate)
EVT_MENU(wxID_EXIT, MainFrameDerived::OnQuit)
EVT_BUTTON(wxID_ADD,MainFrameDerived::OnAddProject)
EVT_BUTTON(wxID_NEW,MainFrameDerived::OnCreateProject)
EVT_BUTTON(wxID_FIND,MainFrameDerived::OnLocateInstall)
EVT_BUTTON(wxID_CLEAR,MainFrameDerived::OnRemoveInstallPath)
EVT_BUTTON(wxID_DELETE,MainFrameDerived::OnRemoveProject)
EVT_BUTTON(wxID_NO,MainFrameDerived::OnUninstall)
EVT_BUTTON(wxID_JUMP_TO,MainFrameDerived::OnRevealProject)
EVT_BUTTON(wxID_BACKWARD,MainFrameDerived::OnOpenHub)
EVT_BUTTON(wxID_RELOAD,MainFrameDerived::OnReloadEditors)
EVT_BUTTON(OPEN_WITH, MainFrameDerived::OnOpenWith)
EVT_BUTTON(ACTIV_PROPLUS, MainFrameDerived::OnActivateProPlus)
EVT_BUTTON(ACTIV_PERSONAL, MainFrameDerived::OnActivatePersonal)

EVT_LIST_ITEM_ACTIVATED(wxID_HARDDISK, MainFrameDerived::OnOpenProject)
EVT_LIST_ITEM_SELECTED(wxID_HARDDISK, MainFrameDerived::OnSelectProject)
EVT_LIST_ITEM_DESELECTED(wxID_HARDDISK, MainFrameDerived::OnDeselectProject)
EVT_LIST_DELETE_ITEM(wxID_HARDDISK,  MainFrameDerived::OnDeselectProject)

EVT_LISTBOX(wxID_FLOPPY, MainFrameDerived::OnSelectEditor)
EVT_LISTBOX(wxID_HOME, MainFrameDerived::OnSelectEditorPath)

EVT_LISTBOX_DCLICK(wxID_FLOPPY,MainFrameDerived::OnRevealEditor)
EVT_LISTBOX_DCLICK(wxID_HOME,MainFrameDerived::OnRevealInstallLocation)
//EVT_SEARCHCTRL_SEARCH_BTN(FILTER_PROJ_ID,MainFrameDerived::Filter)
//EVT_SEARCHCTRL_CANCEL_BTN(FILTER_PROJ_ID, MainFrameDerived::Filter)

wxEND_EVENT_TABLE()

//call superclass constructor
MainFrameDerived::MainFrameDerived() : MainFrame(NULL){
	//set up project list columns
	{
		string cols[] = {"Project Name","Unity Version","Last Modified","Path"};
		for (const auto& str : cols){
			projectsList->AppendColumn(str,wxLIST_FORMAT_LEFT);
		}
	}
	//make the data folder if it does not already exist (with readwrite for all groups)
	#if defined __APPLE__ || defined __linux__
		int status = mkdir(datapath.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		menuReveal->SetItemLabel("Reveal In Finder\tCtrl-F");
	#elif defined _WIN32
		int status = mkdir(datapath.string().c_str());
		//on windows also make the main window background white
		this->SetBackgroundColour(*wxWHITE);
		//high DPI scaling fixes
		dpi_scale(this);
		//set reveal label
		menuReveal->SetItemLabel("Reveal In File Explorer\tCtrl-F");
	#endif
	if (status != 0){
		ReloadData();
	}
	#if defined __linux__
	launchHubBtn->Hide();
	#endif
	
	//if no projects to load, the interface will be blank

	//show current version in titlebar
	this->SetLabel(fmt::format("Unity Hub Native {}",AppVersion));
    projSearchCtrl->Bind(wxEVT_KEY_UP, &MainFrameDerived::Filter, this);
	projSearchCtrl->SetFocus();
}

void MainFrameDerived::OnSelectProject(wxListEvent&){
    for (auto ptr : projectActionItems){
        ptr->Enable();
    }
    for (auto ptr : projectActionMenus){
        ptr->Enable();
    }
}

void MainFrameDerived::OnDeselectProject(wxListEvent&){
    for (auto ptr : projectActionItems){
        ptr->Disable();
    }
    for (auto ptr : projectActionMenus){
        ptr->Enable(false);
    }
}

void MainFrameDerived::OnSelectEditor(wxCommandEvent& evt){
    if ( installsList->GetSelection() != -1) {
        for (auto ptr : editorActionItems){
            ptr->Enable();
        }
    }
    else{
        for (auto ptr : editorActionItems){
            ptr->Disable();
        }
    }
}

void MainFrameDerived::OnSelectEditorPath(wxCommandEvent&){
    if ( installsPathsList->GetSelection() != -1) {
        for (auto ptr : editorPathActionItems){
            ptr->Enable();
        }
    }
    else{
        for (auto ptr : editorPathActionItems){
            ptr->Disable();
        }
    }
}

/**
 Loads the data in the main view. If anything is currently loaded, it will be cleared and re-loaded
 */
void MainFrameDerived::ReloadData(){
	//clear any existing items
    {
        wxListEvent e;
        OnDeselectProject(e);
    }
	projectsList->DeleteAllItems();
	installsPathsList->Clear();
    {
        wxCommandEvent e;
        OnSelectEditorPath(e);
    }
	projects.clear();
	installPaths.clear();
	editors.clear();
	
    LoadProjects("");
	
	//check that the installs path file exists in the folder
	auto p = datapath / editorPathsFile;
	if (filesystem::exists(p)){
		//load the editors
		ifstream in; in.open(p); string line;
		while (getline(in, line)){
			LoadEditorPath(line);
		}
	}
	else{
		//add default data
        for(const auto& path : defaultInstall){
            LoadEditorPath(path);
        }
	}
}

// empty string for filter means no filter
void MainFrameDerived::LoadProjects(const std::string &filter){
    //check that projects file exists in folder
    path p = datapath / projectsFile;
    if (filesystem::exists(p)){
        ifstream in;
        in.open(p);
        string line;
        //if one cannot be loaded
        vector<string> erroredProjects;
        //load each project (each path is on its own line)
        while (getline(in, line)){
            try{
                project pr = LoadProject(line);
                AddProject(pr,filter);
            }
            catch(runtime_error& e){
                //remove project
                erroredProjects.push_back(line);
            }
        }
        //alert user if projects could not be loaded
        if (erroredProjects.size() > 0){
            //build string
            string str;
            for (const auto& s : erroredProjects){
                str += s + "\n";
            }
            //message box
            wxMessageBox("The following projects could not be loaded. They have been removed from the list.\n\n"+str, "Loading error", wxOK | wxICON_WARNING );
            
            //save to remove the broken projects
            SaveProjects();
        }
    }
}

void MainFrameDerived::Filter(wxKeyEvent &){
    projectsList->DeleteAllItems();
    wxListEvent e;
    OnDeselectProject(e);
    projects.clear();
    auto filter = projSearchCtrl->GetValue();
    transform(filter.begin(), filter.end(), filter.begin(), ::tolower);
    LoadProjects(filter);
}

//definitions for the events
void MainFrameDerived::OnAbout(wxCommandEvent& event)
{
	wxAboutDialogInfo aboutInfo;
	aboutInfo.SetName("Unity Hub Native");	
	aboutInfo.SetCopyright("(C) Ravbug 2022");
	aboutInfo.SetDescription("Developed with wxWidgets in C++");
#if defined __linux__
	aboutInfo.SetWebSite("https://github.com/ravbug/UnityHubNative");
	aboutInfo.AddDeveloper("Ravbug (github.com/ravbug)");
	aboutInfo.SetIcon(wxIcon(wxICON(wxlin)));
	aboutInfo.SetVersion(AppVersion);
#elif defined _WIN32
	aboutInfo.SetVersion(AppVersion.data());
	aboutInfo.SetIcon(wxIcon("IDI_WXWIN"));
#endif
	wxAboutBox(aboutInfo);
}

void MainFrameDerived::OnAddProject(wxCommandEvent& event){
	auto msg ="Select the folder(s) containing the Unity Project";
    
    wxDirDialog dlg(NULL, msg, "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_MULTIPLE);
    if (dlg.ShowModal() == wxID_CANCEL) {
        return;
    }
    //get the path and return the standard string version
    wxArrayString paths;
    dlg.GetPaths(paths);
    
    for (const auto& p : paths){
        auto path = p.ToStdString();
        if (path != ""){
            //check that the project does not already exist
            for(project& p : projects){
                if (p.path == path){
                    wxMessageBox( fmt::format("Project \"{}\" has already been added.", p.path.string()), "Cannot add project", wxOK | wxICON_WARNING );
                    return;
                }
            }
            
            //add it to the projects list
            try{
                project p = LoadProject(path);
                AddProject(p,"",true);
            }
            catch(runtime_error& e){
                wxMessageBox(e.what(),"Unable to add project",wxOK | wxICON_ERROR);
            }
        }
    }
}

/**
 Called when the top segmented control switches pages
 @param event the wxBookCtrlEvent sent by the segmented control. Used to get the currently selected page
 @discussion If the Learn tab is switched to, the app will load the web view if it is not loaded, or reset the unload timer. If the learn tab is switched away from, then the app will set a timer to unload the web view and save resources. The app saves in memory the currenlty loaded page so that it can be re-loaded when the view gets re-initialized
 */
void MainFrameDerived::OnPageChanging(wxBookCtrlEvent& event){
	// currently unused ...
}
/**
 Loads an editor search path into the app, updating the UI and the vector
 @param path the string path to laod
 */
void MainFrameDerived::LoadEditorPath(const std::filesystem::path& path){
	//add to internal structure and to file
    if (std::find(installPaths.begin(),installPaths.end(),path) != installPaths.end()){
        return;
    }
	installPaths.push_back(path);
	SaveEditorVersions();
	
	//add to the UI
	wxArrayString a;
	a.Add(path.string());
	
	installsPathsList->Append(a);
}

void MainFrameDerived::OnRemoveInstallPath(wxCommandEvent& event){
	int itemIndex = installsPathsList->GetSelection();
	if (itemIndex != wxNOT_FOUND){
		// Got the selected item index
		//remove it from the vector
		installPaths.erase(installPaths.begin()+itemIndex);
		
		//update the UI
		installsPathsList->Delete(itemIndex);
        wxCommandEvent e;
        OnSelectEditorPath(e);
		
		//commit to file
		SaveEditorVersions();
		return;
	}
	wxMessageBox( "You must select an install path in the list before you can remove it.", "No path selected", wxOK | wxICON_WARNING );
}

/**
 Called to create a new project
 */
void MainFrameDerived::OnCreateProject(wxCommandEvent& event){
	//create a dialog and show it
	if (editors.size() > 0){
		DialogCallback d = [&](string str, project p){
			//add the project
			this->AddProject(p,"",true);
			
			//launch the process
			launch_process(str);
		};
		CreateProjectDialogD* dialog = new CreateProjectDialogD(this,editors,d);
		dialog->show();
	}
	else{
		wxMessageBox("UnityHubNative could not find any Unity Editors installed on this sytem. If you have an editor installed, make sure UnityHubNative can find it by adding its location to the Install Search Paths section of the Editor Versions Tab.","Cannot Create Project",wxOK | wxICON_ERROR);
	}
}

/**
 Called when the reveal project button is clicked
 */
void MainFrameDerived::OnRevealProject(wxCommandEvent& event){
	long selectedIndex = wxListCtrl_get_selected(projectsList);
	if (selectedIndex > -1){
		project& p = projects[selectedIndex];
		reveal_in_explorer(p.path.string());
	}
}

/**
 Called when OpenWith button is pressed
 */
void MainFrameDerived::OnOpenWith(wxCommandEvent& event){
	long selectedIndex = wxListCtrl_get_selected(projectsList);
	if (selectedIndex > -1){
		project& p = projects[selectedIndex];
		OpenWithCallback c = [&](project p, editor e){
			//open the project
			OpenProject(p,e);
		};
				
		OpenWithDlg* dlg = new OpenWithDlg(this,p,editors,c);
		dlg->show();
	}
}

/**
 Launches Unity with a project
 @param index the integer representing which project in the projects Vector to load
 */
void MainFrameDerived::OpenProject(const long& index){
	//get the project
	project p = projects[index];
	
	for (const auto& path : installPaths) {
		auto editorPath = path / p.version / executable;

		//check that the unity editor exists at that location
		if (filesystem::exists(editorPath)) {

			string cmd = "\"" + editorPath.string() + "\" -projectpath \"" + p.path.string() + "\"";

			//start the process
			launch_process(cmd);

			return;
		}
#if __APPLE__
		else if (filesystem::exists(path / executable)) {
			// mac unlabeled version
			auto unlabeledPath = path / executable;
			char buffer[16];
			auto unlabeledPathInfo = path / "Unity.app" / "Contents" / "Info.plist";
			getCFBundleVersionFromPlist(unlabeledPathInfo.string().c_str(), buffer, sizeof(buffer));
			if (p.version == buffer) {
				string cmd = "\"" + unlabeledPath.string() + "\" -projectpath \"" + p.path.string() + "\"";
				launch_process(cmd);
				return;
			}
		}
#endif 
	}
    // prompt the user to choose a new editor because we couldn't locate one
    wxCommandEvent evt;
    MainFrameDerived::OnOpenWith(evt);
}

/**
 Open a specific project with a specific editor
 @param p the project to open
 @param e the editor version to use when opening the project
 */
void MainFrameDerived::OpenProject(const project& p, const editor& e){
	string cmd = "\"" + (e.path / e.name / executable).string() + "\" -projectpath \"" + p.path.string() + "\"";
	launch_process(cmd);
}

/** Brings up a folder selection dialog with a prompt
 * @param message the prompt for the user
 * @return path selected, or an empty string if nothing chosen
 */
string MainFrameDerived::GetPathFromDialog(const string& message)
{
	//present the dialog
	wxDirDialog dlg(NULL, message, "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (dlg.ShowModal() == wxID_CANCEL) {
		return string("");
	}
	//get the path and return the standard string version
	wxString path = dlg.GetPath();
	return path.ToStdString();
}

/**
 Load a project into the struct
 @param path string path to the Unity project folder
 @return project struct with fields set based on Unity project folder
 @throws Exception if a file cannot be located in the Unity project, or if the file contains unexpected data.
 @note Surround this function in a try/catch, because it throws if it cannot succeed.
 */
project MainFrameDerived::LoadProject(const std::filesystem::path &p_as_fs){
	//error if the file does not exist
	if (!filesystem::exists(p_as_fs)){
		throw runtime_error(p_as_fs.string() + " does not exist.");
	}
	
	//the name is the final part of the path
	string name = p_as_fs.filename().string();
	
	//Load ProjectSettings/ProjectVersion.txt to get the editor version, if it exists
	std::filesystem::path projSettings = p_as_fs / "ProjectSettings" / "ProjectVersion.txt";
	if (!filesystem::exists(projSettings)){
		throw runtime_error("No ProjectVersion.txt found at " + p_as_fs.string() + "\n\nEnsure the folder you selected is the root folder of a complete Unity project.");
	}
	
	//the first line of ProjectVersion.txt contains the editor verison as plain text
	string version;
	ifstream inFile;
	inFile.open(projSettings);
	getline(inFile,version);
	version = version.substr(17);
	
	//get the modification date
	struct stat fileInfo;
	if (stat(p_as_fs.string().c_str(), &fileInfo) != 0) {
		throw runtime_error("Cannot get modification date. Ensure this program has access to "+p_as_fs.string());
	}
	
	project p = {name,version,ctime(&fileInfo.st_mtime),p_as_fs,};
	return p;
}

/**
 Writes the project file paths to the file in the Application directory
 The file's name is projects.txt and will be created if it is not present.
 */
void MainFrameDerived::SaveProjects(){
	ofstream file;
	file.open(datapath / projectsFile);
	for (project& p : projects){
		file << p.path.string() << endl;
	}
	file.close();
}

void MainFrameDerived::SaveEditorVersions(){
	ofstream file;
	file.open(datapath / editorPathsFile);
	for (auto& p : installPaths){
		file << p.string() << endl;
	}
	file.close();
	LoadEditorVersions();
}

/**
 Adds a project to the table
 @param p the project struct to add
 @note Ensure all the fields on the struct are initialized
 */
void MainFrameDerived::AddProject(const project& p, const std::string& filter, bool select){
	//add to the vector backing the UI
    if (std::find_if(projects.begin(),projects.end(),[&](const auto& item){
        return p == item;
    }) != projects.end()){
        return;
    }
	projects.insert(projects.begin(),p);
	
	//save to file
    if (filter == ""){
        SaveProjects();
    }
	
	//add (painfully) to the UI
    auto name = p.name;
    transform(name.begin(), name.end(), name.begin(), ::tolower);
    if (name.find(filter) != std::string::npos){
        wxListItem i;
        i.SetId(0);
        i.SetText(p.name);
        
        projectsList->InsertItem(i);
        
        i.SetText(p.version);
        i.SetColumn(1);
        projectsList->SetItem(i);
        
        i.SetText(p.modifiedDate);
        
        i.SetColumn(2);
        projectsList->SetItem(i);
        
        i.SetColumn(3);
        i.SetText(p.path.string());
        projectsList->SetItem(i);
        
        //resize columns
        int cols = projectsList->GetColumnCount();
        for (int i = 0; i < cols; i++){
            projectsList->SetColumnWidth(i, wxLIST_AUTOSIZE);
        }
        
        if(select){
            projectsList->SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        }
    }
	
}

/**
 Loads all installed editor versions from the Install Search Paths, and updates the list control
 */
void MainFrameDerived::LoadEditorVersions(){
	//clear list control
	installsList->Clear();
    wxCommandEvent e;
    OnSelectEditor(e);
	editors.clear();
	
	//iterate over the search paths
	for (auto& path : installPaths){
		//open the folder
		DIR* dir = opendir(path.string().c_str());
		if(dir != nullptr){
			struct dirent *entry = readdir(dir);
			//loop over the contents
			wxArrayString a;
			while (entry != nullptr)
			{
				//is this a folder?
				if (entry->d_type == DT_DIR){
					//does this folder have a valid executable inside?
					auto p = path / entry->d_name / executable;
					if (filesystem::exists(p)){
						//add it to the list
#if __APPLE__
                        // the Unity Download Assistant on Mac does not allow multiple
                        // unity versions at once, which sucks. To get the version,
                        // we need to parse the info.plist inside of Unity.app
                        if (strcmp(entry->d_name, ".") == 0){
                            auto infopath = path / entry->d_name / "Unity.app" / "Contents" / "Info.plist";
                            if (filesystem::exists(infopath)){
                                // read the file and look for CFBundleVersion
                                char buffer[16];
                                getCFBundleVersionFromPlist(infopath.string().c_str(), buffer, sizeof(buffer));
                                
                                //add it to the backing datastructure
                                editor e = {buffer, path};
                                if (std::find(editors.begin(), editors.end(), e) == editors.end()){
                                    a.Add(e.name + " - " + e.path.string());
                                    editors.push_back(e);
                                }
                            }
                        }
                        else
#endif
                        {
                            //add it to the backing datastructure
                            editor e = {entry->d_name, path};
                            if (std::find(editors.begin(), editors.end(), e) == editors.end()){
                                a.Add(e.name + " - " + e.path.string());
                                editors.push_back(e);
                            }
                        }
					}
				}
				entry = readdir(dir);
			}
			installsList->Append(a);
			//free resources when finished
			closedir(dir);
			free(entry);
		}
	}
}

void MainFrameDerived::OnOpenHub(wxCommandEvent &event){
    //
    AddNewInstallDlg dlg(this);
    dlg.Show();
}

void MainFrameDerived::OnUninstall(wxCommandEvent &){
    auto selected = installsList->GetSelection();
	if (selected != -1) {
		const auto& editor = editors[selected];

#ifdef __APPLE__
		// delete the folder
		wxMessageBox("Opening editor location in Finder. To uninstall an editor, simply delete its version folder.", "Notice", wxOK | wxICON_INFORMATION);
		reveal_in_explorer(editor.path);
#elif defined _WIN32
		// execute the uninstaller
		auto uninstaller_path = std::filesystem::path(editor.path) / editor.name / "Editor\\Uninstall.exe";
		ShellExecute(0, 0, uninstaller_path.c_str(), NULL, 0, SW_SHOW);
#endif
    }
   
}
