# Unit tests

These tests cover the dependency-light business-logic layer of the app. They
run on any platform with **Qt6** and **nlohmann/json**, independent of the
Visual Studio / Qt MSBuild project used to ship the desktop application.

The network stack (`cpr`) is **not** required: the application now depends on
the `IApiClient` interface, and the tests substitute an in-memory
`FakeApiClient`.

## What is covered

| Suite | Under test |
|-------|------------|
| `tst_errorhandler` | `ErrorHandler` status-code → message mapping and classifiers |
| `tst_jsonutils`    | `JsonUtils` safe field access, asset normalization, user-name resolution |
| `tst_services`     | `AssetService`, `AuthService`, `ProductService`, `ReportService` via `FakeApiClient` |
| `tst_config`       | `Config` API-URL resolution (env var > QSettings > default) |
| `tst_exportutility`| `ExportUtility` CSV escaping, Excel fallback, file-size formatting |

## Running

```bash
# One-time toolchain setup (Debian/Ubuntu):
sudo apt-get install -y qt6-base-dev nlohmann-json3-dev cmake ninja-build g++

# Configure, build, and run:
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

In CI this is run automatically by `.github/workflows/tests.yml`. In Claude Code
web sessions the toolchain is provisioned by `.claude/hooks/setup-tests.sh`
(wired through the `SessionStart` hook in `.claude/settings.json`).

## Adding a test

1. Create `tests/tst_<name>.cpp` with a `QObject` test class and a
   `QTEST_*_MAIN` entry point.
2. Register it in `tests/CMakeLists.txt` via `add_qt_test(tst_<name> <needs_gui>)`.
   Pass `TRUE` for `needs_gui` if the test constructs widgets (runs under the
   offscreen QPA platform).
