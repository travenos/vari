# Project VARI
# @author Alexey Barashkov

cmake_minimum_required(VERSION 3.1.0)

project(vari)
set(PROJECT_VENDOR_LONG "Wicron")
set(ORG_WEBSITE "https://wicron.com")
set(APP_NAME "VARI")

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
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION_LONG}")
message(STATUS "Project version: ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}\n")

#Get information about the compilation time
string(TIMESTAMP CURRENT_YEAR "%Y")
string(TIMESTAMP CURRENT_TIME "%d.%m.%Y %H:%M:%S")

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
endif()
if (${Qt5Gui_FOUND})
    message(STATUS "Found Qt 5 Gui ${Qt5Gui_VERSION}")
    message(STATUS "Qt 5 Gui Directory: ${Qt5Gui_DIR}")
    message(STATUS "Qt 5 Gui Include directories: ${Qt5Gui_INCLUDE_DIRS}\n")
endif()
if (${Qt5Widgets_FOUND})
    message(STATUS "Found Qt 5 Widgets ${Qt5Widgets_VERSION}")
    message(STATUS "Qt 5 Widgets Directory: ${Qt5Widgets_DIR}")
    message(STATUS "Qt 5 Widgets Include directories: ${Qt5Widgets_INCLUDE_DIRS}\n")
endif()
if (${Qt5Sql_FOUND})
    message(STATUS "Found Qt Sql 5 ${Qt5Sql_VERSION}")
    message(STATUS "Qt 5 Sql Directory: ${Qt5Sql_DIR}\n")
    message(STATUS "Qt 5 Sql Include directories: ${Qt5Sql_INCLUDE_DIRS}\n")
endif()

find_package( OpenCV REQUIRED )
if (${OpenCV_FOUND})
    message(STATUS "Found OpenCV ${OpenCV_VERSION}")
    message(STATUS "OpenCV Directory: ${OpenCV_DIR}")
    message(STATUS "OpenCV Include directories: ${OpenCV_INCLUDE_DIRS}\n")
endif()

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

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
if(MSVC)
    set(WIN_APP_RC_SOURCE "${PROJECT_SOURCE_DIR}/cmake/app.rc.cmake")
    set(WIN_APP_RC_DEST "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
    configure_file("${WIN_APP_RC_SOURCE}" "${WIN_APP_RC_DEST}")
    set(WIN_RESOURCES
        "${WIN_APP_RC_DEST}"
        )
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
    WIN32
    "main.cpp"
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${QT_UI_FORMS}
    ${QT_RESOURCES}
    ${WIN_RESOURCES}
    )

target_link_libraries(${EXE_NAME} Qt5::Core)
target_link_libraries(${EXE_NAME} Qt5::Gui)
target_link_libraries(${EXE_NAME} Qt5::Widgets)
target_link_libraries(${EXE_NAME} Qt5::Sql)
target_link_libraries(${EXE_NAME} ${OpenCV_LIBS})
add_dependencies(${EXE_NAME} v_sim)
target_link_libraries(${EXE_NAME} v_sim)
add_dependencies(${EXE_NAME} v_third_party)
target_link_libraries(${EXE_NAME} v_third_party)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

#CONFIGURATING WINDOWS INSTALLER
if(WIN32 AND CMAKE_BUILD_TYPE MATCHES Release)
    if(NOT EXISTS $ENV{QTDIR})
        message(WARNING "QTDIR environment variable is not set correctly. The installer script will not be generated.")
    endif()
    if(NOT EXISTS $ENV{COINDIR})
        message(WARNING "COINDIR environment variable  is not set correctly. The installer script will not be generated.")
    endif()
    if (EXISTS $ENV{QTDIR} AND EXISTS $ENV{COINDIR})
        INCLUDE(win32.vari.install)
    endif()
endif()
