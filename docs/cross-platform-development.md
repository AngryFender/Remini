# Cross-platform development

Remini is a C++17 Qt Widgets application built with CMake. The UI and Markdown
editor should remain as platform-independent as possible; operating-system
integration belongs in `src/platform/`.

This guide describes the current implementation, the local build workflow, and
rules for adding or changing platform-specific code. “Buildable” does not
necessarily mean that every platform-specific feature is available.

## Current platform status

| Platform | Build configuration | Global shortcut status |
| --- | --- | --- |
| Windows desktop | Supported by the current GitHub Actions workflow using Qt 6.5 and MSVC 2022 | Uses a low-level Windows keyboard hook for Alt+J |
| Linux desktop with X11 | CMake requires X11 and links it to the application | Uses X11 key grabs for Alt+J |
| Linux with Wayland | The application may build if Qt dependencies are present | The current implementation deliberately skips the global shortcut on Wayland |
| macOS | CMake has Qt bundle properties, but a complete macOS build and runtime check are not currently established by CI | No macOS global-shortcut implementation is currently provided |

Treat this table as the status of the code in this repository, not a guarantee
for every OS version or desktop environment. Keep it updated when CI or platform
implementations change.

## Prerequisites

- A C++17-capable compiler and a compatible build tool (Ninja or Make on Unix-like
  systems; Visual Studio/MSVC or another supported generator on Windows).
- CMake.
- Qt Widgets (Qt 6 is used by the current Windows CI; the project also probes for
  Qt 5). Install the Qt development components for the selected compiler and
  architecture.
- On desktop Linux using the current implementation, X11 development headers and
  libraries are required by the top-level CMake configuration.
- Catch2 v2's single header (`catch.hpp`) for the test target. The test CMake
  file accepts `CATCH_INCLUDE_DIR`; set it to the directory containing
  `catch.hpp` if it is not in the compiler's normal include path.

Use a Qt kit that matches the compiler ABI and architecture. For example, a
MSVC Qt kit cannot be used with MinGW, and a 64-bit Qt kit must be paired with a
64-bit compiler.

## Configure, build, and test

Run these commands from the repository root. They use CMake's generator and Qt
package discovery; if Qt is installed in a non-standard location, pass the
appropriate `Qt6_DIR` or `Qt5_DIR` path.

### Linux (X11)

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

If CMake cannot find X11, install the distribution's X11 development package and
reconfigure. The global shortcut depends on a working X11 display; it is not
available under Wayland in the current implementation.

### Windows (Visual Studio)

Open a Developer PowerShell for the Visual Studio version matching the installed
Qt kit, then run:

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DQt6_DIR="C:/path/to/Qt/6.x.x/msvcXXXX_64/lib/cmake/Qt6"
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

Replace the example Qt path with the actual Qt installation. If using a
single-configuration generator such as Ninja, `CMAKE_BUILD_TYPE=Release` is
used instead of the Visual Studio `--config Release` selection.

To run a packaged Windows build outside the development environment, deploy the
required Qt libraries and plugins (for example with `windeployqt`) and verify
the application on a clean machine.

### macOS (experimental)

The project has not yet established macOS support through CI. To explore a build,
install a matching Qt kit and Xcode Command Line Tools, then configure and build:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Resolve any platform-specific compiler or Qt packaging issues before describing
the platform as supported. In particular, verify bundle generation, resource
loading, settings persistence, keyboard behavior, and application shutdown on
macOS. The Windows and Linux global-shortcut implementations do not provide a
macOS equivalent.

## Platform-specific code rules

1. **Keep OS APIs behind the platform layer.** Put new OS integration in
   `src/platform/`; do not spread Win32 or X11 headers through UI or Markdown
   code.
2. **Use Qt abstractions for ordinary application behavior.** Prefer Qt signals,
   slots, `QFile`, `QDir`, `QSettings`, `QStandardPaths`, and Qt event APIs
   when they meet the requirement.
3. **Guard native code explicitly.** Use Qt platform macros such as `Q_OS_WIN`
   and `Q_OS_LINUX` and keep native includes inside the relevant implementation
   file where possible. Do not include Xlib headers in headers consumed by
   Qt's Meta-Object Compiler (MOC): Xlib macros can conflict with Qt identifiers.
4. **Do not assume Linux means X11.** Check the active Qt platform plugin before
   using X11 APIs. When an API is unavailable, fail safely and keep normal editor
   functionality usable.
5. **Keep the build graph conditional.** Add platform libraries only for the
   platform that needs them, and make source inclusion match the implementation's
   actual compile-time guards.
6. **Avoid platform-dependent paths and behavior.** Use Qt path and settings APIs
   instead of hard-coded separators, home directories, or registry/file locations.
7. **Test failure paths.** Check missing displays, unavailable hooks, permission
   restrictions, and cleanup during normal exit. A global shortcut should not
   prevent the app from opening when the host environment does not support it.
8. **Document capability differences.** If a feature cannot be implemented on a
   platform, state the limitation and provide a graceful fallback rather than
   implying parity.

## Adding a new platform or CI job

- Add or adapt a GitHub Actions job that installs the appropriate Qt kit and
  compiler, configures CMake, builds the app, and runs `CatchTests`.
- Keep CI toolchain versions aligned with the Qt version being used. A Qt kit
  built for one compiler family or ABI must not be mixed with another.
- Test both Debug and Release when practical, and include at least one test run
  that exercises the platform-specific code path.
- Prefer compile-time separation for native implementations and a shared Qt-level
  interface for application callers.
- Update this guide's platform-status table only after the build and relevant
  runtime behavior have actually been verified.

## Related documentation

- [Code layout and function ownership](architecture.md)
- [Qt platform integration: `WindowApi`](../src/platform/windowapi.h)
- [CMake build configuration](../CMakeLists.txt)
