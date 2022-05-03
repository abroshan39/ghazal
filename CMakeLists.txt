cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME "Ghazal")
project(${PROJECT_NAME} VERSION 1.4 LANGUAGES C CXX)

if(UNIX)
    set(OUTPUT_FILE_NAME "ghazal")
else()
    set(OUTPUT_FILE_NAME "Ghazal")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 QUIET COMPONENTS Core)

if(QT_FOUND)
    set(QT_VERSION_MAJOR 5)
else()
    find_package(QT NAMES Qt6 QUIET COMPONENTS Core)
    if(QT_FOUND)
        set(QT_VERSION_MAJOR 6)
    else()
        set(QT_VERSION_MAJOR 5)  # If neither 5 nor 6 are found, we default to 5. The setup will fail further down.
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

if(QT_VERSION_MAJOR EQUAL 5)
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Sql Concurrent Xml Network)
    if(UNIX)
        set(QUAZIP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/libraries/unix/quazip/libquazip1-qt5.so")
        set(ZLIB "${CMAKE_CURRENT_SOURCE_DIR}/libraries/unix/quazip/libz.so")
    elseif(WIN32)
        set(QUAZIP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/libraries/win/quazip/libquazip1-qt5.dll")
        set(ZLIB "${CMAKE_CURRENT_SOURCE_DIR}/libraries/win/quazip/zlib1.dll")
    endif()
    set(GHAZAL_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Sql Qt5::Concurrent Qt5::Xml Qt5::Network ${QUAZIP_LIB})
elseif(QT_VERSION_MAJOR EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS Core Core5Compat Gui Widgets Sql Concurrent Xml Network)
    if(UNIX)
        set(QUAZIP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/libraries/unix/quazip/libquazip1-qt6.so")
        set(ZLIB "${CMAKE_CURRENT_SOURCE_DIR}/libraries/unix/quazip/libz.so")
    elseif(WIN32)
        set(QUAZIP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/libraries/win/quazip/libquazip1-qt6.dll")
        set(ZLIB "${CMAKE_CURRENT_SOURCE_DIR}/libraries/win/quazip/zlib1.dll")
    endif()
    set(GHAZAL_LIBRARIES Qt6::Core Qt6::Core5Compat Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Concurrent Qt6::Xml Qt6::Network ${QUAZIP_LIB})
endif()

set(HEADERS
    src/abjad_class.h
    src/abjadform.h
    src/abjadformmini.h
    src/aboutauthorform.h
    src/aboutform.h
    src/appthemes.h
    src/common_functions.h
    src/databaseform.h
    src/date_converter.h
    src/downloadform.h
    src/event_functions.h
    src/filedownloader.h
    src/mainwindow.h
    src/searchexamplesform.h
    src/searchform.h
    src/settingsform.h
    src/tabform.h
    src/wordsearchform.h
    src/worker.h
)

set(SOURCES
    src/abjad_class.cpp
    src/abjadform.cpp
    src/abjadformmini.cpp
    src/aboutauthorform.cpp
    src/aboutform.cpp
    src/appthemes.cpp
    src/common_functions.cpp
    src/common_search.cpp
    src/databaseform.cpp
    src/date_converter.c
    src/downloadform.cpp
    src/event_functions.cpp
    src/filedownloader.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow_action_menu.cpp
    src/mainwindow_app_setting.cpp
    src/mainwindow_search_form.cpp
    src/searchexamplesform.cpp
    src/searchform.cpp
    src/settingsform.cpp
    src/tabform.cpp
    src/tabform_context_menu.cpp
    src/wordsearchform.cpp
    src/worker.cpp
)

set(FORMS
    src/abjadform.ui
    src/abjadformmini.ui
    src/aboutauthorform.ui
    src/aboutform.ui
    src/databaseform.ui
    src/downloadform.ui
    src/mainwindow.ui
    src/searchexamplesform.ui
    src/searchform.ui
    src/settingsform.ui
    src/tabform.ui
    src/wordsearchform.ui
)

set(RESOURCES
    resources/themes/darkstyle.qrc
    resources/resource.qrc
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libraries/include")

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    enable_language(RC)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/resources/resource_win.rc")
    add_executable(${PROJECT_NAME} WIN32 ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES} ${APP_ICON_RESOURCE_WINDOWS})
else()
    add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})
endif()

target_link_libraries(${PROJECT_NAME} ${GHAZAL_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_FILE_NAME})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${QUAZIP_LIB}
    ${ZLIB}
    "${CMAKE_BINARY_DIR}")
