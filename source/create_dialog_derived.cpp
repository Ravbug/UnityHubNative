//
//  create_dialog_derived.cpp
//
//  Copyright © 2019 Ravbug. All rights reserved.
//

#include "interface_derived.hpp"
#if defined _WIN32 
#include "dirent.h" 
#else
#include <dirent.h>
#endif
#include <wx/msgdlg.h>
#include <fmt/format.h>
#include <wx/choicdlg.h>

using namespace std;

//Declare events here
wxBEGIN_EVENT_TABLE(CreateProjectDialogD, wxDialog)
EVT_BUTTON(wxID_CANCEL,CreateProjectDialogD::OnCancel)
EVT_BUTTON(wxID_OPEN,CreateProjectDialogD::setProjectPath)
EVT_BUTTON(wxID_FILE,CreateProjectDialogD::OnCreate)
EVT_CHOICE(wxID_INDEX,CreateProjectDialogD::OnChoiceChanged)
wxEND_EVENT_TABLE()

//Construct the dialog
CreateProjectDialogD::CreateProjectDialogD(wxWindow* parent, const vector<editor>& versions, const DialogCallback& callback) : CreateProjectDialog(parent){
#if defined _WIN32
	//high DPI scaling fixes
	fitWindowMinSize(this);
#endif

	editors = versions;
	this->callback = callback;
	//Set combo box values to the different Unity versions passed in
	for (const editor& e : editors){
		unityVersionChoice->AppendString(e.name);
	}
	//select the element 0
	unityVersionChoice->SetSelection(0);
	
	//populate the template chooser
	loadTemplates(editors[GetSelectedEditorIndex()]);
	
	this->Fit();
}

/**
 Called when the Choose button in the dialog is pressed
 Summons an open panel, to allow the user to pick a location to
 write the Unity project
 @param event (unused) command event from wxWidgets
 */
void CreateProjectDialogD::setProjectPath(wxCommandEvent& event){
	string prompt = "Select where to write the Unity project";
	string path = MainFrameDerived::GetPathFromDialog(prompt);
	if (path != ""){
		//set the path
		*projLocTxt << path;
	}
}

/**
 Called when the Create button in the dialog is pressed.
 It checks the form to ensure all fields are filled. If a field is not
 filled correctly, it will show a dialog
 */
void CreateProjectDialogD::OnCreate(wxCommandEvent& event){
	//validate form
	string message = validateForm();
	if (message == ""){
        
        
		//assemble the command that will create the project described by the dialog
        
		editor& e = editors[GetSelectedEditorIndex()];
		auto executablePath = e.path / e.name / executable;
		auto executableTemplatesPath = e.path / e.name / templatesDir;
		string projName = projNameTxt->GetValue().ToStdString();
		string projPath = projLocTxt->GetValue().ToStdString();
        
        auto outPath = filesystem::path(projPath) / projName;
        
        // extra check: ensure that the location to write the project doesn't already have a project
        {
            auto projectversionpath = outPath / "ProjectSettings" / "ProjectVersion.txt";
            if (std::filesystem::exists(projectversionpath)){
                auto answer = wxMessageBox(fmt::format("A Unity project already exists at {}. Overwrite it?", outPath.string()),"Project Already Exists",  wxYES_NO);
                if (answer != wxYES){
                    return;
                }
            }
        }
		
		//get the selected template
		long itemIndex = wxListCtrl_get_selected(templateCtrl);
		string templateName;
		if (itemIndex > -1){
			templateName = templateCtrl->GetItemText(itemIndex);
		}
		
		//create the command string
		#if defined __APPLE__
        string command = fmt::format("\"{}\" -createProject \"{}\" -cloneFromTemplate \"{}{}.{}\"",
                                     executablePath.string(),
                                     outPath.string(),
                                     executableTemplatesPath.string(),
                                     templatePrefix,
                                     templateName
                                     );
		#elif defined _WIN32
			auto fullProj = filesystem::path(projPath) / projName;
			auto fullTemplate = executableTemplatesPath / fmt::format("{}.{}",templatePrefix,templateName);
			string command = fmt::format("\"{}\" -createproject {} -cloneFromTemplate \"{}\"",
				executablePath.string(),
				fullProj.string(),
				fullTemplate.string());
		#elif defined __linux__
			string command = fmt::format("\"{}\"  -createproject \"{}\" -cloneFromTemplate \"{}{}.{}\"",
				executablePath.string(),
				(filesystem::path(projPath) / projName).string(),
				executableTemplatesPath.string(),
				templatePrefix,
				templateName
			);
		#endif
		//TODO: return this command to what summoned this dialog
		project p = {projName,e.name,"",filesystem::path(projPath) / filesystem::path(projName)};
		this->callback(command,p);
		
		//close and dispose self
		this->EndModal(0);
		delete this;
	}
	else{
		//notify user of why the dialog form is invalid
		wxMessageBox(message,"Cannot Create Project",wxOK | wxICON_ERROR);
	}
}

/**
 Determines if the dialog has been filled correctly.
 @return empty string if dialog is correct, or a message explaining what in the dialog
 is wrong.
 */
string CreateProjectDialogD::validateForm(){
	//is the name field correct?
	if (projNameTxt->GetValue().length() == 0){
		return "You must specify a name for the project.";
	}
	
	//is the path field correct?
	if (projLocTxt->GetValue().length() == 0){
		return "You must specify a valid location to write the project.";
	}
	
	//is a template selected?
	if(wxListCtrl_get_selected(templateCtrl) == -1){
		return "You must select a template.";
	}
	
	return "";
}

/**
 Loads the templates for an editor into the listctrl in the dialog
 @param e the editor struct to load templates for
 */
void CreateProjectDialogD::loadTemplates(const editor& e){
	//clear the picker
	templateCtrl->ClearAll();
	
	//open the folder
	auto templatesFolder = e.path /e.name / templatesDir;
	DIR* dir = opendir(templatesFolder.string().c_str());
	struct dirent *entry = readdir(dir);
	//loop over the contents
	while (entry != NULL)
	{
		//does the file start with the correct prefix?
		if (string(entry->d_name).rfind(templatePrefix,0)==0){
			//add it to the UI
			wxListItem i;
			i.SetId(0);
			string label = entry->d_name;
			i.SetText(label.substr(templatePrefix.length()+1));
			
			templateCtrl->InsertItem(i);
		}
		//advance to next item in folder
		entry = readdir(dir);
	}
	//free resources
	closedir(dir);
	free(entry);
}

void CreateProjectDialogD::OnChoiceChanged(wxCommandEvent& event){
	//get the editor struct for the selected id
	editor& e = editors[GetSelectedEditorIndex()];
	//load the templates for this editor
	loadTemplates(e);
}


size_t CreateProjectDialogD::GetSelectedEditorIndex(){
    auto str = unityVersionChoice->GetStringSelection();
    
    auto itr = std::find_if(editors.begin(), editors.end(), [&str](const editor& e){
        return e.name == str;
    });
    auto idx = std::distance(editors.begin(), itr);
    return idx;
}
