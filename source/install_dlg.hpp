#pragma once
#include "interface.h"

struct installVersionData;

struct ConfigureInstallDlg : public ConfigureEditorDlgBase{
public:
    ConfigureInstallDlg(wxWindow* parent, const installVersionData& version);
};

struct InstallProgressDlg : public InstallProgressDlgBase{
    InstallProgressDlg(wxWindow* parent);
};
