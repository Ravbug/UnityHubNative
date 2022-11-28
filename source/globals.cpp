#include "globals.h"
#include <fmt/format.h>

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
