# Final Setup Guide - Complete Implementation

This guide covers the final steps needed to complete the production-ready implementation.

## 1. Enable QtNetwork Module (Required for Auto-Update)

### Visual Studio Project File (.vcxproj)

Open `InventoryQtApp.vcxproj` and locate the Qt configuration. Ensure the following modules are included:

```xml
<QtModules>core;gui;widgets;network</QtModules>
```

Make sure `network` is added to the existing modules list.

### CMakeLists.txt (if using CMake)

```cmake
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Gui 
    Widgets 
    Network
)

target_link_libraries(InventoryQtApp PRIVATE
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets 
    Qt6::Network
)
```

### Qt .pro file (if using qmake)

```pro
QT += core gui widgets network

CONFIG += c++17
```

## 2. Optional: Add QXlsx for Real Excel Export

For true .xlsx support (recommended), integrate QXlsx library:

### Option A: Using vcpkg (Recommended)

```bash
vcpkg install qxlsx:x64-windows
```

Then add to your project:
- Include paths: `vcpkg_installed/x64-windows/include`
- Library paths: `vcpkg_installed/x64-windows/lib`
- Link library: `qxlsx`

### Option B: Build from Source

1. Clone QXlsx: `https://github.com/QtExcel/QXlsx`
2. Add to your project and build with it
3. Add `QXLSX_LIBRARY` to your project's preprocessor definitions

### Option C: Use libxlsxwriter (C library)

```bash
# vcpkg
vcpkg install xlsxwriter:x64-windows

# Or download from: https://github.com/jmcnamara/libxlsxwriter
```

### Fallback: CSV Export

If no Excel library is available, the app will automatically fall back to CSV export.

## 3. Production URLs Configuration

### Update API Base URL

Edit `Config.h`:
```cpp
const std::string DEFAULT_API_BASE_URL = "https://your-production-api.com";
```

Set via environment variable:
```batch
set INVENTORY_APP_API_URL=https://your-production-api.com
```

Or in QSettings at runtime via Settings page.

### Update Server URL

Edit `InventoryQtApp.cpp` line ~48:
```cpp
QString updateServerUrl = "https://your-update-server.com/api/app/latest-version";
```

Expected JSON response from update server:
```json
{
  "version": "2.0.0",
  "downloadUrl": "https://releases.your-company.com/InventoryQtApp-2.0.0.exe",
  "releaseNotes": "Bug fixes and improvements",
  "required": false,
  "changeLog": "Version 2.0.0..."
}
```

## 4. Visual Studio Toolset Update (v145 → v143)

For Visual Studio 2022 compatibility:

1. Right-click project → Properties
2. Configuration Properties → General
3. Set "Platform Toolset" to `Visual Studio 2022 (v143)`
4. Apply to all configurations (Debug and Release)
5. Rebuild solution

## 5. Complete Feature Checklist

### ✅ Auto-Update (NOW REAL)
- [x] QtNetwork module integrated
- [x] QNetworkAccessManager for HTTP requests
- [x] Version comparison logic
- [x] Update download to temp directory
- [x] Progress signals for UI feedback
- [x] Auto-check on app startup

**What to do:**
- Configure your update server endpoint
- Test with a real update check

### ✅ Forgot Password (NOW CONNECTED)
- [x] Connected to `/auth/request-reset` endpoint
- [x] Email validation
- [x] User feedback UI
- [x] Error handling

**What to do:**
- Verify backend endpoint is accessible
- Test password reset email flow

### ✅ Remember Me (NOW COMPLETE)
- [x] Saves both access and refresh tokens
- [x] Auto-login on app restart
- [x] Refresh token support
- [x] Improved security notes (TODO: encryption)

**What to do:**
- Consider implementing DPAPI encryption for stored tokens
- Test auto-login flow

### ✅ Real Excel Export (CONDITIONAL)
- [x] QXlsx integration code
- [x] Fallback to CSV if library unavailable
- [x] Header formatting
- [x] Column auto-sizing

**What to do:**
- Install QXlsx if .xlsx support required
- Otherwise CSV export works as alternative

### ✅ Reports Theme (NOW IMPLEMENTED)
- [x] Dark/Light theme support
- [x] Table styling
- [x] Button styling
- [x] Header styling

