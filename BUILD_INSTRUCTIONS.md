# Build Instructions - MOC Generation Fix

## Issue
Two new Qt classes require Meta-Object Compiler (MOC) generation:
- `AutoUpdateManager` (used for auto-update feature)
- `ReportsPage` (for the reports UI)

## Solution

### Option 1: Automatic (Recommended)
1. Close Visual Studio
2. Delete the build cache and intermediate files:
   - Delete `x64/` folder (build output)
   - Delete `.qt/` folders if they exist
3. Reopen solution in Visual Studio
4. Right-click Solution → "Rebuild Solution"
5. VS will auto-detect new header files and generate MOC files

### Option 2: Manual
1. In Visual Studio, right-click `AutoUpdateManager.h`
2. Select "Qt Meta Object Compiler"
3. Right-click `ReportsPage.h`
4. Select "Qt Meta Object Compiler"
5. Rebuild Solution

### Option 3: Project File Edit
If using CMake, add to CMakeLists.txt:
```cmake
set(PROJECT_SOURCES
    ...existing files...
    ReportsPage.h
    ReportsPage.cpp
    AutoUpdateManager.h
    AutoUpdateManager.cpp
)

set(PROJECT_UI_FILES
    ...existing files...
    ReportsPage.ui
)
```

If using .vcxproj (Visual Studio), ensure these sections exist:
```xml
<ItemGroup>
    <ClInclude Include="ReportsPage.h" />
    <ClCompile Include="ReportsPage.cpp" />
    <ClInclude Include="AutoUpdateManager.h" />
    <ClCompile Include="AutoUpdateManager.cpp" />
</ItemGroup>

<ItemGroup>
    <QtUic Include="ReportsPage.ui" />
</ItemGroup>
```

## Verification
After build completes, check for generated files:
- `x64/Debug/qt/moc/moc_ReportsPage.cpp`
- `x64/Debug/qt/moc/moc_AutoUpdateManager.cpp`
- `x64/Debug/qt/uic/ui_ReportsPage.h`

If these files exist, the build should succeed on next attempt.

## Common Issues

### "Cannot open source file" errors
- MOC files not generated
- → Run "Rebuild Solution" after adding files to project

### Unresolved external symbol errors
- Object files exist but MOC wasn't compiled
- → Clean build cache, delete x64/ folder, rebuild

### Qt Designer can't find ui_ReportsPage.h
- UIC (UI Compiler) hasn't generated header
- → Run "Qt → Run qmake" from menu

## Files that need MOC:

### AutoUpdateManager.h (line 21)
```cpp
class AutoUpdateManager : public QObject
{
    Q_OBJECT  // <-- This requires MOC
    ...
signals:
    void updateAvailable(const UpdateInfo& info);
```

### ReportsPage.h (line 15)
```cpp
class ReportsPage : public QWidget
{
    Q_OBJECT  // <-- This requires MOC
    ...
private slots:
    void onInventorySummaryClicked();
```

Both classes inherit from Qt classes and use the `Q_OBJECT` macro, which requires MOC code generation.

## After Successful Build

Run these tests:
1. Application starts without errors
2. Login window loads
3. ReportsPage appears in Reports menu
4. Remember Me checkbox functionality works
5. Export buttons on ReportsPage respond to clicks
