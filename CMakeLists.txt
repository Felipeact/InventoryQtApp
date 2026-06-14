# Root CMake project.
#
# The production application is built with the Visual Studio / Qt MSBuild
# project (InventoryQtApp.vcxproj). This CMake project exists so the
# dependency-light, business-logic units can be built and unit-tested in a
# portable way (locally and in CI) on any platform with Qt6 + nlohmann/json.
cmake_minimum_required(VERSION 3.16)

project(InventoryQtApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

enable_testing()

add_subdirectory(tests)

# Optionally build the full GUI application. This verifies that the entire app
# (all pages/dialogs, ApiClient, credential storage, async conversions) compiles
# and links. It requires cpr (fetched automatically) and a system libcurl.
# QXlsx remains optional; without it the Excel export falls back to CSV.
option(BUILD_APP "Build the full GUI application" OFF)
if(BUILD_APP)
    add_subdirectory(app)
endif()
