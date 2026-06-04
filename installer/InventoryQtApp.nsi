; NSIS Installer Script for Inventory Qt Application
; This script creates a Windows installer for the Inventory Management System

!include "MUI2.nsh"
!include "x64.nsh"

; Basic Settings
Name "Inventory Management System"
OutFile "InventoryQtApp-Setup-1.0.0.exe"
InstallDir "$PROGRAMFILES64\InventorySystem"

; Variables
Var StartMenuFolder

; MUI Settings
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

; Installer Sections
Section "Install Application Files"
    SetOutPath "$INSTDIR"

    ; Main executable
    File "x64\Release\InventoryQtApp.exe"

    ; Qt Runtime DLLs
    File "x64\Release\Qt6Core.dll"
    File "x64\Release\Qt6Gui.dll"
    File "x64\Release\Qt6Widgets.dll"
    File "x64\Release\Qt6Network.dll"
    File "x64\Release\Qt6Sql.dll"
    File "x64\Release\Qt6Xml.dll"
    File "x64\Release\Qt6PrintSupport.dll"

    ; C++ Runtime
    File "x64\Release\msvcp140.dll"
    File "x64\Release\vcruntime140.dll"

    ; Create platform folder
    SetOutPath "$INSTDIR\platforms"
    File "x64\Release\platforms\qwindows.dll"

    ; Create styles folder
    SetOutPath "$INSTDIR\styles"
    File "x64\Release\styles\*.*"

    ; Create config folder
    SetOutPath "$INSTDIR\config"
    File "config\*.ini"

    SetOutPath "$INSTDIR"

    ; Create Start Menu shortcuts
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
        CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
        CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Inventory Management.lnk" "$INSTDIR\InventoryQtApp.exe"
        CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\uninstall.exe"
    !insertmacro MUI_STARTMENU_WRITE_END

    ; Create Desktop shortcut
    CreateShortcut "$DESKTOP\Inventory Management.lnk" "$INSTDIR\InventoryQtApp.exe"

    ; Write uninstaller
    WriteUninstaller "$INSTDIR\uninstall.exe"

    ; Write registry entries for Add/Remove Programs
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\InventorySystem" "DisplayName" "Inventory Management System"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\InventorySystem" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\InventorySystem" "DisplayVersion" "1.0.0"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\InventorySystem" "DisplayIcon" "$INSTDIR\InventoryQtApp.exe"
SectionEnd

; Uninstaller Section
Section "Uninstall"
    ; Remove files
    Delete "$INSTDIR\InventoryQtApp.exe"
    Delete "$INSTDIR\*.dll"
    Delete "$INSTDIR\platforms\*.dll"
    Delete "$INSTDIR\styles\*.dll"
    Delete "$INSTDIR\config\*.ini"
    Delete "$INSTDIR\uninstall.exe"

    ; Remove directories
    RMDir "$INSTDIR\platforms"
    RMDir "$INSTDIR\styles"
    RMDir "$INSTDIR\config"
    RMDir "$INSTDIR"

    ; Remove Start Menu shortcuts
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    Delete "$SMPROGRAMS\$StartMenuFolder\Inventory Management.lnk"
    Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
    RMDir "$SMPROGRAMS\$StartMenuFolder"

    ; Remove Desktop shortcut
    Delete "$DESKTOP\Inventory Management.lnk"

    ; Remove registry entries
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\InventorySystem"
SectionEnd