**What to do:**
- Test theme switching in DashboardWindow
- Customize colors as needed

### ✅ Production API URL (INFRASTRUCTURE READY)
- [x] Config.h supports multiple sources (env var, settings, default)
- [x] Settings page can modify URL
- [x] QSettings persistence

**What to do:**
- Set your production API URL
- Test Settings UI to change API endpoint

## 6. Security Recommendations

### Token Storage (Current: Plain Text in QSettings)

**TODO for Production:**

**Windows (Recommended):**
```cpp
// Use DPAPI encryption
#include <windows.h>
#include <wincrypt.h>

QString EncryptToken(const QString& token) {
    DATA_BLOB dataIn = {0};
    DATA_BLOB dataOut = {0};

    QByteArray tokenBytes = token.toUtf8();
    dataIn.pbData = (BYTE*)tokenBytes.data();
    dataIn.cbData = tokenBytes.size();

    if (CryptProtectData(&dataIn, NULL, NULL, NULL, NULL, 0, &dataOut)) {
        return QString::fromLatin1(QByteArray((char*)dataOut.pbData, dataOut.cbData).toBase64());
    }
    return token; // Fallback
}
```

**Cross-Platform:**
```cpp
// Use Qt's SimpleEncryption or similar
// Or integrate OpenSSL for AES encryption
```

### API Security

- [ ] Use HTTPS only
- [ ] Implement CORS properly
- [ ] Add request signing if needed
- [ ] Implement token refresh logic
- [ ] Add rate limiting awareness

## 7. Testing Checklist

### Pre-Deployment Testing

- [ ] Auto-update check (no update available)
- [ ] Auto-update check (update available)
- [ ] Update download and install
- [ ] Forgot password flow
- [ ] Remember Me login
- [ ] Reports page rendering
- [ ] Excel export (if QXlsx available)
- [ ] PDF export
- [ ] CSV export
- [ ] Theme switching
- [ ] API URL changes via settings
- [ ] Offline error handling
- [ ] Token refresh on expiration

### Installer Testing

- [ ] Test on clean Windows 10/11 machine
- [ ] Verify all dependencies installed
- [ ] Check file associations
- [ ] Verify uninstall process
- [ ] Check Start menu shortcuts

## 8. Deployment Instructions

### Build Release Version

```bash
cd InventoryQtApp
mkdir build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

### Create Installer

```bash
# Copy build artifacts
# Run NSIS installer builder
"C:\Program Files (x86)\NSIS\makensis.exe" installer.nsi
```

### Deploy Update Server

Ensure your update server responds to:
```
GET /api/app/latest-version
```

With JSON response containing version info and download URL.

## 9. Troubleshooting

### QtNetwork Module Not Found

```bash
# Ensure Qt6Network is installed
# Rebuild cmake cache
cmake --fresh .
```

### QXlsx Compilation Errors

1. Check include paths
2. Verify library linking
3. Consider using CSV fallback instead

### Auto-Update Not Working

- Verify update server URL is accessible
- Check network connectivity
- Review update server JSON format
- Check temp directory permissions

### Token Storage Issues

- Verify QSettings registry path
- Check user permissions for registry access
- On Linux, check ~/.config permissions

## 10. Next Steps for Production

1. **Configure update server** with proper versioning and release URLs
2. **Set production API URLs** in Config.h and environment
3. **Implement token encryption** for secure storage
4. **Add logging** for debugging production issues
5. **Create user documentation** for password reset flow
6. **Set up monitoring** for update server availability
7. **Plan backup/recovery** procedures
8. **Test on multiple Windows versions** (10, 11, Server editions)
9. **Code sign the installer** and executable
10. **Set up CI/CD pipeline** for automated builds and updates

## Quick Reference: All TODO Items

Search your codebase for these markers:

- `TODO: Change this to your production update server URL`
- `TODO: Update this to your production API URL before deployment`
- `TODO: Implement secure token storage (e.g., using DPAPI on Windows or OS keychain)`
- `TODO: Configure your update server URL before deployment`
- `TODO: Implement token encryption for production`

All marked items need attention before production deployment.
