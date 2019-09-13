# Unity Hub Native

UnityHubNative is a lightweight C++ / wxWidgets alternative to the unnecessarily heavy official Unity Hub, built using my [wxWidgets Template](https://github.com/ravbug/wxwidgetstemplate) repository. This means that, instead of using heavy web components like the official Electron hub, it uses native GUI components on all platforms.


It launches many times faster, uses far less memory and CPU, and is a fraction of the total size of the offical Unity Hub.

![Mac Dark Mode screenshot](https://i.imgur.com/esHBL44.png)
> Supports Mac dark mode!


## Features
- Add an existing project to the list using the Add Existing button
- Open projects by double-clicking them in the list, or by selecting them and pressing Enter
- Create new unity projects with templates, for any Unity modern version you have installed, with the Create New button
- View Unity installs by double clicking them in the list views in the Editor Versions section

UnityHubNative does not have every feature that the official hub has, namely changing editor versions or installing / managing Unity Installs. There is a Manage button in the Editor Versions page which will launch the official hub.

## Installation
1. Open the [Releases](https://github.com/Ravbug/UnityHubNative/releases) section, and download the version for your platform.
2. Place the executable anywhere (Recommended `/Applications` on Mac and `C:\Program Files` on windows)
3. Double click to run

## Application data files
This application stores its own files in an application data directory. If the application is behaving unexpectedly or is not working at all, try removing the folder at these locations:
- macOS: `~/Library/Application Support/UnityHubNative`
- Windows: `%APPDATA%/UnityHubNative`

## Compiling it yourself
At the moment, you cannot compile this Application for Linux. Linux support is planned for the future, but is currently a low priority.
### macOS
1. Open `mac.xcodeproj` in Xcode.
2. Select `Minecraft Sounds Extractor` from the target selector.
3. Press Build (cmd + b). Everything will compile in one step. 

### Windows
1. Open `wxWidgets\build\msw\` and find the SLN that most closely matches your Visual Studio version. For example, open `wx_vc15.sln` if you are using Visual Studio 2015 or later.
2. Select the configuration you want to build (example: `Debug x86`), and press `Build -> Build Solution` (ctrl + shift + B). 
3. Repeat step 2 for each configuration you want to use. I recommend compiling Debug and Release for x64 at the very least.
   - Alternatively, to build all the possible configurations, go to Build → Batch Build, Select All, and press Build, 
4. In the repository root folder, open `windows.sln` in the template root, select your configuration, and build.

## Issues
Please report all problems in the [Issues](https://github.com/Ravbug/wxWidgetsTemplate/issues) section of this repository. 
Make sure to include as many details as possible, or I won't be able to fix it.
