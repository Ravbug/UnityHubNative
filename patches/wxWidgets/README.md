# Local wxWidgets patches

`wxWidgets/` is a copy of the wxWidgets sources with the changes below applied on top.
When you update wxWidgets, re-apply each patch from the repository root, then check that
it is still needed:

```sh
git apply patches/wxWidgets/<name>.patch
```

Each patched spot in the wxWidgets sources is marked with a `UnityHubNative patch` comment.

## wakeuppipe-lost-wakeup.patch

Files: `src/unix/wakeuppipe.cpp`, `include/wx/unix/private/wakeuppipe.h` (macOS and Linux).

`wxWakeUpPipe::WakeUpNoLock()` wrote to the pipe and only then marked it as not empty. If the
main thread drained the pipe in between, the flag stayed "not empty" for good, and every later
wake up was skipped. Signals such as SIGCHLD wake the event loop through this pipe, so wxWidgets
stopped noticing that child processes had exited, and Unity CLI commands seemed to hang forever.

The patch makes the flag atomic, because signal handlers also call `WakeUpNoLock()`, and claims
it before writing.

Without it, asynchronous `wxExecute` calls (all Unity CLI commands, see `source/unity_cli.cpp`)
can stop reporting that they finished.
