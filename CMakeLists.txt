cmake_minimum_required(VERSION 3.5)

project(Remini VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# X11 is required on Linux for the global-hotkey implementation.
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
endif()

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    views_handler.h
    views_handler.cpp
    mkedit.h
    mkedit.cpp
    highlighter.h
    highlighter.cpp
    mktextdocument.h
    mktextdocument.cpp
    blockdata.h
    blockdata.cpp
    linedata.h
    linedata.cpp
    theme.h
    navigationview.h
    navigationview.cpp
    navigationmodel.h
    navigationmodel.cpp
    formatdata.h
    formatdata.cpp
    textsearchworker.h
    textsearchworker.cpp
    appeventfilter.h
    appeventfilter.cpp
    resources/icons.qrc
    startuptext.h
    recentfilesdialog.h
    recentfilesdialog.cpp
    settingsdialog.h
    settingsdialog.cpp
    settingsdialog.ui
    togglebutton.h
    togglebutton.cpp

    # Cross-platform: the .cpp contains #ifdef Q_OS_WIN / Q_OS_LINUX branches.
    windowapi.h
    windowapi.cpp
)

# Windows-only helper interface header.
if(WIN32)
    list(APPEND PROJECT_SOURCES
        iwindowapi.h
    )
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Remini
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(Remini SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Remini
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Remini PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Link X11 on Linux so windowapi.cpp can use XGrabKey / XGetInputFocus.
if(UNIX AND NOT APPLE)
    target_link_libraries(Remini PRIVATE X11::X11)
endif()

enable_testing()

add_subdirectory(CatchTests)

add_test(NAME CatchTests COMMAND CatchTests)

set_target_properties(Remini PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Remini)
endif()
