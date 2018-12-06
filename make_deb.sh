#!/usr/bin/env bash
# Copyright (c) 2018 Alexey Barashkov <travenos@robot-develop.org>
PACKAGE_NAME="vari"
DEPENDENCIES="libc6 (>= 2.27), libgcc1 (>= 1:8), libgl1-mesa-glx | libgl1, libstdc++6 (>= 8), libx11-6, libxi6, qt5-default (>= 5.9), postgresql, libqt5sql5-psql"
CONFLICTS="libsoqt3-20, libsoqt4-20, libsoqt4-dev, libcoin80v5, libcoin80-dev, libcoin80-runtime"
PROVIDES="libcoin80v5, libcoin80-dev"

CODE_SOURCE_DIR="src"

PREFIX="pkg-vari"

BIN_DEST_DIR="$PREFIX/usr/bin"
BIN_FILE="vari"

SHARE_SOURCE_DIR="share"
SHARE_DEST_DIR="$PREFIX/usr/share"

PACKAGE_SOURCE_DIR="package"
PACKAGE_DEST_DIR="$PREFIX/DEBIAN"

CONTROL_NAME="control"

VERSION_ARG=-v
THREADS_ARG=-j
BUILD_DIR_ARG=-d
HELP_ARG=-h

HELP_STRING="Tool for making DEB package from source.

Usage:
"${BASH_SOURCE[0]}" [$VERSION_ARG VERSION] [$THREADS_ARG THREADS_NUMBER] [$BUILD_DIR_ARG BUILD_DIRECTORY]

Arguments:
$VERSION_ARG VERSION
VERSION - specified verion of the package
$THREADS_ARG THREADS_NUMBER
THREADS_NUMBER - how many threads should be used by make process
$BUILD_DIR_ARG BUILD_DIRECTORY
BUILD_DIRECTORY- specify directory, where binary files are being built
$HELP_ARG - print help text"

SCRIPT=$(readlink -f "${BASH_SOURCE[0]}")
WORKSPACE=$(dirname "$SCRIPT")

THREADS_NUMBER=1
BUILD_DIR_NAME=build

# Checking arguments
for (( i=1; i<=$#; i++ ))
do
    if [[ ${!i} == "$VERSION_ARG" ]]
    then
        ((i++))
        VERSION="${!i}"
    elif [[ ${!i} == "$THREADS_ARG" ]]
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
        BUILD_DIR=$(readlink -f "${!i}")
        if [[ -z "$BUILD_DIR" ]]
        then
                >&2 echo "Error. Build directory name not entered after ${BUILD_DIR_ARG} key"
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

if [[ -z "$BUILD_DIR" ]]
then
    BUILD_DIR="$WORKSPACE/$BUILD_DIR_NAME"
fi

#Setting version
if [[ -z "$VERSION" ]]
then
     VERSION=$(git describe --tags 2> /dev/null) || VERSION=1.0
     if [[ ${VERSION:0:1} == "v" ]]
     then
        VERSION=${VERSION:1}
     fi
fi

SYSTEM_ARCHITECTURE=$(dpkg --print-architecture)

INSTALL_DIR="$WORKSPACE/$PREFIX"
#Remove deb build directory if it existed
rm -rf "$INSTALL_DIR" || exit

./make_coin_soqt.sh -w coin3d -i "$INSTALL_DIR" -j $THREADS_NUMBER || exit
export PATH=${PATH}:"${INSTALL_DIR}/usr"
export CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:"${INSTALL_DIR}/usr"
export LD_LIBRARY_PATH="${INSTALL_DIR}/usr/lib":${LD_LIBRARY_PATH}
export CPLUS_INCLUDE_PATH=${CPLUS_INCLUDE_PATH}:"${INSTALL_DIR}/usr/include"

echo "Starting to build package $PACKAGE_NAME. Architecture: $SYSTEM_ARCHITECTURE. Version: $VERSION. Making build in directory $BUILD_DIR. Using $THREADS_NUMBER threads."

mkdir -p "$BUILD_DIR" || exit
cd "$BUILD_DIR" || exit

cmake -DCMAKE_BUILD_TYPE=Release "$WORKSPACE/$CODE_SOURCE_DIR" || exit
make -j $THREADS_NUMBER || exit

cd "$WORKSPACE" || exit

mkdir -p "$BIN_DEST_DIR"
cp -ar "$BUILD_DIR/$BIN_FILE" "$BIN_DEST_DIR" || exit
mkdir -p "$SHARE_DEST_DIR"
cp -ar "$SHARE_SOURCE_DIR"/* "$SHARE_DEST_DIR" || exit
mkdir -p "$PACKAGE_DEST_DIR"
cp -ar "$PACKAGE_SOURCE_DIR"/* "$PACKAGE_DEST_DIR" || exit

#Set package parametres
sed -i "s/@version@/${VERSION}/g" "$PACKAGE_DEST_DIR/$CONTROL_NAME"
sed -i "s/@package@/${PACKAGE_NAME}/g" "$PACKAGE_DEST_DIR/$CONTROL_NAME"
sed -i "s/@architecture@/${SYSTEM_ARCHITECTURE}/g" "$PACKAGE_DEST_DIR/$CONTROL_NAME"
sed -i "s/@dependencies@/${DEPENDENCIES}/g" "$PACKAGE_DEST_DIR/$CONTROL_NAME"
sed -i "s/@conflicts@/${CONFLICTS}/g" "$PACKAGE_DEST_DIR/$CONTROL_NAME"
sed -i "s/@provides@/${PROVIDES}/g" "$PACKAGE_DEST_DIR/$CONTROL_NAME"

#Set size of installed package
size=$(du -h -s -k "$PREFIX")
size=${size//[^0-9]/}
sed -i "s/@size@/${size}/g" "$PACKAGE_DEST_DIR/$CONTROL_NAME"

#Remove old deb files
find . -type f -name "${PACKAGE_NAME}*.deb" -delete

#Creating new deb
fakeroot dpkg-deb --build "./$PREFIX" || exit
deb_name="${PACKAGE_NAME}_${VERSION}_${SYSTEM_ARCHITECTURE}.deb"
mv "${PREFIX}.deb" "$deb_name" || exit
rm -rf "$PREFIX"
echo "Successfully created package $deb_name"