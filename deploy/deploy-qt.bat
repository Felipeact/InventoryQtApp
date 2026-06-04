@echo off
REM Qt Deployment Script for Inventory Qt Application
REM This script copies all necessary Qt dependencies to the release folder

setlocal enabledelayedexpansion

REM Define variables
set QT_PATH=C:\Qt\6.11.0\msvc2022_64
set RELEASE_PATH=x64\Release
set PLUGINS_PATH=%RELEASE_PATH%\plugins

echo ========================================
echo Qt Deployment Script
echo ========================================

REM Create directories
echo Creating directories...
if not exist "%PLUGINS_PATH%" mkdir "%PLUGINS_PATH%"
if not exist "%RELEASE_PATH%\platforms" mkdir "%RELEASE_PATH%\platforms"
if not exist "%RELEASE_PATH%\styles" mkdir "%RELEASE_PATH%\styles"
if not exist "%RELEASE_PATH%\imageformats" mkdir "%RELEASE_PATH%\imageformats"

REM Copy core Qt DLLs
echo Copying Qt core libraries...
copy "%QT_PATH%\bin\Qt6Core.dll" "%RELEASE_PATH%\" /Y
copy "%QT_PATH%\bin\Qt6Gui.dll" "%RELEASE_PATH%\" /Y
copy "%QT_PATH%\bin\Qt6Widgets.dll" "%RELEASE_PATH%\" /Y
copy "%QT_PATH%\bin\Qt6Network.dll" "%RELEASE_PATH%\" /Y
copy "%QT_PATH%\bin\Qt6Sql.dll" "%RELEASE_PATH%\" /Y
copy "%QT_PATH%\bin\Qt6Xml.dll" "%RELEASE_PATH%\" /Y
copy "%QT_PATH%\bin\Qt6PrintSupport.dll" "%RELEASE_PATH%\" /Y
copy "%QT_PATH%\bin\Qt6Concurrent.dll" "%RELEASE_PATH%\" /Y

REM Copy platform plugins
echo Copying platform plugins...
copy "%QT_PATH%\plugins\platforms\qwindows.dll" "%RELEASE_PATH%\platforms\" /Y

REM Copy styles
echo Copying styles...
copy "%QT_PATH%\plugins\styles\*" "%RELEASE_PATH%\styles\" /Y

REM Copy image formats
echo Copying image formats...
copy "%QT_PATH%\plugins\imageformats\*" "%RELEASE_PATH%\imageformats\" /Y

REM Copy Visual C++ runtime
echo Copying Visual C++ runtime...
copy "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.41.33906\x64\Microsoft.VC143.CRT\msvcp140.dll" "%RELEASE_PATH%\" /Y
copy "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.41.33906\x64\Microsoft.VC143.CRT\vcruntime140.dll" "%RELEASE_PATH%\" /Y

REM Create config directory and sample config
echo Creating configuration...
if not exist "%RELEASE_PATH%\config" mkdir "%RELEASE_PATH%\config"

echo Deployment complete!
echo.
echo Release files are located in: %RELEASE_PATH%
echo.
pause
