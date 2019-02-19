#!/usr/bin/env bash
# Copyright (c) 2019 Alexey Barashkov <barasher@yandex.ru>
PREFIX="pkg-vari"

GMSH_REPO_URL="https://gitlab.onelab.info/gmsh/gmsh.git"
GMSH_TAG="gmsh_3_0_6"

THREADS_ARG=-j
WORK_DIR_ARG=-w
INSTALL_DIR_ARG=-i
HELP_ARG=-h

HELP_STRING="A tool for building static Gmsh library for usage with VARI programm.

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


READLINK=$(which greadlink)
if [[ -z "$READLINK" ]]
	READLINK=$(which readlink)
fi

SCRIPT=$(${READLINK} -f "${BASH_SOURCE[0]}")
WORKSPACE=$(dirname "$SCRIPT")

THREADS_NUMBER=1
WORK_DIR_NAME=gmsh
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
	if [[ -z "$WORK_DIR" ]]
        then
                >&2 echo "Error. Build directory name not entered after ${WORK_DIR_ARG} key"
                exit 2
	else
		mkdir -p "${!i}"
        	WORK_DIR=$(${READLINK} -f "${!i}")
        fi
    elif [[ ${!i} == "$INSTALL_DIR_ARG" ]]
    then
        ((i++))
	if [[ -z "$INSTALL_DIR" ]]
        then
                >&2 echo "Error. Build directory name not entered after ${INSTALL_DIR_ARG} key"
                exit 2
	else
		mkdir -p "${!i}"
        	INSTALL_DIR=$(${READLINK} -f "${!i}")
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

# Building Gmsh
GMSH_REPO_PATH=gmsh
if [ ! -d "$GMSH_REPO_PATH" ]; then
	git clone $GMSH_REPO_URL || exit
	cd $GMSH_REPO_PATH || exit
else
	cd "$GMSH_REPO_PATH"
	git pull
fi
git checkout $GMSH_TAG

echo "Starting to build Gmsh. Making build in directory $WORK_DIR. It will be installed to directory $INSTALL_DIR. Using $THREADS_NUMBER threads."
mkdir -p my_build
cd my_build || exit
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_BUILD_LIB=ON -DENABLE_BLAS_LAPACK=OFF -DENABLE_GMP=OFF -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} .. || exit
make -j $THREADS_NUMBER || exit
$FAKEROOT make install -j $THREADS_NUMBER || exit

echo "Successfully built Gmsh"
