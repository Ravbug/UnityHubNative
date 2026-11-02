//
//  unity_cli.cpp
//  Unity Hub Native
//

#include "unity_cli.hpp"
#include <wx/app.h>
#include <wx/arrstr.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/progdlg.h>
#include <wx/timer.h>
#include <wx/utils.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <memory>
#include <tuple>
#include <vector>

namespace {
	// A Unity CLI version such as "1.0.0-beta.13"
	struct CliVersion {
		int major = 0, minor = 0, patch = 0;
		std::string preTag;    // e.g. "beta", empty for a release
		int preNumber = 0;
	};

	// The Editor identity helper that `unity open` starts arrived in 1.0.0-beta.6, but
	// before 1.0.0-beta.10 it could leave the Editor silently signed out, and before
	// 1.0.0-beta.12 it dropped Editor requests on Windows
#if _WIN32
	const CliVersion minimumVersion{1, 0, 0, "beta", 12};
#else
	const CliVersion minimumVersion{1, 0, 0, "beta", 10};
#endif

	// The installer's version manifest for the stable channel (its default)
	constexpr const char* stableManifestUrl = "https://public-cdn.cloud.unity3d.com/hub/prod/cli/latest.json";

	std::optional<CliVersion> ParseCliVersion(const std::string& text) {
		CliVersion version;
		char tag[32]{};
		const int matched = sscanf(text.c_str(), "%d.%d.%d-%31[A-Za-z].%d", &version.major, &version.minor, &version.patch, tag, &version.preNumber);
		if (matched < 3) {
			return std::nullopt;
		}
		if (matched >= 4) {
			version.preTag = tag;
		}
		return version;
	}

	bool IsAtLeast(const CliVersion& version, const CliVersion& minimum) {
		const auto core = [](const CliVersion& v) { return std::tie(v.major, v.minor, v.patch); };
		if (core(version) != core(minimum)) {
			return core(version) > core(minimum);
		}
		// a release is newer than any pre-release of the same version
		if (version.preTag.empty() || minimum.preTag.empty()) {
			return version.preTag.empty();
		}
		return std::tie(version.preTag, version.preNumber) >= std::tie(minimum.preTag, minimum.preNumber);
	}

	wxString ToWx(const std::filesystem::path& path) {
#if _WIN32
		return wxString(path.wstring());
#else
		return wxString::FromUTF8(path.string());
#endif
	}

	std::filesystem::path HomeDirectory() {
#if _WIN32
		const wchar_t* profile = _wgetenv(L"USERPROFILE");
		return profile ? std::filesystem::path(profile) : homedir;
#else
		return std::filesystem::path(getpwuid(getuid())->pw_dir);
#endif
	}

	// Where the official Hub and the CLI's installer record where the CLI is (cli-install.json)
	std::filesystem::path HubDataDirectory() {
#if __APPLE__
		return HomeDirectory() / "Library/Application Support/UnityHub";
#elif _WIN32
		const wchar_t* appData = _wgetenv(L"APPDATA");
		return appData ? std::filesystem::path(appData) / "UnityHub" : HomeDirectory() / "AppData" / "Roaming" / "UnityHub";
#else
		const char* config = getenv("XDG_CONFIG_HOME");
		return (config && *config ? std::filesystem::path(config) : HomeDirectory() / ".config") / "unityhub";
#endif
	}

	// Where Unity's installer puts the CLI by default
	std::filesystem::path DefaultExecutable() {
#if _WIN32
		const wchar_t* localAppData = _wgetenv(L"LOCALAPPDATA");
		const auto base = localAppData ? std::filesystem::path(localAppData) : HomeDirectory() / "AppData" / "Local";
		return base / "Unity" / "bin" / "unity.exe";
#elif __APPLE__
		return HomeDirectory() / ".unity" / "bin" / "unity";
#else
		return HomeDirectory() / ".local" / "bin" / "unity";
#endif
	}

	// Finds where `"key"` and its following colon end, in a small JSON document
	std::optional<size_t> JsonValueStart(const std::string& json, std::string_view key) {
		const std::string quotedKey = "\"" + std::string(key) + "\"";
		auto pos = json.find(quotedKey);
		if (pos == std::string::npos) {
			return std::nullopt;
		}
		pos = json.find_first_not_of(" \t\r\n", pos + quotedKey.size());
		if (pos == std::string::npos || json[pos] != ':') {
			return std::nullopt;
		}
		pos = json.find_first_not_of(" \t\r\n", pos + 1);
		if (pos == std::string::npos) {
			return std::nullopt;
		}
		return pos;
	}

