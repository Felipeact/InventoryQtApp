# Implementation Details & Code References

## 1. ReportsPage Integration

### Files Created/Modified:
- **Created**: `ReportsPage.h`, `ReportsPage.cpp`, `ReportsPage.ui`
- **Modified**: `DashboardWindow.h`, `DashboardWindow.cpp`

### How It Works:
```cpp
// DashboardWindow.cpp
reportsPage = new ReportsPage(reportService, this);
ui.mainStack->addWidget(reportsPage);

// In setupSidebar():
connect(sidebar, &SidebarWidget::reportsClicked, this, [this]() {
    reportsPage->refreshReports();
    ui.mainStack->setCurrentWidget(reportsPage);
});
```

### ReportsPage Features:
- Fetches inventory & assets summaries from ReportService
- Displays data in QTableWidget
- Export to CSV/PDF/Excel
- Refresh functionality

## 2. Remember Me Implementation

### Files Modified:
- `InventoryQtApp.h` (added methods)
- `InventoryQtApp.cpp` (added implementation)

### Storage:
Uses `QSettings` (Windows Registry / ini file):
```cpp
QSettings settings("InventorySystem", "InventoryQtApp");
settings.setValue("login/email", email);
settings.setValue("login/token", token);
settings.setValue("login/rememberMe", true);
```

### Auto-Login Flow:
1. App loads → `loadSavedCredentials()` is called
2. If Remember Me was checked previously:
   - Email is populated
   - Token is validated with backend
   - If valid → automatically login and show dashboard
   - If invalid → clear credentials, show login form

## 3. Forgot Password

### Implementation:
```cpp
void InventoryQtApp::onForgotPasswordClicked()
{
    QString email = ui.emailInput->text();
    if (email.isEmpty()) {
        QMessageBox::warning(this, "Forgot Password", 
                           "Please enter your email address.");
        return;
    }

    QMessageBox::information(this, "Password Reset",
        "Instructions sent to: " + email);

    // TODO: Call authService.requestPasswordReset(email.toStdString());
}
```

### To Complete:
Implement `requestPasswordReset()` in AuthService:
```cpp
void AuthService::requestPasswordReset(const std::string& email) {
    // Make API call to /api/auth/forgot-password
    // Backend sends reset link to user's email
}
```

## 4. Dynamic API URL Configuration

### Files Modified:
- `Config.h`

### Priority Order:
1. Environment variable: `INVENTORY_APP_API_URL`
2. QSettings: `api/baseUrl`
3. Default hardcoded value

### Usage:
```cpp
apiClient(Config::API_BASE_URL())  // Note: Function call, not constant

// To change at runtime:
Config::setApiBaseUrl("https://new-api.example.com");
```

### For Deployment:
Windows:
```batch
set INVENTORY_APP_API_URL=https://production-api.example.com
InventoryQtApp.exe
```

Linux:
```bash
export INVENTORY_APP_API_URL=https://production-api.example.com
./InventoryQtApp
```

## 5. Error Handling

### Files Created:
- `ErrorHandler.h`, `ErrorHandler.cpp`

### Common Errors Handled:
```cpp
// Network errors (status 0)
"Unable to connect to the server. Check internet connection."

// Authentication (401, 403)
"Unauthorized: Your session has expired."

// Server errors (500, 502, 503, 504)
"Server error: The server is temporarily unavailable."

// Client errors (400, 404)
"Bad Request: The server could not understand the request."
```

### Usage:
```cpp
if (response.status_code == 0) {
    // Network error
    QString msg = ErrorHandler::getErrorMessage(response);
}

if (ErrorHandler::isServerUnavailable(response)) {
    // Show offline message
}
```

## 6. Dashboard Data Replacement

### Before (Hardcoded):
```cpp
ui.itemListTable->setItem(row, 1, new QTableWidgetItem("Image"));
ui.itemListTable->setItem(row, 2, new QTableWidgetItem("Main Store"));
```

### After (Dynamic):
```cpp
std::string imageUrl = product.value("imageUrl", "N/A");
ui.itemListTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(imageUrl)));

std::string location = product.value("location", "Unknown Store");
ui.itemListTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(location)));
```

### Expected Backend Response:
```json
{
    "name": "Product Name",
    "imageUrl": "https://example.com/product.jpg",
    "location": "Warehouse A",
    "quantity": 150
}
```

## 7. Export Features

### Files Created:
- `ExportUtility.h`, `ExportUtility.cpp`

### Formats Supported:

**CSV Export:**
- Escapes quotes properly
- Headers included
- UTF-8 encoding

