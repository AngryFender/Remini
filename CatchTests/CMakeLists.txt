cmake_minimum_required(VERSION 3.5)

project(CatchTests LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# FIX: AUTOUIC by default only searches the current source dir for .ui files.
# Since the tests live in a subdirectory but the .ui files live in the project
# root, the search path must be extended or `ui_mainwindow.h` / 
# `ui_settingsdialog.h` will never be generated.
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Gui Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui Test)

add_executable(${PROJECT_NAME}
    main.cpp
    MkEdit_Tests.cpp
    mktextdocument_tests.cpp
    NavigationView_Tests.cpp
    RecentFilesDialog_Tests.cpp

    ${CMAKE_SOURCE_DIR}/blockdata.cpp
    ${CMAKE_SOURCE_DIR}/linedata.cpp
    ${CMAKE_SOURCE_DIR}/formatdata.cpp
    ${CMAKE_SOURCE_DIR}/mkedit.cpp
    ${CMAKE_SOURCE_DIR}/mktextdocument.cpp
    ${CMAKE_SOURCE_DIR}/highlighter.cpp
    ${CMAKE_SOURCE_DIR}/views_handler.cpp
    ${CMAKE_SOURCE_DIR}/navigationview.cpp
    ${CMAKE_SOURCE_DIR}/navigationmodel.cpp
    ${CMAKE_SOURCE_DIR}/recentfilesdialog.cpp
    ${CMAKE_SOURCE_DIR}/settingsdialog.cpp

    # FIX: views_handler.cpp instantiates ToggleButton and owns a
    # TextSearchWorker member, so both translation units must be linked,
    # otherwise the build fails at link time with "undefined reference
    # to ToggleButton::ToggleButton" / "TextSearchWorker::...".
    ${CMAKE_SOURCE_DIR}/togglebutton.cpp
    ${CMAKE_SOURCE_DIR}/textsearchworker.cpp

    # FIX: add the .ui files explicitly so AUTOUIC has something to chew on.
    ${CMAKE_SOURCE_DIR}/mainwindow.ui
    ${CMAKE_SOURCE_DIR}/settingsdialog.ui
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Test
)

# FIX: windowapi.cpp is NOT compiled here (no reference to it from the
# test target), so X11 is not required. If you later add windowapi.cpp,
# re-enable this block.
# if (UNIX AND NOT APPLE)
#     find_package(X11 REQUIRED)
#     target_link_libraries(${PROJECT_NAME} PRIVATE X11::X11)
# endif()

if (DEFINED ENV{CATCH_INCLUDE_DIR})
    set(CATCH_INCLUDE_DIR $ENV{CATCH_INCLUDE_DIR})
else ()
    set(CATCH_INCLUDE_DIR "")
endif ()

if (CATCH_INCLUDE_DIR STREQUAL "")
    message("CATCH_INCLUDE_DIR is not set, assuming Catch2 can be found automatically in your system")
elseif (EXISTS ${CATCH_INCLUDE_DIR})
    target_include_directories(${PROJECT_NAME} PRIVATE ${CATCH_INCLUDE_DIR})
endif ()