	/**
	 Reads a string value from a small JSON document, enough for cli-install.json
	 @return the unescaped value, or nullopt if the key is missing or not a string
	 */
	std::optional<std::string> JsonStringField(const std::string& json, std::string_view key) {
		const auto start = JsonValueStart(json, key);
		if (!start || json[*start] != '"') {
			return std::nullopt;
		}
		std::string value;
		for (auto pos = *start + 1; pos < json.size(); ++pos) {
			const char c = json[pos];
			if (c == '"') {
				return value;
			}
			if (c != '\\') {
				value += c;
				continue;
			}
			if (++pos == json.size()) {
				break;
			}
			switch (json[pos]) {
				case 'n': value += '\n'; break;
				case 't': value += '\t'; break;
				case 'r': value += '\r'; break;
				case 'u': return std::nullopt;    // not needed for paths, JSON keeps them as UTF-8
				default: value += json[pos]; break;    // \\ \" \/
			}
		}
		return std::nullopt;
	}

	// Reads a boolean value from a small JSON document
	std::optional<bool> JsonBoolField(const std::string& json, std::string_view key) {
		const auto start = JsonValueStart(json, key);
		if (!start) {
			return std::nullopt;
		}
		if (json.compare(*start, 4, "true") == 0) {
			return true;
		}
		if (json.compare(*start, 5, "false") == 0) {
			return false;
		}
		return std::nullopt;
	}

	std::optional<std::string> QueryVersion(const std::filesystem::path& cli) {
		wxArrayString output, errors;
		const auto command = "\"" + ToWx(cli) + "\" --version";
		if (wxExecute(command, output, errors, wxEXEC_NODISABLE | wxEXEC_HIDE_CONSOLE) != 0) {
			return std::nullopt;
		}
		for (auto& line : output) {
			const auto text = line.Trim().Trim(false).utf8_string();
			if (ParseCliVersion(text)) {
				return text;
			}
		}
		return std::nullopt;
	}

	// Shows why a command failed, with the end of its output, where the reason usually is
	void ShowFailure(const wxString& title, const wxString& summary, wxString output) {
		constexpr size_t maxLength = 2000;
		if (output.length() > maxLength) {
			output = "…" + output.Right(maxLength);
		}
		wxMessageBox(output.empty() ? summary : summary + "\n\n" + output, title, wxOK | wxICON_ERROR);
	}

	struct Progress {
		wxWindow* parent;
		wxString title;
		wxString message;
	};

	/**
	 Called when a command finishes
	 @param status the exit code
	 @param cancelled whether the user cancelled it from its progress window
	 @param output what it wrote to stdout and stderr
	 */
	using CommandDone = std::function<void(int status, bool cancelled, const wxString& output)>;

	// Runs a command without blocking the UI, and collects its output
	class CommandProcess : public wxProcess {
	public:
		explicit CommandProcess(CommandDone done) : done(std::move(done)) {
#if _WIN32
			Redirect();
#else
			outputFile = wxFileName::CreateTempFileName("UnityHubNative-cli-");
#endif
		}

		~CommandProcess() override {
			if (!outputFile.empty()) {
				wxRemoveFile(outputFile);
			}
		}

		// Where the command's output goes on macOS and Linux
		wxString OutputTarget() const {
			return outputFile.empty() ? wxString("/dev/null") : outputFile;
		}

		// Shows an indeterminate progress window with a Cancel button while the command runs
		void ShowProgress(const Progress& info) {
			progress = std::make_unique<wxProgressDialog>(info.title, info.message, 100, info.parent, wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_SMOOTH);
			timer.SetOwner(this);
			Bind(wxEVT_TIMER, &CommandProcess::OnPulse, this);
			timer.Start(100);
		}

		void OnTerminate(int, int status) override {
			timer.Stop();
			progress.reset();
			// report outside of wxWidgets' process bookkeeping, which may show dialogs
			wxTheApp->CallAfter([done = std::move(done), status, cancelled = cancelled, output = ReadOutput()] {
				done(status, cancelled, output);
			});
			delete this;
		}

	private:
		void OnPulse(wxTimerEvent&) {
			if (progress && !progress->Pulse() && !cancelled) {
				cancelled = true;
				wxProcess::Kill(static_cast<int>(GetPid()), wxSIGTERM, wxKILL_CHILDREN);
			}
		}

