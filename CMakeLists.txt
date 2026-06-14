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
