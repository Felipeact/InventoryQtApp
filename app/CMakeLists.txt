# Full GUI application build (portable CMake equivalent of the Visual Studio
# project). Primarily used to verify that the whole application compiles and
# links on a CI runner. The shipping Windows builds still use the .vcxproj.

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network PrintSupport Concurrent)

find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# cpr (HTTP). Use the system libcurl to keep the build fast.
include(FetchContent)
set(CPR_USE_SYSTEM_CURL ON CACHE BOOL "" FORCE)
set(CPR_ENABLE_SSL ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(cpr
    URL https://github.com/libcpr/cpr/archive/refs/tags/1.11.0.tar.gz
)
FetchContent_MakeAvailable(cpr)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/../InventoryQtApp)

file(GLOB APP_SOURCES CONFIGURE_DEPENDS ${SRC}/*.cpp)
file(GLOB APP_UIS CONFIGURE_DEPENDS ${SRC}/*.ui)

add_executable(InventoryApp ${APP_SOURCES} ${APP_UIS} ${SRC}/InventoryQtApp.qrc)
target_include_directories(InventoryApp PRIVATE ${SRC})
target_link_libraries(InventoryApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::PrintSupport
    Qt6::Concurrent
    cpr::cpr
    nlohmann_json::nlohmann_json
)
