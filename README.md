# Stockvio Inventory Workspace

This repository contains the client-side applications for the Stockvio inventory platform:

- `InventoryQtApp/` — Qt 6/C++ desktop application for warehouse and operations users
- `mobile/` — Flutter field-technician companion app
- `docs/` — source documentation and PDF build scripts
- `installer/` — Inno Setup packaging for the desktop app
- `deploy.ps1` — Windows build/staging script for the desktop app

This repo does not include the backend API server implementation. The desktop and mobile clients communicate with a separate REST API service, and the shipped default configuration points at the production Stockvio API.

---

## What the project does

The desktop application is an inventory and truck-stock management client. It authenticates users against a backend API, exposes product and asset management, stock scanning, report views, and truck-stock workflows, and integrates an AI assistant page.

From the code, the desktop app supports:

- Login, logout, session resume, password reset, and token validation
- Product catalog browsing, searching, low-stock checks, and scan in/out actions
- Asset CRUD and asset views
- User management and profile updates
- Fleet and truck-stock management (trucks, templates, assignments, assigned stock, low-stock, receipts, movements, and cost analytics)
- Reports for inventory and asset summaries
- AI chat (`/ai/chat`) with server-side permission-aware tools
- Auto-update checks against a configured update endpoint
- Theme selection and local settings persistence

The Flutter app in `mobile/` is the field-technician version of the same platform: it handles assigned truck-stock operations, scans, receipts, and low-stock monitoring.

---

## Repository layout

```text
InventoryQtApp/
├── InventoryQtApp/                 Qt desktop app sources
│   ├── CMakeLists.txt             CMake build for Qt app
│   ├── vcpkg.json                 Native dependency manifest
│   ├── main.cpp                   App entry point and logging
│   ├── Config.h                   API/update config resolution
│   ├── Version.h                  App version (1.2.0)
│   ├── ApiClient.*                HTTP client with bearer auth + refresh
│   ├── AuthService.*              Login/logout/reset services
│   ├── ProductService.*           Product and scanning APIs
│   ├── AssetService.*              Asset APIs
│   ├── UserService.*               User APIs
│   ├── ReportService.*             Report APIs
│   ├── TruckStockService.*         Truck/stock workflow APIs
│   ├── AiService.*                 AI assistant API client
│   ├── DashboardWindow.*          Main authenticated window
│   ├── *Page.* / *Dialog.*        UI pages and dialogs
│   ├── app_icon.*                 App icon assets
│   └── ...
├── mobile/                         Flutter app
│   ├── lib/
│   ├── android/
│   ├── ios/
│   ├── pubspec.yaml
│   └── README.md
├── docs/                           Documentation source and PDF builder
├── installer/                      Inno Setup installer script
├── deploy.ps1                      Windows deploy and packaging script
├── InventoryQtApp.slnx            Visual Studio solution wrapper
├── InventoryQtApp.vcxproj         Legacy desktop project file
└── README.md                       This file
```

---

## Technology stack

### Desktop app (`InventoryQtApp/`)

- C++17
- Qt 6 (Core, Gui, Widgets, Network, Svg)
- cpr for HTTP requests
- nlohmann/json for JSON parsing
- QSettings for local configuration storage
- Optional Excel export via QXlsx when available
- Windows packaging via Inno Setup and `windeployqt`

### Mobile app (`mobile/`)

- Flutter 3 / Dart
- Provider for state management
- Dio for HTTP
- flutter_secure_storage for secure token storage
- shared_preferences for application configuration
- mobile_scanner for barcode scanning
- image_picker for receipts and uploads
- fl_chart for dashboard charts

### Shared runtime configuration

The desktop app resolves configuration in this order:

1. environment variables
2. `QSettings`
3. compiled default values from `Config.h`

The app identity is set to:

- organization: `InventorySystem`
- application: `InventoryQtApp`

Session/config persistence goes to the per-user app-data directory on Windows, with a rolling log file under `%APPDATA%/InventorySystem/InventoryQtApp/inventory-app.log`.

---

## Default desktop app configuration

The desktop app default endpoints are defined in `InventoryQtApp/Config.h`:

```cpp
DEFAULT_API_BASE_URL = "https://inventory-system-api-production.up.railway.app"
DEFAULT_UPDATE_SERVER_URL = "https://inventory-system-api-production.up.railway.app/updates/latest"
```

Environment variables supported by the desktop client:

- `INVENTORY_APP_API_URL`
- `INVENTORY_APP_UPDATE_URL`

QSettings keys used when no environment variable is set:

- `api/baseUrl`
- `updates/checkUrl`

The app also persists login state in the `InventorySystem/InventoryQtApp` registry scope and stores auth tokens through `SecureStore` (Windows DPAPI-encrypted storage).

