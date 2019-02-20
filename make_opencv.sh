#!/usr/bin/env bash
# Copyright (c) 2019 Alexey Barashkov <barasher@yandex.ru>
OPENCV_REPO_URL="https://github.com/opencv/opencv.git"
OPENCV_TAG="3.2.0"

THREADS_ARG=-j
WORK_DIR_ARG=-w
INSTALL_DIR_ARG=-i
OSX_SYSROOT_ARG=-x
HELP_ARG=-h

HELP_STRING="A tool for building static OpenCV library for usage with VARI programm.

Usage:
"${BASH_SOURCE[0]}" [$THREADS_ARG THREADS_NUMBER] [$WORK_DIR_ARG WORK_DIRECTORY] [$INSTALL_DIR_ARG INSTALL_DIRECTORY] [$OSX_SYSROOT_ARG OSX_SYSROOT]

Arguments:
$THREADS_ARG THREADS_NUMBER
THREADS_NUMBER - how many threads should be used by make process
$WORK_DIR_ARG WORK_DIRECTORY
WORK_DIRECTORY - specify directory, where binary files are being built
$INSTALL_DIR_ARG INSTALLATION_DIRECTORY
INSTALL_DIRECTORY - specify directory, where binary files will be installed
$OSX_SYSROOT_ARG OSX_SYSROOT
OSX_SYSROOT - macOS SDK path
$HELP_ARG - print help text"

READLINK=$(which greadlink)
if [[ -z "$READLINK" ]]
then
	READLINK=$(which readlink)
fi

SCRIPT=$(${READLINK} -f "${BASH_SOURCE[0]}")
WORKSPACE=$(dirname "$SCRIPT")

THREADS_NUMBER=1
WORK_DIR_NAME=opencv
INSTALL_DIR_NAME=installed

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
    elif [[ ${!i} == "$WORK_DIR_ARG" ]]
    then
        ((i++))
	if [[ -z "${!i}" ]]
        then
                >&2 echo "Error. Work directory name not entered after ${WORK_DIR_ARG} key"
                exit 2
	else
		mkdir -p "${!i}"
        	WORK_DIR=$(${READLINK} -f "${!i}")
        fi
    elif [[ ${!i} == "$INSTALL_DIR_ARG" ]]
    then
        ((i++))
	if [[ -z "${!i}" ]]
        then
                >&2 echo "Error. Build directory name not entered after ${INSTALL_DIR_ARG} key"
                exit 2
	else
		mkdir -p "${!i}"
        	INSTALL_DIR=$(${READLINK} -f "${!i}")
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

if [[ -z "$WORK_DIR" ]]
then
    WORK_DIR="$WORKSPACE/$WORK_DIR_NAME"
fi

if [[ -z "$INSTALL_DIR" ]]
then
    INSTALL_DIR="$WORKSPACE/$INSTALL_DIR_NAME"
fi

mkdir -p "$WORK_DIR" || exit
cd "$WORK_DIR" || exit

FAKEROOT=$(which fakeroot)

# Building OpenCV
OPENCV_REPO_PATH=opencv
if [ ! -d "$OPENCV_REPO_PATH" ]; then
	git clone $OPENCV_REPO_URL || exit
	cd $OPENCV_REPO_PATH || exit
else
	cd "$OPENCV_REPO_PATH"
	git pull
fi
git checkout $OPENCV_TAG

echo "Starting to build OpenCV. Making build in directory $WORK_DIR. It will be installed to directory $INSTALL_DIR. Using $THREADS_NUMBER threads."
mkdir -p my_build
cd my_build || exit
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_DOCS=OFF -DWITH_FFMPEG=OFF -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_OSX_SYSROOT="$OSX_SYSROOT" .. || exit
make -j $THREADS_NUMBER || exit
$FAKEROOT make install -j $THREADS_NUMBER || exit

echo "Successfully built OpenCV"
