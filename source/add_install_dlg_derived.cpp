#include "interface_derived.hpp"
#include <thread>
#include "HTTP.hpp"
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <format>

using namespace std;
#define UPDATEEVT 2004
#define REENABLEEVT 2005
#define EXECUTEEVT 2006
wxDEFINE_EVENT(updateEvt, wxCommandEvent);
wxDEFINE_EVENT(reenableEvt, wxCommandEvent);
wxDEFINE_EVENT(executeEvt, wxCommandEvent);

//Declare events here
wxBEGIN_EVENT_TABLE(AddNewInstallDlg, wxDialog)
EVT_COMMAND(UPDATEEVT, updateEvt, AddNewInstallDlg::PopulateTable)
EVT_COMMAND(REENABLEEVT, reenableEvt, AddNewInstallDlg::Reenable)
EVT_COMMAND(EXECUTEEVT, executeEvt, AddNewInstallDlg::ExecuteProc)
EVT_BUTTON(wxID_FILE,AddNewInstallDlg::InstallSelected)
EVT_BUTTON(INSTALLVIAHUB,AddNewInstallDlg::InstallSelectedWithHub)
EVT_SEARCHCTRL_SEARCH_BTN(wxID_FIND,AddNewInstallDlg::Filter)
wxEND_EVENT_TABLE()

static inline string remove_quotes(const std::string& str){
    return str.substr(1,str.length()-2);
}

AddNewInstallDlg::AddNewInstallDlg(wxWindow* parent) : AddNewInstallDlgBase(parent){
#if defined _WIN32
    //high DPI scaling fixes
    fitWindowMinSize(this);
#endif
    
    // get all the versions available to install
    std::thread th([&]{
        this->GetAllVersions();
    });
    th.detach();
}

void AddNewInstallDlg::PopulateTable(wxCommandEvent&){
    // populate the data view
    PopulateWithFilter([](const version&){return true;});

    installBtn->SetLabel("Install Selected");
    installBtn->Enable();
    installViaHubBtn->Enable();
    installBtn->Fit();
    installSearchSizer->Layout();
}

#define TRYCATCH 1


void AddNewInstallDlg::GetAllVersions(){
#ifndef __linux__
	
    // version date info
    unordered_map<string,string> versionDates;
    {
#if TRYCATCH
		try
#endif
        {
			auto r = fetch("https://symbolserver.unity3d.com/000Admin/history.txt");
			if (r.code != 200){
				wxMessageBox("Unable to access Unity version metadata", "Download error", wxOK | wxICON_ERROR);
			}
			else{
				// parse the CSV
				stringstream stream(r.text);
				string str;
				string last_date;
				while(getline(stream,str,'\n')){
					int i = 0;
					stringstream line(str);
					while(getline(line,str,',')){
						i++;
						if (i % 4 == 0){
							last_date = str;
						}
						else if (i % 7 == 0){
							versionDates.insert(make_pair(remove_quotes(str), last_date));
							break;
						}
					}
				}
				// installation URLs
				{
					auto r = fetch("https://unity.com/releases/editor/archive");
					
					// check if succeeded
					if (r.code != 200){
						wxMessageBox("Unable to access Unity versions", "Download error", wxOK | wxICON_ERROR);
					}
					else{
						// get all the Unity versions
						// they are prefixed with unityhub:// links
						
						constexpr std::string_view match("unityhub://");
						
						for(size_t i = 0; i < r.text.size(); i++){
							std::string_view section(r.text.data() + i,match.size());
							if (strncmp(r.text.data() + i, match.data(), match.size()) == 0){
								// we have found a version, extract its data
								auto begin = i + match.size();
								auto end = std::min(r.text.find_first_of("\"", i + match.size()), r.text.find_first_of("\\", i + match.size()));
								std::string_view versiondata(r.text.data() + begin,end-begin);
								
								// get the version and hashcode
								auto slashpos = versiondata.find_first_of("/");
                                if (slashpos == versiondata.npos){
                                    continue;
                                }
								
								auto version = string(string_view(versiondata.data(),slashpos));
								if (versionDates.find(version) != versionDates.end()){
                                    auto hashcode = string(string_view(versiondata.data() + slashpos + 1, versiondata.size() - slashpos - 1));
									versions.emplace_back(version,hashcode, versionDates.at(version));
								}
							}
						}
						// post to main thread to update table
						wxCommandEvent evt(updateEvt);
						evt.SetId(UPDATEEVT);
						wxPostEvent(this, evt);
					}
				}
			}
		}
#if TRYCATCH
		catch(std::exception& e){
			wxMessageBox(std::format("Network error: {}", e.what()), "Error", wxOK | wxICON_ERROR);
		}
#endif
    }
#endif
}