---

## Architecture and application flow

The desktop app is structured as a Qt main window that authenticates the user, then opens the dashboard with a permission-aware sidebar and page set.

```text
Login window (InventoryQtApp)
    ↓
AuthService -> /auth/login
    ↓
ApiClient (bearer tokens + single-flight refresh on 401)
    ↓
Token validation -> /auth/validate
    ↓
DashboardWindow + SidebarWidget + page controllers
    ├── ProductService -> /products, /products/scan-in, /products/scan-out, /products/low-stock
    ├── AssetService -> /assets
    ├── UserService -> /users
    ├── ReportService -> /reports
    ├── TruckStockService -> /truck-stock/*
    └── AiService -> /ai/*
```

### Security / auth behavior

The `ApiClient` implementation does the following:

- injects `Authorization: Bearer <token>` on authenticated requests
- treats `401` responses as a token-refresh trigger
- uses a refresh flow that is serialized with a mutex to avoid concurrent refresh storms
- excludes `/auth/login` and `/auth/refresh` from auth-header injection
- retries protected requests after a successful refresh

Login state is saved locally when `Remember me` is enabled. On startup, the app attempts to resume the session by validating the saved access token and refresh token.

---

## Desktop app endpoints used by the client

The Qt client calls these REST API endpoints as part of the implemented services.

### Authentication

- `POST /auth/login`
- `POST /auth/logout`
- `POST /auth/request-reset`
- `POST /auth/reset-password`
- `GET /auth/validate`
- `POST /auth/refresh`

### Products and inventory

- `GET /products?page=1&limit=100`
- `POST /products`
- `PUT /products/:id`
- `DELETE /products/:id`
- `POST /products/scan-in`
- `POST /products/scan-out`
- `GET /products/low-stock`

### Assets

- `GET /assets`
- `POST /assets`
- `PUT /assets/:id`
- `DELETE /assets/:id`

### Users

- `GET /users`
- `POST /users/invite`
- `PUT /users/:id`
- `DELETE /users/:id`
- `PATCH /users/:id`
- `GET /users/me`
- `POST /users/:id/reset-password`

### Reports

- `GET /reports/inventory-summary`
- `GET /reports/assets-summary`

### Truck stock / fleet workflows

- `GET /truck-stock/trucks`
- `POST /truck-stock/trucks`
- `PUT /truck-stock/trucks/:id`
- `DELETE /truck-stock/trucks/:id`
- `GET /truck-stock/templates`
- `POST /truck-stock/templates`
- `PUT /truck-stock/templates/:id`
- `DELETE /truck-stock/templates/:id`
- `POST /truck-stock/assignments`
- `GET /truck-stock/assignments`
- `GET /truck-stock/my-stock`
- `POST /truck-stock/use-item`
- `GET /truck-stock/low-stock`
- `GET /truck-stock/receipts`
- `GET /truck-stock/movements`
- `POST /truck-stock/receipts`
- `POST /truck-stock/receipts/upload`
- `POST /truck-stock/receipts/extract`
- `PATCH /truck-stock/receipts/:id/status`

### AI assistant

- `POST /ai/chat`
- `GET /ai/status`

---

## Prerequisites and installation

### Desktop app prerequisites

- Windows 10 or 11 x64
- Visual Studio 2022 with MSBuild and C++ tooling
- Qt 6.11.0 (MSVC 2022 64-bit) installed and registered for Qt VS Tools
- vcpkg with `cpr`, `nlohmann-json`, and `qxlsx`
- Inno Setup (`ISCC.exe`) for packaging the installer

The repository includes:

- `InventoryQtApp.slnx` for the Visual Studio/MSBuild workflow
- `InventoryQtApp/CMakeLists.txt` for portable CMake builds
- `InventoryQtApp/vcpkg.json` for native dependencies

### Mobile app prerequisites

- Flutter 3.33.0 or newer
- Dart SDK compatible with `mobile/pubspec.yaml` (`>=3.6.0 <4.0.0`)

---

## Building and running the desktop app

### Option 1: Visual Studio / MSBuild (Windows)

From the repo root:

```powershell
msbuild InventoryQtApp.slnx /p:Configuration=Release /p:Platform=x64 /m
```

The project also includes a deployment helper:

```powershell
./deploy.ps1 -Version 1.2.0 -QtDir "C:\Qt\6.11.0\msvc2022_64"
```

This script:

1. locates MSBuild via `vswhere.exe`
2. builds Release/x64
3. stages the app output under `deploy/InventoryQtApp`
4. runs `windeployqt` to copy the Qt runtime
5. copies the `cpr`, `libcurl`, and `zlib` DLLs necessary to run the app

### Option 2: CMake + vcpkg

