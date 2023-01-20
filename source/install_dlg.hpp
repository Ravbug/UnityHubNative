#pragma once
#include "interface.h"
#include <thread>
#include <atomic>
#include <semaphore>

struct installVersionData;

struct ConfigureInstallDlg : public ConfigureEditorDlgBase{
public:
    ConfigureInstallDlg(wxWindow* parent, const installVersionData& version);
    std::unordered_map<std::string_view, std::unordered_map<std::string_view, std::string_view>> inidata;
    std::string initext;
    std::string hashcode;
    void OnCheckedChanged(wxTreeListEvent&);
    void OnInstallClicked(wxCommandEvent&);
    
    void RecalculateSize();
    
    wxDECLARE_EVENT_TABLE();
private:
    
    struct treeItemData : public wxClientData{
        const decltype(inidata)::value_type::second_type* data;
        treeItemData(decltype(data) data) : data(data){}
    };
};

struct ComponentInstaller{
    std::string name;
    std::string installerURL;
    std::string outputFileName;
    ComponentInstaller(const decltype(name)& name, const decltype(installerURL)& installerURL, const decltype(outputFileName)& outputFileName): name(name), installerURL(installerURL), outputFileName(outputFileName){}
};

struct InstallProgressDlg : public InstallProgressDlgBase{
    std::vector<std::thread> installThreads;
    std::atomic<uint32_t> ncomplete = 0;
    std::string baseURL;
    std::binary_semaphore installedEditorSemaphore{0};  // start in non-signalled state
    bool Complete() const{
        return ncomplete == installThreads.size();
    }
    InstallProgressDlg(wxWindow* parent, const ComponentInstaller& editorInstaller, const std::vector<ComponentInstaller>& componentInstallers, const decltype(baseURL)& baseURL);
    
    void OnCancelClicked(wxCommandEvent&);
    void CancelAll();
    
    void InstallComponent(const ComponentInstaller& installer, uint32_t row);
    void OnNumericProgressUpdate(wxCommandEvent&);
    void OnStatusUpdate(wxCommandEvent&);
    void OnAllCompleted(wxCommandEvent&);
    
    void PostProgressUpdate(int prog, uint32_t row);
    void PostStatusUpdate(const std::string& newStatus, uint32_t row);
    
    wxDECLARE_EVENT_TABLE();
    virtual ~InstallProgressDlg();
};