void AddNewInstallDlg::Filter(wxCommandEvent& evt){
    // get filter string
    auto filter = evt.GetString();
    versionsListCtrl->DeleteAllItems();
    PopulateWithFilter([&](const version& item) -> bool{
        return item.name.find(filter) != string::npos;
    });
   
}

void AddNewInstallDlg::PopulateWithFilter(const std::function<bool (const version &)> func){
    for(const auto& version : versions){
        if (func(version)){
            wxVector<wxVariant> data;
            data.push_back(version.name);
            data.push_back(version.date);
            versionsListCtrl->AppendItem(data,reinterpret_cast<wxUIntPtr>(&version));
        }
    }
}

void AddNewInstallDlg::InstallSelected(wxCommandEvent&){
#ifndef __linux__ 
    // get the selected item
    auto item = versionsListCtrl->GetSelection();
    auto data = *(reinterpret_cast<version*>(versionsListCtrl->GetItemData(item)));
    installBtn->Disable();
    installBtn->SetLabel("Downloading...");
    if (item.IsOk()){
        
        thread th([=]{
            // download the file
            string url = "https://download.unity3d.com/download_unity/" + data.hashcode + "/UnityDownloadAssistant" +
            #ifdef __APPLE__
                ".dmg";
            #elif defined _WIN32
                ".exe";
            #elif __linux__
                
            #else
            #error This platform is not supported.
                    
            #endif
            
			auto r = fetch(url);
            
            if (r.code != 200){
                // TODO: post that download failed
                throw runtime_error("Unable to download installer");
            }
            else{
                // create the temp location if it does not exist
                #if defined __APPLE__ || defined __linux__
                    int status = mkdir(cachedir.string().c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                #elif defined _WIN32
                    int status = mkdir(cachedir.string().c_str());
                #endif
                
                // write the file to temp location
                auto outpath = std::format("{}{}{}.{}", cachedir.string(), "UnityDownloadAssistant", data.hashcode, installerExt);
                ofstream outfile(outpath, std::ios::binary);
                outfile.write(r.text.c_str(),r.text.size());
                
                // open the file
                wxCommandEvent lnevt(executeEvt);
#ifdef __APPLE__
                lnevt.SetString(fmt::format("open \"{}\"", outpath));
#elif defined _WIN32
                lnevt.SetString(std::format("\"{}\"", outpath));
#endif
                lnevt.SetId(EXECUTEEVT);
                wxPostEvent(this,lnevt);

                wxCommandEvent evt(reenableEvt);
                evt.SetId(REENABLEEVT);
                wxPostEvent(this, evt);
            }
        });
        th.detach();


    }
    else{
        wxMessageBox("Select a version", "Error", wxOK | wxICON_ERROR);
    }
#endif
}

void AddNewInstallDlg::InstallSelectedWithHub(wxCommandEvent &){
    auto item = versionsListCtrl->GetSelection();
    auto data = *(reinterpret_cast<version*>(versionsListCtrl->GetItemData(item)));
    
    auto url = std::format("unityhub://{}/{}", data.name,data.hashcode);
    
    wxLaunchDefaultBrowser(url);
}

void AddNewInstallDlg::Reenable(wxCommandEvent &){
    installBtn->Enable();
    installViaHubBtn->Enable();
    installBtn->SetLabel("Install Selected");
}

void AddNewInstallDlg::ExecuteProc(wxCommandEvent& evt)
{
    auto cmd = evt.GetString();
#ifdef _WIN32
    ShellExecute(0, 0, cmd.c_str(), NULL, 0, SW_SHOW);
#else
    launch_process(cmd.ToStdString());
#endif
}
