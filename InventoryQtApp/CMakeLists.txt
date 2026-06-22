# Portable CMake build for the Inventory Qt desktop app.
#
# This complements the Visual Studio .vcxproj (it does not replace it) and enables
# reproducible, headless/CI builds with Qt6 + vcpkg.
#
# Configure (from the repo root, using the vcpkg manifest in this folder):
#   cmake -S InventoryQtApp -B build \
#     -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake \
#     -DCMAKE_PREFIX_PATH=C:/Qt/6.11.0/msvc2022_64 \
#     -DCMAKE_BUILD_TYPE=Release
#   cmake --build build --config Release

cmake_minimum_required(VERSION 3.21)

project(InventoryQtApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Let CMake run moc/uic/rcc automatically for Qt sources.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Svg)
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Collect sources from this directory. GLOB keeps the file list in one place; re-run
# CMake configure after adding/removing files.
file(GLOB APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB APP_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB APP_FORMS   CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")
file(GLOB APP_RES     CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc")

# Windows executable icon (compiled from app_icon.rc → app_icon.ico). Windows-only;
# MSVC enables the RC language automatically.
set(APP_WIN_RC "")
if(WIN32)
    set(APP_WIN_RC "${CMAKE_CURRENT_SOURCE_DIR}/app_icon.rc")
endif()

# WIN32 => GUI subsystem (no console window) on Windows.
add_executable(InventoryQtApp WIN32
    ${APP_SOURCES}
    ${APP_HEADERS}
    ${APP_FORMS}
    ${APP_RES}
    ${APP_WIN_RC}
)

target_link_libraries(InventoryQtApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Svg
    cpr::cpr
    nlohmann_json::nlohmann_json
)

# Optional Excel export via QXlsx; enabled automatically when the package is available.
find_package(QXlsx CONFIG QUIET)
if(QXlsx_FOUND)
    target_compile_definitions(InventoryQtApp PRIVATE QXLSX_LIBRARY)
    target_link_libraries(InventoryQtApp PRIVATE QXlsx::QXlsx)
    message(STATUS "QXlsx found - Excel export enabled")
else()
    message(STATUS "QXlsx not found - Excel export disabled (CSV/PDF still available)")
endif()

# Suppress qDebug output in Release builds (mirrors the .vcxproj behaviour).
target_compile_definitions(InventoryQtApp PRIVATE
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)

# Deploy Qt runtime DLLs next to the executable on Windows (windeployqt).
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET InventoryQtApp POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations --compiler-runtime
                    "$<TARGET_FILE:InventoryQtApp>"
            COMMENT "Running windeployqt")
    endif()
endif()
