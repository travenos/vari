#!/usr/bin/env bash
# Copyright (c) 2018 Alexey Barashkov <barasher@yandex.ru>

CODE_SOURCE_DIR="src"

SQL_SOURCE_DIR="share/vari/sql"

THREADS_ARG=-j
BUILD_DIR_ARG=-d
OSX_SYSROOT_ARG=-x
HELP_ARG=-h

HELP_STRING="Script for building macOS installer.

Usage:
"${BASH_SOURCE[0]}" [$THREADS_ARG THREADS_NUMBER] [$BUILD_DIR_ARG BUILD_DIRECTORY] [$OSX_SYSROOT_ARG OSX_SYSROOT]

Arguments:
$THREADS_ARG THREADS_NUMBER
THREADS_NUMBER - how many threads should be used by make process
$BUILD_DIR_ARG BUILD_DIRECTORY
BUILD_DIRECTORY- specify directory, where binary files are being built
$OSX_SYSROOT_ARG OSX_SYSROOT
OSX_SYSROOT - macOS SDK path
$HELP_ARG - print help text"

SCRIPT=$(greadlink -f "${BASH_SOURCE[0]}")
WORKSPACE=$(dirname "$SCRIPT")

THREADS_NUMBER=4
BUILD_DIR_NAME=build

# Checking arguments
for (( i=1; i<=$#; i++ ))
do
    if [[ ${!i} == "$THREADS_ARG" ]]
    then
	((i++))
        THREADS_NUMBER="${!i}"   
        if [[ -z "${THREADS_NUMBER##*[!0-9]*}" || $THREADS_NUMBER = 0 ]]
        then
            >&2 echo "Error. Invalid number of threads ${!i}"
            exit 2
        fi
    elif [[ ${!i} == "$BUILD_DIR_ARG" ]]
    then
        ((i++))
	if [[ -z "${!i}" ]]
        then
                >&2 echo "Error. Build directory name not entered after ${BUILD_DIR_ARG} key"
                exit 2
	else
		mkdir -p "${!i}"
        	BUILD_DIR=$(greadlink -f "${!i}")
        fi
    elif [[ ${!i} == "$OSX_SYSROOT_ARG" ]]
    then
        ((i++))
	if [[ -z "${!i}" ]]
        then
                >&2 echo "Error. macOS SDK directory name not entered after ${OSX_SYSROOT_ARG} key"
                exit 2
	else
        	OSX_SYSROOT="${!i}"
        fi
    elif [[ ${!i} == "$HELP_ARG" ]]
    then
        echo -e "$HELP_STRING"
        exit 0
    else
        echo "Error. Invalid argument ${!i}"
        echo -e "$HELP_STRING"
        exit 2
    fi
done

if [[ -z "$BUILD_DIR" ]]
then
    BUILD_DIR="$WORKSPACE/$BUILD_DIR_NAME"
fi

# Build Gmsh
mkdir -p gmsh/installed
export GMSH_DIR=$(greadlink -f gmsh/installed)
if [[ -z "$OSX_SYSROOT" ]]
then
  ./make_gmsh.sh -w gmsh -i "$GMSH_DIR" -j $THREADS_NUMBER || exit
else
  ./make_gmsh.sh -w gmsh -i "$GMSH_DIR" -j $THREADS_NUMBER -x "$OSX_SYSROOT" || exit
fi

# Build Coin3D
mkdir -p coin3d/installed
export COINDIR=$(greadlink -f coin3d/installed)
if [[ -z "$OSX_SYSROOT" ]]
then
  ./make_coin_soqt.sh -w coin3d -i "$COINDIR" -j $THREADS_NUMBER || exit
else
  ./make_coin_soqt.sh -w coin3d -i "$COINDIR" -j $THREADS_NUMBER -x "$OSX_SYSROOT"  || exit
fi

# Build OpenCV
mkdir -p opencv/installed
export OPENCV_DIR=$(greadlink -f opencv/installed)
if [[ -z "$OSX_SYSROOT" ]]
then
  ./make_opencv.sh -w opencv -i "$OPENCV_DIR" -j $THREADS_NUMBER || exit
else
  ./make_opencv.sh -w opencv -i "$OPENCV_DIR" -j $THREADS_NUMBER -x "$OSX_SYSROOT"  || exit
fi

# Add built libraries to default paths
export PATH=${PATH}:"${INSTALL_DIR}/usr"
export CMAKE_PREFIX_PATH="${COINDIR}":"${GMSH_DIR}":"${OPENCV_DIR}":${CMAKE_PREFIX_PATH}
export LD_LIBRARY_PATH="${COINDIR}/lib":"${GMSH_DIR}/lib":"${OPENCV_DIR}/lib":${LD_LIBRARY_PATH}
export CPLUS_INCLUDE_PATH="${COINDIR}/include":"${GMSH_DIR}/include":"${OPENCV_DIR}/include":${CPLUS_INCLUDE_PATH}

# Building VARI and creating a package
echo "Starting to build package VARI. Version: $VERSION. Making build in directory $BUILD_DIR. Using $THREADS_NUMBER threads."

cd "$WORKSPACE"
mkdir -p "$BUILD_DIR" || exit

export SQL_DIR="$BUILD_DIR/sql"
mkdir -p "$SQL_DIR"
for filename in ${SQL_SOURCE_DIR}/*
do
	cp "$filename" "$SQL_DIR"
done

cd "$BUILD_DIR" || exit

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_SYSROOT="$OSX_SYSROOT" "$WORKSPACE/$CODE_SOURCE_DIR" || exit
make -j $THREADS_NUMBER || exit
make install -j $THREADS_NUMBER || exit

