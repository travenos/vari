#!/usr/bin/env bash
# Copyright (c) 2018 Alexey Barashkov <barasher@yandex.ru>

COIN_REPO_URL="https://github.com/coin3d/coin.git"
SOQT_REPO_URL="https://github.com/coin3d/soqt.git"
COIN_CHANGESET_HASH="SoQt-1.6.0"
SOQT_CHANGESET_HASH="Coin-4.0.0"

THREADS_ARG=-j
WORK_DIR_ARG=-w
INSTALL_DIR_ARG=-i
OSX_SYSROOT_ARG=-x
HELP_ARG=-h

HELP_STRING="Tool for building Coin3D and SoQt.

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
WORK_DIR_NAME=coin3d
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

FAKEROOT=$(which fakeroot)

mkdir -p "$WORK_DIR" || exit
cd "$WORK_DIR" || exit

# Building Coin3D
COIN_REPO_PATH=coin
if [ ! -d "$COIN_REPO_PATH" ]; then
	git clone $COIN_REPO_URL || exit
	cd $COIN_REPO_PATH || exit
else
	cd "$COIN_REPO_PATH"
	git pull
fi
#git update default
git checkout $COIN_CHANGESET_HASH
git submodule update --init --recursive

echo "Starting to build Coin3D. Making build in directory $WORK_DIR. It will be installed to directory $INSTALL_DIR. Using $THREADS_NUMBER threads."
mkdir -p my_build
cd my_build || exit
cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_OSX_SYSROOT="$OSX_SYSROOT" .. || exit
make -j $THREADS_NUMBER || exit
$FAKEROOT make install -j $THREADS_NUMBER || exit

# Building SoQt
cd "$WORK_DIR" || exit
export PATH=${PATH}:${INSTALL_DIR}
export CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:${INSTALL_DIR}:${QTDIR}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:"${QTDIR}/lib":"${INSTALL_DIR}/lib"
export CPLUS_INCLUDE_PATH=${CPLUS_INCLUDE_PATH}:"${QTDIR}/include"
SOQT_REPO_PATH=soqt
if [ ! -d "$SOQT_REPO_PATH" ]; then
	git clone $SOQT_REPO_URL || exit
	cd $SOQT_REPO_PATH || exit
else
	cd "$SOQT_REPO_PATH"
	git pull
fi
#git update default
git checkout $SOQT_CHANGESET_HASH
git submodule update --init --recursive

echo "Starting to build SoQt. Making build in directory $WORK_DIR. It will be installed to directory $INSTALL_DIR. Using $THREADS_NUMBER threads."
mkdir -p my_build
cd my_build || exit
cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_OSX_SYSROOT="$OSX_SYSROOT" .. || exit
make -j $THREADS_NUMBER || exit
$FAKEROOT make install -j $THREADS_NUMBER || exit

echo "Successfully built Coin3D and SoQt"
