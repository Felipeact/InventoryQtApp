; Inno Setup script for InventoryQtApp
; Build: ISCC.exe installer\InventoryQtApp.iss
;   override version: ISCC.exe /DAppVersion=1.2.0 installer\InventoryQtApp.iss
; Produces a single setup .exe that also serves as the auto-update payload
; (matches the InventoryQtApp-<version>.exe name AutoUpdateManager downloads).

#ifndef AppVersion
  #define AppVersion "1.2.0"
#endif
#define AppName "StockPilot"
#define AppPublisher "StockPilot"
#define StageDir "..\deploy\InventoryQtApp"

[Setup]
AppId={{151D8979-7F5D-4846-8F9C-9468AB2E496D}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
VersionInfoVersion={#AppVersion}
DefaultDirName={autopf}\{#AppName}
DefaultGroupName={#AppName}
UninstallDisplayName={#AppName}
UninstallDisplayIcon={app}\InventoryQtApp.exe
OutputDir=Output
OutputBaseFilename=InventoryQtApp-{#AppVersion}
Compression=lzma2
SolidCompression=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
WizardStyle=modern
CloseApplications=force
; SetupIconFile=app.ico   ; uncomment once an .ico exists

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional icons:"

[Files]
Source: "{#StageDir}\*"; DestDir: "{app}"; Excludes: "vc_redist.x64.exe"; \
    Flags: recursesubdirs createallsubdirs ignoreversion
; VC++ runtime staged by windeployqt --compiler-runtime (skipped silently if absent)
Source: "{#StageDir}\vc_redist.x64.exe"; DestDir: "{tmp}"; \
    Flags: deleteafterinstall skipifsourcedoesntexist

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\InventoryQtApp.exe"
Name: "{group}\Uninstall {#AppName}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#AppName}"; Filename: "{app}\InventoryQtApp.exe"; Tasks: desktopicon

[Run]
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; \
    StatusMsg: "Installing Visual C++ runtime..."; Check: VCRedistPresent; \
    Flags: waituntilterminated
Filename: "{app}\InventoryQtApp.exe"; Description: "Launch {#AppName}"; \
    Flags: nowait postinstall skipifsilent

[Code]
function VCRedistPresent: Boolean;
begin
  Result := FileExists(ExpandConstant('{tmp}\vc_redist.x64.exe'));
end;