		wxString ReadOutput() {
			wxString text;
#if _WIN32
			// read only what is there: the command's children may have inherited these pipes and keep them open
			std::string bytes;
			for (auto stream : {GetInputStream(), GetErrorStream()}) {
				char buffer[4096];
				while (stream && stream->CanRead()) {
					stream->Read(buffer, sizeof(buffer));
					if (stream->LastRead() == 0) {
						break;
					}
					bytes.append(buffer, stream->LastRead());
				}
			}
			text = wxString::FromUTF8(bytes);
#else
			if (!outputFile.empty()) {
				if (wxFFile file(outputFile); file.IsOpened()) {
					file.ReadAll(&text, wxConvUTF8);
				}
			}
#endif
			return text.Trim().Trim(false);
		}

		CommandDone done;
		wxString outputFile;
		std::unique_ptr<wxProgressDialog> progress;
		wxTimer timer;
		bool cancelled = false;
	};

	/**
	 Runs a command without blocking the UI
	 @param args the program and its arguments
	 @param done called on the UI thread when the command finishes
	 @param progress if set, shows a progress window that can cancel the command
	 */
	void RunCommand(std::vector<wxString> args, CommandDone done, std::optional<Progress> progress = std::nullopt) {
		const auto program = args.front();
		auto process = new CommandProcess(done);
#if !_WIN32
		// Send the output to a file rather than a pipe: the Editor and the CLI's background
		// helpers outlive the CLI, and when a process exits wxWidgets reads its pipes to
		// the end, which would block the UI for as long as they hold them open
		args.insert(args.begin(), {"/bin/sh", "-c", "out=\"$1\"; shift; exec \"$@\" >\"$out\" 2>&1 </dev/null", "sh", process->OutputTarget()});
#endif

#if _WIN32
		std::vector<std::wstring> storage;
		std::vector<const wchar_t*> argv;
		for (const auto& arg : args) {
			storage.push_back(arg.ToStdWstring());
		}
#else
		std::vector<std::string> storage;
		std::vector<const char*> argv;
		for (const auto& arg : args) {
			storage.push_back(arg.utf8_string());
		}
#endif
		for (const auto& arg : storage) {
			argv.push_back(arg.c_str());
		}
		argv.push_back(nullptr);

		int flags = wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE;
		if (progress) {
			// so cancelling stops everything the command started
			flags |= wxEXEC_MAKE_GROUP_LEADER;
			// before starting: the process may be reported finished as soon as it starts
			process->ShowProgress(*progress);
		}
		if (wxExecute(argv.data(), flags, process) == 0) {
			delete process;
			done(-1, false, "Could not start " + program);
		}
	}
}

