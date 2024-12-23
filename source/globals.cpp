#include "globals.h"
#include <fmt/format.h>
#include <wx/listctrl.h>
#include <wx/msgdlg.h>
#if __APPLE__
#include "AppleUtilities.h"
#endif

void launch_process(const std::string& command, int flags) {
#if defined __APPLE__ || defined __linux__
    //the '&' runs the command nonblocking, and >/dev/null 2>&1 destroys output
    auto fullcmd = fmt::format("{}{} &", command,null_device);
    FILE* stream = popen(fullcmd.c_str(), "r");
    pclose(stream);

#elif _WIN32
    //call wxExecute with the Async flag
    wxExecute(wxString(command),flags);

#endif
}

void reveal_in_explorer(const std::filesystem::path& path) {
#if defined __APPLE__
    RevealFile(path);
#else
#if defined __linux__
	std::string command = "xdg-open \"" + path.string() + "\"";

#elif defined _WIN32
	//do not surround the paths in quotes, it will not work
	std::string command = "\\Windows\\explorer.exe \"" + path.string() + "\"";
#endif
	if (std::filesystem::exists(path)) {
		launch_process(command);
	}
	else {
		wxMessageBox("The project at " + path.string() + " could not be found.", "Cannot Reveal Project", wxOK | wxICON_ERROR);
	}
#endif
}

long wxListCtrl_get_selected(wxListCtrl* listCtrl) {
	long itemIndex = -1;
	while ((itemIndex = listCtrl->GetNextItem(itemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != wxNOT_FOUND) {
		break;
	}
	return itemIndex;
}

void fitWindowMinSize(wxWindow* window) {
	//fit size to children
	window->Fit();

	//constrain minimum size to the minimum fitting size
	wxSize size = window->GetSize();
	
	window->SetSizeHints(size);
}

/**
@returns the calculated display scale factor using GDI+
*/
float get_WIN_dpi_multiple() {
	return 1;
	/*
	FLOAT dpiX;
	HDC screen = GetDC(0);
	dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
	ReleaseDC(0, screen);
	return dpiX / 96;*/
}

void dpi_scale(wxWindow* window){
	//fit size to children
	window->Fit();

	//calculate the scaled min size
	float fac = get_WIN_dpi_multiple();
	float minh = window->GetMinHeight() * fac;
	float minw = window->GetMinWidth() * fac;
	//set the minimum size
	window->SetSizeHints(wxSize(minw, minh));
}
