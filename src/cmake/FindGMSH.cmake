# - Try to find Gmsh library
# Once done this will define
#
#  GMSH_FOUND - system has GMSH
#  GMSH_INCLUDE_DIRS - the GMSH include directory
#  GMSH_LIBRARIES - Link these to use GMSH
#  GMSH_VERSION - Version of GMSH
#
#  Copyright (c) 2019 Alexey Barashkov <barasher@yandex.ru>
#
#  Based on CMake script originally made by Anna Perduta
#  Copyright (c) 2012 Anna Perduta <aperduta@l5.pk.edu.pl>
#

if (GMSH_LIBRARIES AND GMSH_INCLUDE_DIRS)
    # in cache already
    set(GMSH_FOUND TRUE)
else (GMSH_LIBRARIES AND GMSH_INCLUDE_DIRS)
    if(NOT DEFINED GMSH_DIR)
        set(GMSH_DIR $ENV{GMSH_DIR})
    endif()
    find_path(GMSH_INCLUDE_DIR NAMES gmsh/Gmsh.h
        PATHS "${GMSH_DIR}"
        "${GMSH_DIR}/include"
        "${GMSH_DIR}/usr/include"
        "${GMSH_DIR}/../install/include"
        "${GMSH_INSTALL_DIR}/include"
        NO_DEFAULT_PATH
        )

    find_path(GMSH_INCLUDE_DIR NAMES gmsh/Gmsh.h
        "${CMAKE_FIND_ROOT_PATH}"
        )

    set(GMSH_INCLUDE_DIRS
        ${GMSH_INCLUDE_DIR} CACHE PATH "Path to GMSH headers"
        )

    set(LIB_PATHS "${GMSH_DIR_LIB}"
        "${GMSH_DIR}"
        "${GMSH_DIR}/lib"
        "${GMSH_DIR}/usr/lib"
        "${GMSH_DIR_LIB}/../../install/lib"
        "${GMSH_INSTALL_DIR}/lib")

    find_library(GMSH_LIBRARY_RELEASE NAMES Gmsh gmsh
        PATHS
        ${LIB_PATHS}
        NO_DEFAULT_PATH
        )
    find_library(GMSH_LIBRARY_RELEASE NAMES Gmsh gmsh
        "${CMAKE_FIND_ROOT_PATH}"
        )

    find_library(GMSH_LIBRARY_DEBUG NAMES Gmshd gmshd
        PATHS
        ${LIB_PATHS}
        NO_DEFAULT_PATH
        )
    find_library(GMSH_LIBRARY_DEBUG NAMES Gmshd gmshd
        "${CMAKE_FIND_ROOT_PATH}"
        )

    if(WIN32 OR (GMSH_LIBRARY_RELEASE AND GMSH_LIBRARY_DEBUG))
        if(GMSH_LIBRARY_RELEASE)
            set(GMSH_LIBRARIES ${GMSH_LIBRARIES} "optimized" ${GMSH_LIBRARY_RELEASE})
        endif()
        if(GMSH_LIBRARY_DEBUG)
            set(GMSH_LIBRARIES ${GMSH_LIBRARIES} "debug" ${GMSH_LIBRARY_DEBUG})
        endif()
    else()
        if(GMSH_LIBRARY_RELEASE)
            set(GMSH_LIBRARIES ${GMSH_LIBRARIES} ${GMSH_LIBRARY_RELEASE})
        endif()
        if(GMSH_LIBRARY_DEBUG)
            set(GMSH_LIBRARIES ${GMSH_LIBRARIES} ${GMSH_LIBRARY_DEBUG})
        endif()
    endif()

    if(WIN32 AND GMSH_LIBRARIES)
        set(GMSH_LIBRARIES winmm Ws2_32 ${GMSH_LIBRARIES})
    endif()

    set(GMSH_VERSION_FILE "${GMSH_INCLUDE_DIR}/gmsh/GmshVersion.h")
    if(GMSH_INCLUDE_DIR AND EXISTS "${GMSH_VERSION_FILE}")
        file(STRINGS "${GMSH_VERSION_FILE}" gmsh_version_str
            REGEX "#define GMSH_VERSION[ ]*\"([0-9]+).([0-9]+).([0-9]+)\"")

            if(gmsh_version_str)
                string(REGEX REPLACE "#define GMSH_VERSION[ ]*" "" GMSH_VERSION ${gmsh_version_str})
                    string(REPLACE "\"" "" GMSH_VERSION ${GMSH_VERSION})
                else()
                    set(GMSH_VERSION 0.0.0)
                endif()
            else()
                set(GMSH_VERSION 0.0.0)
            endif()

            include(FindPackageHandleStandardArgs)
            # handle the QUIETLY and REQUIRED arguments and set GMSH_FOUND to TRUE
            # if all listed variables are TRUE
            find_package_handle_standard_args(GMSH DEFAULT_MSG
                GMSH_LIBRARIES GMSH_INCLUDE_DIRS GMSH_VERSION)

            if (GMSH_FIND_VERSION)
                if(NOT ${GMSH_FIND_VERSION} VERSION_EQUAL ${GMSH_VERSION})
                    set(GMSH_VERSION_EXACT False)
                    set(GMSH_FOUND False)
                    message(FATAL_ERROR "Incompable Gmsh version: ${GMSH_VERSION}. Required is ${GMSH_FIND_VERSION}." )
                endif()
            endif()

            mark_as_advanced(GMSH_INCLUDE_DIRS GMSH_LIBRARIES GMSH_VERSION)
        endif (GMSH_LIBRARIES AND GMSH_INCLUDE_DIRS)