std::optional<UnityCli> UnityCli::Locate() {
	std::vector<std::filesystem::path> candidates;
	// the Hub and the CLI's installer record where the CLI is
	if (std::ifstream file{HubDataDirectory() / "cli-install.json"}) {
		const std::string json{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
		if (const auto recorded = JsonStringField(json, "path")) {
			candidates.emplace_back(std::u8string(recorded->begin(), recorded->end()));
		}
	}
	candidates.push_back(DefaultExecutable());

	for (const auto& candidate : candidates) {
		std::error_code ec;
		if (!std::filesystem::is_regular_file(candidate, ec)) {
			continue;
		}
		UnityCli cli{candidate};
		if (const auto version = QueryVersion(candidate)) {
			cli.version = *version;
			cli.recentEnough = IsAtLeast(*ParseCliVersion(*version), minimumVersion);
		}
		return cli;
	}
	return std::nullopt;
}

std::optional<UnityCli> UnityCli::Find() {
	auto cli = Locate();
	if (cli && cli->recentEnough) {
		return cli;
	}
	return std::nullopt;
}

bool UnityCli::CanOpen(const project& p, const editor& e) {
	const auto ev = parseVersion(e.name);
	// Editors before 2019.1 sign in on their own and never look for a Hub. This also
	// rules out editors whose version could not be read.
	if (std::tuple(ev.major, ev.minor) < std::tuple(2019, 1)) {
		return false;
	}
	// `unity open` refuses to move an older project onto 6.7 or later (PROJECT_UPGRADE_REQUIRED)
	if (std::tuple(ev.major, ev.minor) >= std::tuple(6000, 7)) {
		const auto pv = parseVersion(p.version);
		if (std::tuple(pv.major, pv.minor, pv.patch, pv.build) < std::tuple(ev.major, ev.minor, ev.patch, ev.build)) {
			return false;
		}
	}
	return true;
}

void UnityCli::Install(wxWindow* parent, std::function<void(bool)> done) {
	// Unity's official installer, as documented for the CLI. It also records where it
	// put the CLI in cli-install.json, which Locate() reads.
	//
	// The installer defaults to the stable channel, but Unity has published only beta
	// versions of the CLI so far, so the stable manifest is missing and that install fails.
	// Use the beta channel until a stable release exists.
#if _WIN32
	std::vector<wxString> args{"powershell.exe", "-NoProfile", "-ExecutionPolicy", "Bypass", "-Command",
		"if (-not $env:UNITY_CLI_CHANNEL) { "
			"try { Invoke-WebRequest -UseBasicParsing -Method Head " + wxString(stableManifestUrl) + " | Out-Null } "
			"catch { $env:UNITY_CLI_CHANNEL = 'beta' } "
		"}; "
		"irm https://unity.com/install.ps1 | iex"};
#else
	// Download with curl or wget, whichever is there (stock Ubuntu desktops have only wget),
	// and download first, so a failed download fails the install instead of running an
	// empty script. UNITY_NON_INTERACTIVE skips the installer's terminal-only setup questions.
	std::vector<wxString> args{"/bin/sh", "-c",
		"fetch() { "
			"if command -v curl >/dev/null 2>&1; then curl -fsSL \"$1\" -o \"$2\"; "
			"elif command -v wget >/dev/null 2>&1; then wget -qO \"$2\" \"$1\"; "
			"else echo 'Installing the Unity CLI needs curl or wget.' >&2; return 127; fi; "
		"}; "
		"if [ -z \"${UNITY_CLI_CHANNEL:-}\" ] && ! fetch " + wxString(stableManifestUrl) + " /dev/null 2>/dev/null; then "
			"export UNITY_CLI_CHANNEL=beta; "
		"fi; "
		"f=$(mktemp) || exit 1; "
		"fetch https://unity.com/install.sh \"$f\" && UNITY_NON_INTERACTIVE=1 bash \"$f\"; "
		"s=$?; rm -f \"$f\"; exit $s"};
#endif
	RunCommand(args, [done](int status, bool cancelled, const wxString& output) {
		if (status != 0 && !cancelled) {
			ShowFailure("Cannot Install Unity CLI", wxString::Format("The Unity CLI installer failed (exit code %d).", status), output);
		}
		done(status == 0);
	}, Progress{parent, "Installing Unity CLI", "Downloading and installing the Unity CLI from unity.com…"});
}

void UnityCli::Open(const project& p, const editor& e, std::string_view buildTarget, std::function<void()> launchDirectly) const {
#if __APPLE__
	const auto editorPath = e.path;    // the Unity.app bundle
#else
	const auto editorPath = e.executablePath();
#endif
	std::vector<wxString> args{ToWx(executable), "open", ToWx(p.path), "--editor-path", ToWx(editorPath)};
	if (!buildTarget.empty()) {
		args.insert(args.end(), {"--build-target", wxString::FromUTF8(buildTarget.data(), buildTarget.size())});
	}
	// never wait on a prompt nobody can see
	args.insert(args.end(), {"--non-interactive", "--no-banner", "--no-color"});

	RunCommand(args, [projectName = wxString::FromUTF8(p.name), launchDirectly](int status, bool, const wxString& output) {
		if (status == 0) {
			return;
		}
		auto message = wxString::Format("Unity CLI could not open \"%s\" (exit code %d).", projectName, status);
		if (!output.empty()) {
			message += "\n\n" + output;
		}
		message += "\n\nOpen it directly with the Editor instead? Unity may then start Unity Hub in the background to sign in.";
		if (wxMessageBox(message, "Cannot Open Project", wxYES_NO | wxICON_WARNING) == wxYES) {
			launchDirectly();
		}
	});
}

void UnityCli::CheckSignedIn(std::function<void(bool)> done) const {
	RunCommand({ToWx(executable), "auth", "status", "--json", "--non-interactive", "--no-banner", "--no-color"},
		[done](int status, bool, const wxString& output) {
			done(status == 0 && JsonBoolField(output.utf8_string(), "loggedIn").value_or(false));
		});
}

void UnityCli::SignIn(wxWindow* parent, std::function<void(bool)> done) const {
	RunCommand({ToWx(executable), "auth", "login", "--no-banner", "--no-color"},
		[done](int status, bool cancelled, const wxString& output) {
			if (status != 0 && !cancelled) {
				ShowFailure("Cannot Sign In", wxString::Format("Unity CLI could not sign in (exit code %d).", status), output);
			}
			done(status == 0);
		},
		Progress{parent, "Sign In to Unity", "Your browser opens to sign in to your Unity account.\nFinish signing in there; this window closes when you're done."});
}
