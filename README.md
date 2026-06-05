# Unity Hub Native

UnityHubNative is a lightweight C++ / wxWidgets alternative to the unnecessarily heavy official Unity Hub, built using [wxWidgets](https://wxwidgets.org/). It uses native widgets on all platforms instead of heavy web components like the official Electron hub.

UnityHubNative launches many times faster, uses far less memory and CPU, and is a fraction of the total size of the official Unity Hub.

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

## Custom Environment Variables & Arguments

UnityHubNative can pass custom environment variables and command-line arguments to the Unity Editor when launching a project. This is especially useful for HiDPI / Wayland scaling on Linux, or any other Editor configuration that needs env vars or CLI flags.

### How to use it

1. Go to **Window → Preferences**
2. Configure your settings:

   **Custom Environment Variables** — one `KEY=VALUE` per line. These are prepended to the Editor launch command as shell variables:

   ```
   GDK_SCALE=2
   GDK_DPI_SCALE=0.5
   XCURSOR_SIZE=24
   ```

   **Extra Command-line Arguments** — appended directly to the Editor command:

   ```
   -force-gfx-vulkan -disable-gpu-skinning
   ```

3. Click **Save** to apply

### Presets

You can save multiple named presets and switch between them:

| Button | What it does |
|--------|-------------|
| **Save As...** | Save current fields as a new preset (or overwrite if name exists) |
| **Delete** | Remove the selected preset |
| **Set Default** | Mark a preset as the default — its settings are automatically applied every time you open any project |

Presets are persisted across sessions. If no preset is marked as default, launch behaviour is unchanged from the original app.

### Use cases

- **Linux Wayland HiDPI** — pass `GDK_SCALE=2` and `GDK_DPI_SCALE=0.5` to make the Unity Editor usable on 4K displays with fractional scaling
- **Vulkan forcing** — pass `-force-gfx-vulkan` to use the Vulkan renderer
- **Logging** — pass `-logFile /path/to/log.txt` to redirect Editor logs
- **Project-specific configs** — save multiple presets for different workflows

UnityHubNative does not have every feature that the official hub has, but it has most of the commonly-needed features. UnityHubNative serves as a shortcut to get into your projects faster.

## Installation
This application is self-contained.
1. Open the [Releases](https://github.com/Ravbug/UnityHubNative/releases) section, and download the version for your platform.
2. Place the executable anywhere. (Recommended `/Applications` on Mac, `C:\Program Files` on Windows, and `/opt` on Linux)
3. Double click to run

### Arch Linux
A PKGBUILD is included in the repository. Build and install with:

```bash
cd unity-hub-native
makepkg -si
```

Or use the AUR package (`unity-hub-native` — coming soon).

## Application data files
This application stores its own files in an application data directory. If the application is behaving unexpectedly or is not working at all, try removing the folder at these locations:
- macOS: `~/Library/Application Support/UnityHubNative`
- Windows: `%APPDATA%/UnityHubNative`
- Linux: `~/UnityHubNative`

## Compiling it yourself

Use CMake:

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target install
```

## Issues
Please report all problems in the [Issues](https://github.com/Ravbug/UnityHubNative/issues) section of this repository. 
Make sure to include as many details as possible, otherwise I won't be able to fix it.
