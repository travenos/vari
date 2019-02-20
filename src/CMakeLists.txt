# Project VARI
# @author Alexey Barashkov

cmake_minimum_required(VERSION 3.1.0)

project(vari)
set(PROJECT_VENDOR_LONG "Wicron")
set(ORG_WEBSITE "https://wicron.com")
set(APP_NAME "VARI")

#Get information about the compilation time
string(TIMESTAMP CURRENT_YEAR "%Y")
string(TIMESTAMP CURRENT_TIME "%d.%m.%Y %H:%M:%S")

set(PROJECT_COPYRIGHT "Copyright (c) ${CURRENT_YEAR} ${PROJECT_VENDOR_LONG}")

set(EXE_NAME ${PROJECT_NAME})

message(STATUS "CREATING VARI APP\n")

#Find modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
include_directories(${PROJECT_SOURCE_DIR})
set(MODULES_FOLDERS "modules")

include(common)

#Set application version
find_package(Git REQUIRED)
set(GIT_RES_LONG  FALSE)
set(GIT_FILE_VERSION_LONG  "${CMAKE_CURRENT_BINARY_DIR}/git.version.long")

execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} RESULT_VARIABLE ${GIT_RES_LONG}  OUTPUT_FILE ${GIT_FILE_VERSION_LONG})

file(READ ${GIT_FILE_VERSION_LONG} READ_DATA_LONG)
string(COMPARE EQUAL "${READ_DATA_LONG}" "" NO_VERSION)
if(NOT NO_VERSION)
    string(REPLACE "\n" "" PROJECT_VERSION_LONG ${READ_DATA_LONG})
    string(REPLACE "v" "" PROJECT_VERSION_LONG ${PROJECT_VERSION_LONG})
else()
    set(PROJECT_VERSION_LONG "0.0")
endif()
SET_PROJECT_VER("${PROJECT_VERSION_LONG}")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION_LONG}")
message(STATUS "Project version: ${PROJECT_VERSION}\n")

#Configure a file with vendor definitions
set(VDEFINITIONS_SOURCE "${PROJECT_SOURCE_DIR}/cmake/VDefinitions.h.cmake")
set(VDEFINITIONS_DEST "${PROJECT_BINARY_DIR}/VDefinitions.h")
configure_file("${VDEFINITIONS_SOURCE}" "${VDEFINITIONS_DEST}")
include_directories(${PROJECT_BINARY_DIR})

#Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}\n")

#Set compiler flags
message(STATUS "Using C++ compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Using C compiler: ${CMAKE_C_COMPILER_ID}\n")
set(CMAKE_CXX_STANDARD 11)
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -fPIC")
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fPIC -pthread")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -DDEBUG_MODE -DQT_QML_DEBUG")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
    message(STATUS "C Flags: ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}\n")
else()
    message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
    message(STATUS "C Flags: ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}\n")
endif()

#Find packages
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{QTDIR})
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{OPENCV_DIR})

find_package(Qt5Core    REQUIRED)
find_package(Qt5Gui     REQUIRED)
find_package(Qt5Widgets     REQUIRED)
find_package(Qt5Sql     REQUIRED)

if (${Qt5Core_FOUND})
    message(STATUS "Found Qt 5 Core ${Qt5Core_VERSION}")
    message(STATUS "Qt 5 Core Directory: ${Qt5Core_DIR}")
    message(STATUS "Qt 5 Core Include directories: ${Qt5Core_INCLUDE_DIRS}\n")
    set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::Core)
    include_directories(${Qt5Core_INCLUDE_DIRS})
endif()
if (${Qt5Gui_FOUND})
    message(STATUS "Found Qt 5 Gui ${Qt5Gui_VERSION}")
    message(STATUS "Qt 5 Gui Directory: ${Qt5Gui_DIR}")
    message(STATUS "Qt 5 Gui Include directories: ${Qt5Gui_INCLUDE_DIRS}\n")
    set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::Gui)
    include_directories(${Qt5Gui_INCLUDE_DIRS})
