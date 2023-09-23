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
        editcommand.h
        editcommand.cpp
        theme.h
        navigationview.h
        navigationview.cpp
        navigationmodel.h
        navigationmodel.cpp
        formatdata.h
        formatdata.cpp
        searchalldialog.h
        searchalldialog.cpp
        textsearchworker.h
        textsearchworker.cpp
        transparentdialog.h
        transparentdialog.cpp
        appeventfilter.h
        appeventfilter.cpp
        resources/icons.qrc
        startuptext.h
        recentfilesdialog.h
        recentfilesdialog.cpp
        settingsdialog.h
        settingsdialog.cpp
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Remini
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        settingsdialog.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Remini APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Remini SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Remini
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Add the test directory to the build process
add_subdirectory(CatchTests)

target_link_libraries(Remini PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Run the tests as part of the build process
add_test(NAME CatchTests COMMAND CatchTests)

set_target_properties(Remini PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Remini)
endif()
