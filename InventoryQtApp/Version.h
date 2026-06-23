// Version.h - Single source of truth for the desktop application version.
//
// Keep this in sync with the installer (installer/InventoryQtApp.iss /DAppVersion)
// and the deploy script (deploy.ps1 -Version). main.cpp writes this value into the
// QSettings key `app/version`, which the auto-updater reads to compare against the
// latest release returned by the update server.
#pragma once

#define INVENTORY_APP_VERSION "1.2.0"
