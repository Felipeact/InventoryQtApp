<#
.SYNOPSIS
    Builds InventoryQtApp (Release|x64), gathers the Qt + third-party runtime
    with windeployqt, and stages a self-contained folder ready for Inno Setup.

.EXAMPLE
    .\deploy.ps1
    .\deploy.ps1 -Version 1.2.0 -SkipBuild
#>
[CmdletBinding()]
param(
    [string]$Version = "1.2.0",
    [string]$QtDir   = "C:\Qt\6.11.0\msvc2022_64",
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"
$root  = $PSScriptRoot
$rel   = Join-Path $root "x64\Release"
$stage = Join-Path $root "deploy\InventoryQtApp"
$exe   = Join-Path $rel "InventoryQtApp.exe"

# --- 1. Build Release|x64 -------------------------------------------------
if (-not $SkipBuild) {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) { throw "vswhere.exe not found. Install Visual Studio." }
    $msbuild = & $vswhere -latest -requires Microsoft.Component.MSBuild `
        -find "MSBuild\**\Bin\MSBuild.exe" | Select-Object -First 1
    if (-not $msbuild) { throw "MSBuild.exe not found." }

    Write-Host "Building Release|x64 ..." -ForegroundColor Cyan
    & $msbuild (Join-Path $root "InventoryQtApp.slnx") `
        /p:Configuration=Release /p:Platform=x64 /m /nologo /v:minimal
    if ($LASTEXITCODE -ne 0) { throw "Build failed." }
}

if (-not (Test-Path $exe)) { throw "Not found: $exe (build first or drop -SkipBuild)." }

# --- 2. Fresh staging folder ---------------------------------------------
if (Test-Path $stage) { Remove-Item $stage -Recurse -Force }
New-Item -ItemType Directory -Force $stage | Out-Null
Copy-Item $exe $stage

# --- 3. Qt runtime (DLLs, plugins, TLS backends, VC++ redist) ------------
$windeployqt = Join-Path $QtDir "bin\windeployqt.exe"
if (-not (Test-Path $windeployqt)) { throw "windeployqt not found at $windeployqt (set -QtDir)." }

Write-Host "Running windeployqt ..." -ForegroundColor Cyan
& $windeployqt --release --no-translations --compiler-runtime (Join-Path $stage "InventoryQtApp.exe")
if ($LASTEXITCODE -ne 0) { throw "windeployqt failed." }

# --- 4. Non-Qt third-party DLLs (vcpkg: cpr / curl / zlib) ----------------
foreach ($dll in "cpr.dll","libcurl.dll","zlib1.dll") {
    $src = Join-Path $rel $dll
    if (Test-Path $src) { Copy-Item $src $stage }
    else { Write-Warning "Missing $dll in $rel - app may fail to start." }
}

Write-Host "`nStaged at: $stage" -ForegroundColor Green
Write-Host "Next: ISCC.exe installer\InventoryQtApp.iss  (or run with /DAppVersion=$Version)" -ForegroundColor Green
