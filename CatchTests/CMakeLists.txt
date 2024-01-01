cmake_minimum_required(VERSION 3.5)

project(ReminiTests LANGUAGES CXX)

enable_testing()

SET(CMAKE_CXX_STANDARD 17)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Gui Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui Test)

include_directories("${CMAKE_SOURCE_DIR}")

add_executable(${PROJECT_NAME}
    main.cpp
    navigationview_tests.cpp
    mkedit_tests.cpp
    mktextdocument_tests.cpp
    RecentFilesDialog_Tests.cpp
    ${CMAKE_SOURCE_DIR}/blockdata.cpp
    ${CMAKE_SOURCE_DIR}/mkedit.cpp
    ${CMAKE_SOURCE_DIR}/linedata.cpp
    ${CMAKE_SOURCE_DIR}/blockdata.cpp
    ${CMAKE_SOURCE_DIR}/formatdata.cpp
    ${CMAKE_SOURCE_DIR}/mktextdocument.cpp
    ${CMAKE_SOURCE_DIR}/recentfilesdialog.cpp


)

target_link_libraries(ReminiTests PRIVATE Qt${QT_VERSION_MAJOR}::Gui
                                          Qt${QT_VERSION_MAJOR}::Widgets
                                          Qt${QT_VERSION_MAJOR}::Test)


if (DEFINED ENV{CATCH_INCLUDE_DIR})
    set(CATCH_INCLUDE_DIR $ENV{CATCH_INCLUDE_DIR})
else ()
    set(CATCH_INCLUDE_DIR "") # set by Qt Creator wizard
endif ()
if (CATCH_INCLUDE_DIR STREQUAL "")
    message("CATCH_INCLUDE_DIR is not set, assuming Catch2 can be found automatically in your system")
elseif (EXISTS ${CATCH_INCLUDE_DIR})
    include_directories(${CATCH_INCLUDE_DIR})
endif ()


message(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
message(PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
