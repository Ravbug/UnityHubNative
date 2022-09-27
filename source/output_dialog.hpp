#pragma once
#include "interface.h"

class OutputDialog : public OutputDialogBase{
public:
    OutputDialog(wxWindow* parent, const std::string& data, const std::string& title) : OutputDialogBase(parent) {
        SetTitle(title);
        contentText->SetValue(data);
    }
};