endif()
if (${Qt5Widgets_FOUND})
    message(STATUS "Found Qt 5 Widgets ${Qt5Widgets_VERSION}")
    message(STATUS "Qt 5 Widgets Directory: ${Qt5Widgets_DIR}")
    message(STATUS "Qt 5 Widgets Include directories: ${Qt5Widgets_INCLUDE_DIRS}\n")
    set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::Widgets)
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
endif()
if (${Qt5Sql_FOUND})
    message(STATUS "Found Qt Sql 5 ${Qt5Sql_VERSION}")
    message(STATUS "Qt 5 Sql Directory: ${Qt5Sql_DIR}")
    message(STATUS "Qt 5 Sql Include directories: ${Qt5Sql_INCLUDE_DIRS}\n")
    set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::Sql)
    include_directories(${Qt5Sql_INCLUDE_DIRS})
endif()

if(WIN32)
    find_package(Qt5WinExtras     REQUIRED)
    if (${Qt5WinExtras_FOUND})
        message(STATUS "Found Qt 5 WinExtras ${Qt5WinExtras_VERSION}")
        message(STATUS "Qt 5 WinExtras Directory: ${Qt5WinExtras_DIR}")
        message(STATUS "Qt 5 WinExtras Include directories: ${Qt5WinExtras_INCLUDE_DIRS}\n")
        set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::WinExtras)
        include_directories(${Qt5WinExtras_INCLUDE_DIRS})
    endif()
endif()

find_package( OpenCV REQUIRED )
if (${OpenCV_FOUND})
    message(STATUS "Found OpenCV ${OpenCV_VERSION}")
    message(STATUS "OpenCV Directory: ${OpenCV_DIR}")
    message(STATUS "OpenCV Include directories: ${OpenCV_INCLUDE_DIRS}\n")
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