```powershell
cmake -S InventoryQtApp -B build `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
  -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/msvc2022_64" `
  -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release
```

The CMake project uses:

- `Qt6::Core`, `Qt6::Gui`, `Qt6::Widgets`, `Qt6::Network`, `Qt6::Svg`
- `cpr::cpr`
- `nlohmann_json::nlohmann_json`
- `QXlsx::QXlsx` when available

### Building the installer

```powershell
ISCC.exe /DAppVersion=1.2.0 installer\InventoryQtApp.iss
```

The current app version is defined in `InventoryQtApp/Version.h` as `1.2.0`.

---

## Running the desktop app

Once built, launch the executable produced by the chosen build path.

The app opens a login window titled `Stockvio — Sign in` and performs a version check using the update URL configured in `Config.h`.

### Runtime configuration

Set the API URL at runtime in one of these ways:

- environment variable: `INVENTORY_APP_API_URL`
- QSettings key: `api/baseUrl`
- app Settings page in the running UI (when the app is already open)

The update endpoint can be overridden with:

- environment variable: `INVENTORY_APP_UPDATE_URL`
- QSettings key: `updates/checkUrl`

The app defaults to:

```text
API: https://inventory-system-api-production.up.railway.app
Update: https://inventory-system-api-production.up.railway.app/updates/latest
```

---

## Mobile app setup

From the repo root:

```bash
cd mobile
flutter pub get
flutter run
```

The mobile app includes the `flutter_launcher_icons` setup and expects the generated icon assets under `mobile/assets/icon/`.

To generate launcher icons:

```bash
cd mobile
dart run flutter_launcher_icons
```

The app’s configuration file stores the backend API base URL in `shared_preferences` and defaults to an HTTPS host, with the runtime configuration flow handled in `mobile/lib/config/app_config.dart` pattern and UI settings screens.

---

## Testing and validation

This repository does not ship a desktop-application automated test suite. The project’s verification path is primarily build/run validation plus the documentation in `docs/`.

### Manual desktop validation workflow

1. Build the app using Visual Studio or CMake.
2. Launch the executable.
3. Log in to a valid backend instance.
4. Test product, asset, scan-in/scan-out, truck-stock, receipt, and report flows.
5. Confirm the app uses the configured backend API and saves login state correctly.

The project includes documentation and test planning materials in `docs/`:

- `docs/documentation.md`
- `docs/test-plan.md`
- `docs/build_pdf.py`
- `docs/build_test_plan.py`

### Mobile app testing

The Flutter project includes `flutter_test` as a dev dependency, so the standard validation is:

```bash
cd mobile
flutter test
```

---

## Build and deployment notes

### Desktop packaging

The installer script is located at `installer/InventoryQtApp.iss`.

Build the package with:

```powershell
ISCC.exe /DAppVersion=1.2.0 installer\InventoryQtApp.iss
```

### Startup and versioning

- `InventoryQtApp/Version.h` defines the single-source version value (`1.2.0`)
- `InventoryQtApp/main.cpp` writes the version into `QSettings` under `app/version`
- the auto-update manager compares the current version against the backend update response

### Log file location

The desktop app installs a Qt message handler that logs to a per-user rolling file at:

```text
%APPDATA%/InventorySystem/InventoryQtApp/inventory-app.log
```

---

## Important implementation notes

- The Qt app is a client only; it depends on a backend API service.
- The app uses a single API client instance and refreshes tokens transparently when a request receives `401`.
- The app saves user credentials and tokens with `QSettings` and Windows DPAPI-based encryption for the `Remember me` flow.
- The desktop app includes a permission-aware sidebar; the server is expected to enforce permissions for each route.
- The AI assistant page is available to authenticated users and sends chat requests to `/ai/chat`.

---

## Quick reference

```powershell
# Desktop: Visual Studio build
msbuild InventoryQtApp.slnx /p:Configuration=Release /p:Platform=x64 /m

# Desktop: deploy package
./deploy.ps1 -Version 1.2.0 -QtDir "C:\Qt\6.11.0\msvc2022_64"

# Desktop: CMake build
cmake -S InventoryQtApp -B build -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/msvc2022_64" -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Desktop: installer
ISCC.exe /DAppVersion=1.2.0 installer\InventoryQtApp.iss

# Mobile
cd mobile
flutter pub get
flutter run
```

---

## Related documentation

- `docs/documentation.md` — system overview, architecture, and build instructions
- `docs/test-plan.md` — manual QA checklist for the wider platform
- `mobile/README.md` — Flutter app-specific setup and architecture
- `docs/README.md` — PDF generation instructions

This README focuses on the code that exists in this repository and the actual configuration files, commands, and endpoints found in the implementation.
