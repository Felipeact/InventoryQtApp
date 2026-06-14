# Unit tests for the InventoryQtApp business-logic layer.
#
# These tests deliberately avoid the network (cpr) and the full UI. They
# exercise the service layer through the IApiClient seam (via FakeApiClient),
# the pure JSON helpers, the error mapping, and the export/config utilities.

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport Concurrent Test)

# Prefer a system/package-provided nlohmann/json; fall back to fetching the
# single-header release so the tests build on platforms without it installed
# (e.g. Windows CI).
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

set(APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../InventoryQtApp)

# Source files under test. None of these depend on cpr any longer, so they can
# be compiled and linked without an HTTP stack.
add_library(sut STATIC
    ${APP_DIR}/ErrorHandler.cpp
    ${APP_DIR}/ExportUtility.cpp
    ${APP_DIR}/AssetService.cpp
    ${APP_DIR}/AuthService.cpp
    ${APP_DIR}/ProductService.cpp
    ${APP_DIR}/UserService.cpp
    ${APP_DIR}/ReportService.cpp
    ${APP_DIR}/TruckStockService.cpp
    ${APP_DIR}/CredentialStore.cpp
    ${APP_DIR}/AsyncApi.cpp
)
target_include_directories(sut PUBLIC ${APP_DIR})
target_link_libraries(sut PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
    Qt6::Concurrent
    nlohmann_json::nlohmann_json
)

# Helper to register a QtTest executable.
#   needs_gui: if TRUE the test runs with the offscreen QPA platform so widget
#   code works in a headless CI environment.
function(add_qt_test name needs_gui)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE sut Qt6::Test)
    add_test(NAME ${name} COMMAND ${name})
    if(needs_gui)
        set_tests_properties(${name} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
    endif()
endfunction()

add_qt_test(tst_errorhandler FALSE)
add_qt_test(tst_jsonutils FALSE)
add_qt_test(tst_urlutils FALSE)
add_qt_test(tst_services FALSE)
add_qt_test(tst_config FALSE)
add_qt_test(tst_credentialstore FALSE)
add_qt_test(tst_asyncapi FALSE)
add_qt_test(tst_exportutility TRUE)