#Add simulator module
set(SIMULATOR_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/sim")
add_subdirectory(${SIMULATOR_DIRECTORY})

#Add third-party modules
set(THIRD_PARTY_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third_party")
add_subdirectory(${THIRD_PARTY_DIRECTORY})

#Add user interface
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(RESOURCES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources)
if(WIN32)
	add_definitions(-DNOMINMAX)
    set(WIN_APP_RC_SOURCE "${PROJECT_SOURCE_DIR}/cmake/app.rc.cmake")
    set(WIN_APP_RC_DEST "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
    configure_file("${WIN_APP_RC_SOURCE}" "${WIN_APP_RC_DEST}")
    set(WIN_RESOURCES
        "${WIN_APP_RC_DEST}"
        )
    set(OPTIONS WIN32)
endif()

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE ${RESOURCES_DIRECTORY}/vari.icns)
    set(OSX_RESOURCES ${RESOURCES_DIRECTORY}/vari.icns)
    set_source_files_properties(${OSX_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(OPTIONS MACOSX_BUNDLE)
endif()

set(QT_RESOURCES
    ${RESOURCES_DIRECTORY}/resources.qrc)

set(QT_UI_FORMS
    VWindowMain.ui
    VWindowMaterials.ui
    VWindowLayer.ui
    VWindowPolygon.ui
    )

set(HEADER_FILES
    VWindowMain.h
    VWindowMaterials.h
    VWindowCloth.h
    VWindowResin.h
    VWindowLayer.h
    VWindowPolygon.h
    VDatabaseInteractor.h
    VDatabaseCloth.h
    VDatabaseResin.h
    VSqlDatabase.h
    VScreenShooter.h
    VVideoShooter.h
    )

set(SOURCE_FILES
    main.cpp
    VWindowMain.cpp
    VWindowMaterials.cpp
    VWindowCloth.cpp
    VWindowResin.cpp
    VWindowLayer.cpp
    VWindowPolygon.cpp
    VDatabaseInteractor.cpp
    VDatabaseCloth.cpp
    VDatabaseResin.cpp
    VSqlDatabase.cpp
    VScreenShooter.cpp
    VVideoShooter.cpp
    )

#Create executable
add_executable(${EXE_NAME}
    ${OPTIONS}
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${QT_UI_FORMS}
    ${QT_RESOURCES}
    ${WIN_RESOURCES}
    ${OSX_RESOURCES}
    )

target_link_libraries(${EXE_NAME} ${QT_LIBRARIES})
target_link_libraries(${EXE_NAME} ${OpenCV_LIBS})
add_dependencies(${EXE_NAME} v_sim)
target_link_libraries(${EXE_NAME} v_sim)
add_dependencies(${EXE_NAME} v_third_party)
target_link_libraries(${EXE_NAME} v_third_party)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

if(EXISTS "$ENV{QTDIR}")
    set(QT_DIR "$ENV{QTDIR}")
else()
    set(QT_DIR "${Qt5Core_DIR}/../../..")
endif()
if(EXISTS "$ENV{COINDIR}")
    set(COIN_DIR "$ENV{COINDIR}")
else()
    set(COIN_DIR "${Coin_DIR}/../../..")
endif()

#CONFIGURATING WINDOWS INSTALLER
if(WIN32 AND CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE(win32.vari.install)
endif()

#CONFIGURATING MAC INSTALLER
if(APPLE)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/Applications")
    set(MACOSX_BUNDLE_INFO_STRING          "com.wicron.vari")
    set(MACOSX_BUNDLE_ICON_FILE            "vari.icns")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER       "com.wicron.vari")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING  ${PROJECT_VERSION_LONG})
    set(MACOSX_BUNDLE_BUNDLE_NAME          ${EXE_NAME})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_BUNDLE_VERSION       ${PROJECT_VERSION_LONG})
    set(MACOSX_BUNDLE_COPYRIGHT            ${PROJECT_COPYRIGHT})

    set(APPS "${CMAKE_INSTALL_PREFIX}/${EXE_NAME}.app")
    set(DIRS "${QT_LIBRARY_DIRS}" "${QT_DIR}/lib" "${COIN_DIR}/lib")

    set(QT_PLUGINS_SOURCE_DIR  ${QT_DIR}/plugins)
    set(CONTENT_DIR            ${EXE_NAME}.app/Contents)
    set(QT_PLUGIN_DESTINAT_DIR ${EXE_NAME}.app/Contents/PlugIns)
    set(QT_RESOURCE_DEST_DIR   ${EXE_NAME}.app/Contents/Resources)

    install(TARGETS ${EXE_NAME}
    BUNDLE DESTINATION  .   COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
    )

    if(EXISTS $ENV{SQL_DIR})
      set(SQL_DIR $ENV{SQL_DIR})
    else()
      set(SQL_DIR ${PROJECT_SOURCE_DIR}/../share/vari/sql)
    endif()
    install(FILES ${SQL_DIR}/create.sql DESTINATION ${QT_RESOURCE_DEST_DIR})
    install(FILES ${SQL_DIR}/remove.sql DESTINATION ${QT_RESOURCE_DEST_DIR})

    install(FILES ${QT_PLUGINS_SOURCE_DIR}/platforms/libqcocoa.dylib     DESTINATION ${QT_PLUGIN_DESTINAT_DIR}/platforms)
    install(FILES ${QT_PLUGINS_SOURCE_DIR}/platforms/libqminimal.dylib   DESTINATION ${QT_PLUGIN_DESTINAT_DIR}/platforms)
    install(FILES ${QT_PLUGINS_SOURCE_DIR}/sqldrivers/libqsqlpsql.dylib  DESTINATION ${QT_PLUGIN_DESTINAT_DIR}/sqldrivers)
    install(FILES ${QT_PLUGINS_SOURCE_DIR}/printsupport/libcocoaprintersupport.dylib DESTINATION ${QT_PLUGIN_DESTINAT_DIR}/printsupport)

    install(CODE "
      FILE(WRITE \"\${CMAKE_INSTALL_PREFIX}/${QT_RESOURCE_DEST_DIR}/qt.conf\" \"\")
      " COMPONENT Runtime
    )

    install(CODE "
      FILE(GLOB_RECURSE MODULES \"${CMAKE_INSTALL_PREFIX}/${CONTENT_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
      INCLUDE(BundleUtilities)
      FIXUP_BUNDLE(\"${APPS}\" \"\${MODULES}\" \"${DIRS}\")
      " COMPONENT Runtime
    )
endif()