**PDF Export:**
- Uses QPdfWriter
- Includes title, timestamp, headers
- Table layout with pagination support

**Excel Export:**
- Currently saves as CSV (requires xlsxwriter library for true .xlsx)
- Can integrate xlsxwriter for production

### Usage in ReportsPage:
```cpp
void ReportsPage::exportToCsv() {
    if (ExportUtility::exportTableToCsv(ui.reportTable, fileName)) {
        QString size = ExportUtility::getFileSizeString(fileName);
        QMessageBox::information(...);
    }
}
```

## 8. Installer Configuration

### NSIS Script (installer/InventoryQtApp.nsi):
- Installs application to `Program Files\InventorySystem`
- Copies all Qt DLLs and plugins
- Creates Start Menu shortcuts
- Creates Desktop shortcut
- Adds uninstaller entry

### To Build Installer:
1. Install NSIS: https://nsis.sourceforge.io/
2. Edit paths in .nsi script
3. Right-click .nsi file → Compile
4. Output: `InventoryQtApp-Setup-1.0.0.exe`

### Qt Deployment Batch Script (deploy/deploy-qt.bat):
Automates copying dependencies:
```batch
copy "%QT_PATH%\bin\Qt6Core.dll" "%RELEASE_PATH%\"
copy "%QT_PATH%\plugins\platforms\qwindows.dll" "%RELEASE_PATH%\platforms\"
```

## 9. Auto-Update System

### Files Created:
- `AutoUpdateManager.h`, `AutoUpdateManager.cpp`

### Version Comparison:
```cpp
// Parses semantic versioning (e.g., "1.2.3")
bool AutoUpdateManager::isUpdateNeeded(const QString& latestVersion) {
    QString current = "1.0.0";
    QString latest = "1.0.1";
    // Returns: true (update available)
}
```

### Expected Update Server Response:
```json
{
    "version": "1.0.1",
    "downloadUrl": "https://releases.example.com/app-1.0.1.exe",
    "releaseNotes": "Bug fixes and improvements",
    "required": false,
    "changeLog": "- Fixed login issue\n- Improved performance"
}
```

### To Implement:
1. Set up update server endpoint
2. Implement network calls in AutoUpdateManager
3. Create version endpoint at: `/api/app/latest-version`
4. Enable QtNetwork in project configuration

### Signals Emitted:
```cpp
void updateAvailable(const UpdateInfo& info);
void noUpdateAvailable();
void updateError(const QString& msg);
```

## 10. Testing Each Feature

### ReportsPage:
```
Menu: Click Reports button
Expected: ReportsPage UI loads with two buttons and table
```

### Remember Me:
```
1. Check "Remember Me" checkbox
2. Enter credentials
3. Click Login
4. Close app
5. Reopen app
Expected: Email field auto-filled, auto-login occurs
```

### Error Messages:
```
1. Unplug network
2. Try to login
Expected: "Unable to connect" message with suggestions
```

### Dashboard Data:
```
1. Login
2. Go to Dashboard
3. Check table cells
Expected: Real data, not "Image" or "Main Store"
```

### Export:
```
1. Go to Reports
2. Click "Export CSV"
3. Check exported file
Expected: Properly formatted CSV with data
```

## Configuration Files

### Default Settings Location:
- **Windows**: `HKEY_CURRENT_USER\Software\InventorySystem\InventoryQtApp`
- **Linux**: `~/.config/InventorySystem/InventoryQtApp.conf`

### Stored Settings:
```
appearance/theme          "Light" or "Dark"
api/baseUrl              "https://api.example.com"
login/email              "user@example.com"
login/token              "auth_token_here"
login/rememberMe         true/false
app/version              "1.0.0"
```

## Debugging Tips

### Enable Debug Output:
```cpp
qDebug() << "ReportsPage loaded";
qDebug() << "Credentials saved for:" << email;
```

### Check Settings:
```cpp
QSettings settings("InventorySystem", "InventoryQtApp");
qDebug() << settings.allKeys();  // List all keys
```

### Network Errors:
```cpp
if (ErrorHandler::isNetworkError(response)) {
    qDebug() << "Status code:" << response.status_code;
    qDebug() << "Error:" << response.text;
}
```

## Performance Considerations

1. **Reports**: Cache data if frequently accessed
2. **Auto-update**: Check only on app startup
3. **Exports**: Show progress for large datasets
4. **Settings**: Use QSettings::sync() sparingly

## Security Notes

1. **Tokens**: Consider encryption before storing
2. **API URLs**: Use HTTPS in production
3. **Environment Variables**: Don't expose in logs
4. **Error Messages**: Don't expose sensitive data
