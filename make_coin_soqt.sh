#!/usr/bin/env bash
# Copyright (c) 2018 Alexey Barashkov <travenos@robot-develop.org>
PREFIX="pkg-vari"

COIN_REPO_URL="https://bitbucket.org/Coin3D/coin"
SOQT_REPO_URL="https://bitbucket.org/Coin3D/soqt"
COIN_CHANGESET_HASH="11927:e74da184f75b"
SOQT_CHANGESET_HASH="2011:ea5cd7646087"

THREADS_ARG=-j
WORK_DIR_ARG=-w
INSTALL_DIR_ARG=-i
HELP_ARG=-h

HELP_STRING="Tool for building Coin3D and SoQt.

Usage:
"${BASH_SOURCE[0]}" [$THREADS_ARG THREADS_NUMBER] [$WORK_DIR_ARG WORK_DIRECTORY] [$INSTALL_DIR_ARG INSTALL_DIRECTORY]

Arguments:
$THREADS_ARG THREADS_NUMBER
THREADS_NUMBER - how many threads should be used by make process
$WORK_DIR_ARG WORK_DIRECTORY
WORK_DIRECTORY- specify directory, where binary files are being built
$INSTALL_DIR_ARG INSTALLATION_DIRECTORY
INSTALL_DIRECTORY- specify directory, where binary files will be installed
$HELP_ARG - print help text"

SCRIPT=$(readlink -f "${BASH_SOURCE[0]}")
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
        WORK_DIR=$(readlink -f "${!i}")
        if [[ -z "$WORK_DIR" ]]
        then
                >&2 echo "Error. Working directory name not specified after ${WORK_DIR_ARG} key"
                exit 2
        fi
    elif [[ ${!i} == "$INSTALL_DIR_ARG" ]]
    then
        ((i++))
        INSTALL_DIR=$(readlink -f "${!i}")
        if [[ -z "$INSTALL_DIR" ]]
        then
                >&2 echo "Error. Install directory name not entered after ${INSTALL_DIR_ARG} key"
                exit 2
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
INSTALL_DIR="$INSTALL_DIR/usr"

mkdir -p "$WORK_DIR" || exit
cd "$WORK_DIR" || exit

# Building Coin3D
COIN_REPO_PATH=coin
if [ ! -d "$COIN_REPO_PATH" ]; then
	hg clone $COIN_REPO_URL || exit
	cd $COIN_REPO_PATH || exit
	hg update default
else
	cd "$COIN_REPO_PATH"
	hg pull
	hg merge
fi
hg checkout $COIN_CHANGESET_HASH

echo "Starting to build Coin3D. Making build in directory $WORK_DIR. It will be installed to directory $INSTALL_DIR. Using $THREADS_NUMBER threads."
mkdir -p my_build
cd my_build || exit
cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} .. || exit
make -j $THREADS_NUMBER || exit
fakeroot make install -j $THREADS_NUMBER || exit

# Building SoQt
cd "$WORK_DIR" || exit
export PATH=${PATH}:${INSTALL_DIR}
export CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:${INSTALL_DIR}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:"${INSTALL_DIR}/lib"
SOQT_REPO_PATH=soqt
if [ ! -d "$SOQT_REPO_PATH" ]; then
	hg clone $SOQT_REPO_URL || exit
	cd $SOQT_REPO_PATH || exit
	hg update default
else
	cd "$SOQT_REPO_PATH"
	hg pull
	hg merge
fi
hg checkout $SOQT_CHANGESET_HASH

echo "Starting to build SoQt. Making build in directory $WORK_DIR. It will be installed to directory $INSTALL_DIR. Using $THREADS_NUMBER threads."
mkdir -p my_build
cd my_build || exit
cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} .. || exit
make -j $THREADS_NUMBER || exit
fakeroot make install -j $THREADS_NUMBER || exit

echo "Successfully built Coin3D and SoQt"
