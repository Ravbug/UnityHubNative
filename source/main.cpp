//
//  main.cpp
//  wxWidgetsTemplate
//
//  Copyright © 2019 Ravbug. All rights reserved.
//
// This file contains the definition for a wxWidgets application. It configures it and launches it.
// There is no int main function.

// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "interface_derived.hpp"

class UnityHubNative: public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(UnityHubNative);
bool UnityHubNative::OnInit()
{
    MainFrame *frame = new MainFrameDerived();
	//set the icon (windows and linux only)
#ifdef _WIN32
	//name is the same as the one used in the resource file definition
	//the resource file definition must have the same ending as the name of the icon file itself
	//in this case, the icon file is wxwin.ico, so the definition is IDI_WXWIN
	frame->SetIcon(wxIcon("IDI_WXWIN"));
#elif __linux
	frame->SetIcon(wxIcon(wxICON(wxlin)));
#endif
    frame->Show( true );
    return true;
}
