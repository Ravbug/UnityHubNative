# Unity Hub Native

UnityHubNative is a lightweight C++ / wxWidgets alternative to the unnecessarily heavy official Unity Hub, built using [wxWidgets](https://wxwidgets.org/). This means that, instead of using heavy web components like the official Electron hub, it uses native GUI components on all platforms.

It launches many times faster, uses far less memory and CPU, and is a fraction of the total size of the offical Unity Hub.

<img width="912" alt="image" src="https://user-images.githubusercontent.com/22283943/193929653-a4aad2b6-229b-4438-afee-c30a7ae39949.png">
> Supports Mac dark mode!


## Features
- Add an existing project to the list using the Add Existing button
- Open projects by double-clicking them in the list, or by selecting them and pressing Enter
- Create new unity projects with templates, for any modern Unity version you have installed, with the Create New button
- View Unity installs by double clicking them in the list views in the Editor Versions section
- Manage Unity installs
- Manage Unity licenses
- Update projects to a Unity version of your choosing

UnityHubNative does not have every feature that the official hub has, but it has most of them. UnityHubNative serves as a shortcut to get into your projects faster. 

## Installation
This application is self-contained.
1. Open the [Releases](https://github.com/Ravbug/UnityHubNative/releases) section, and download the version for your platform.
2. Place the executable anywhere. (Recommended `/Applications` on Mac, `C:\Program Files` on Windows, and `/usr/bin` on Linux)
3. Double click to run

## Application data files
This application stores its own files in an application data directory. If the application is behaving unexpectedly or is not working at all, try removing the folder at these locations:
- macOS: `~/Library/Application Support/UnityHubNative`
- Windows: `%APPDATA%/UnityHubNative`
- Linux: `~/UnityHubNative`

## Compiling it yourself

Use CMake:
```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target install
```

## Issues
Please report all problems in the [Issues](https://github.com/Ravbug/UnityHubNative/issues) section of this repository. 
Make sure to include as many details as possible, otherwise I won't be able to fix it.
