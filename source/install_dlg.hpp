#pragma once
#include "interface.h"

struct installVersionData;

struct ConfigureInstallDlg : public ConfigureEditorDlgBase{
public:
    ConfigureInstallDlg(wxWindow* parent, const installVersionData& version);
    std::unordered_map<std::string_view, std::unordered_map<std::string_view, std::string_view>> inidata;
    std::string initext;
    void OnCheckedChanged(wxTreeListEvent&);
    wxDECLARE_EVENT_TABLE();
private:
    
    struct treeItemData : public wxClientData{
        const decltype(inidata)::value_type::second_type* data;
        treeItemData(decltype(data) data) : data(data){}
    };
};

struct InstallProgressDlg : public InstallProgressDlgBase{
    InstallProgressDlg(wxWindow* parent);
};
