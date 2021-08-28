#include "interface_derived.hpp"
#include <thread>
#include <cpr/cpr.h>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <fmt/format.h>

using namespace std;
#define UPDATEEVT 2004
#define REENABLEEVT 2005
wxDEFINE_EVENT(updateEvt, wxCommandEvent);
wxDEFINE_EVENT(reenableEvt, wxCommandEvent);

//Declare events here
wxBEGIN_EVENT_TABLE(AddNewInstallDlg, wxDialog)
EVT_COMMAND(UPDATEEVT, updateEvt, AddNewInstallDlg::PopulateTable)
EVT_COMMAND(REENABLEEVT, reenableEvt, AddNewInstallDlg::Reenable)
EVT_BUTTON(wxID_FILE,AddNewInstallDlg::InstallSelected)
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
    installBtn->Fit();
    installSearchSizer->Layout();
}


void AddNewInstallDlg::GetAllVersions(){
    // version date info
    unordered_map<string,string> versionDates;
    {
        cpr::Response r = cpr::Get(cpr::Url{"https://symbolserver.unity3d.com/000Admin/history.txt"});
        if (r.status_code != 200){
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
        }
    }
    // installation URLs
    {
        cpr::Response r = cpr::Get(cpr::Url{"https://unity3d.com/get-unity/download/archive"});
        
        // check if succeeded
        if (r.status_code != 200){
            wxMessageBox("Unable to access Unity versions", "Download error", wxOK | wxICON_ERROR);
        }
        else{
            // get all the Unity versions
            // they are prefixed with unityhub:// links
            
            std::string_view match("unityhub://");
            
            for(size_t i = 0; i < r.text.size(); i++){
                std::string_view section(r.text.data() + i,match.size());
                if (strncmp(r.text.data() + i, match.data(), match.size()) == 0){
                    // we have found a version, extract its data
                    auto begin = i + match.size();
                    auto end = r.text.find_first_of("\"", i + match.size());
                    std::string_view versiondata(r.text.data() + begin,end-begin);
                    
                    // get the version and hashcode
                    auto slashpos = versiondata.find_first_of("/");
                    
                    auto version = string(string_view(versiondata.data(),slashpos));
                    if (versionDates.find(version) != versionDates.end()){
                        versions.emplace_back(version,string(string_view(versiondata.data() + slashpos + 1, versiondata.size() - slashpos - 1)), versionDates.at(version));
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
    // get the selected item
    auto item = versionsListCtrl->GetSelection();
    auto data = *(reinterpret_cast<version*>(versionsListCtrl->GetItemData(item)));
    installBtn->Disable();
    installBtn->SetLabel("Downloading...");
    if (item.IsOk()){
        
        thread th([=]{
            // download the file
            string url = "https://download.unity3d.com/download_unity/" + data.hashcode +
            #ifdef __APPLE__
            "/UnityDownloadAssistant.dmg";
            #elif defined _WIN32
                
            #elif __linux__
                
            #else
            #error This platform is not supported.
                    
            #endif
            
            auto r = cpr::Get(cpr::Url{url});
            
            if (r.status_code != 200){
                // TODO: post that download failed
                throw runtime_error("Unable to download installer");
            }
            else{
                // write the file to temp location
                auto outpath = fmt::format("{}{}{}{}{}", cachedir,dirsep,"UnityDownloadAssistant",data.hashcode,".dmg");
                ofstream outfile(outpath, std::ios::binary);
                outfile.write(r.text.c_str(),r.text.size());
                
                // open the file
                launch_process(fmt::format("open \"{}\"", outpath));
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
}

void AddNewInstallDlg::Reenable(wxCommandEvent &){
    installBtn->Enable();
    installBtn->SetLabel("Install Selected");
}
