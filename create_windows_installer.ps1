$GENERATOR_NAME="Visual Studio 12 2013 Win64"
$BUILD_TYPE="Release"

$COIN_REPO_URL="https://bitbucket.org/Coin3D/coin"
$SOQT_REPO_URL="https://bitbucket.org/Coin3D/soqt"
$GMSH_REPO_URL="https://gitlab.onelab.info/gmsh/gmsh.git"
$COIN_CHANGESET_HASH="11932:acee8063042f"
$SOQT_CHANGESET_HASH="2021:fd7ae3be0e28"
$GMSH_TAG="gmsh_3_0_6"

$env:COINDIR="C:\coin3d"
$env:GMSH_DIR="C:\gmsh"

function check_exit_code([int]$value) {
     if($value -ne 0) {
            throw "Check exit code error"
            exit 1
    }
}

if (!(test-path coin3d)) {mkdir coin3d}
cd coin3d
#BUILD COIN3D
$COIN_REPO_PATH="coin"
if (!(test-path "$COIN_REPO_PATH"))
{
	hg clone $COIN_REPO_URL
	cd "$COIN_REPO_PATH"
	check_exit_code($LASTEXITCODE)
}
else
{
	cd "$COIN_REPO_PATH"
	hg pull
}
hg update default
hg checkout $COIN_CHANGESET_HASH
if (!(test-path coin_build)) {mkdir coin_build}
cd coin_build
$CMAKE_INST_PREFIX_ARG="CMAKE_INSTALL_PREFIX=$env:COINDIR"
$CMAKE_BUILD_ARG="CMAKE_BUILD_TYPE=$BUILD_TYPE"
cmake "-D$CMAKE_INST_PREFIX_ARG" "-D$CMAKE_BUILD_ARG" -G $GENERATOR_NAME ..
check_exit_code($LASTEXITCODE)
$SLN_NAME="Coin.sln"
devenv $SLN_NAME /Build $BUILD_TYPE /Project INSTALL
check_exit_code($LASTEXITCODE)

#BUILD SOQT
cd ../..
$SOQT_REPO_PATH="soqt"
if (!(test-path "$SOQT_REPO_PATH"))
{
	hg clone $SOQT_REPO_URL
	cd "$SOQT_REPO_PATH"
	check_exit_code($LASTEXITCODE)
}
else
{
	cd "$SOQT_REPO_PATH"
	hg pull
}
hg update default
hg checkout $SOQT_CHANGESET_HASH
if (!(test-path soqt_build)) {mkdir soqt_build}
cd soqt_build
$CMAKE_INST_PREFIX_ARG="CMAKE_INSTALL_PREFIX=$env:COINDIR"
$CMAKE_PREFIX_PATH_ARG="CMAKE_PREFIX_PATH=$env:COINDIR;$env:QTDIR"
$CMAKE_BUILD_ARG="CMAKE_BUILD_TYPE=$BUILD_TYPE"
cmake "-D$CMAKE_INST_PREFIX_ARG" "-D$CMAKE_PREFIX_PATH_ARG" "-D$CMAKE_BUILD_ARG" -G $GENERATOR_NAME ..
check_exit_code($LASTEXITCODE)
$SLN_NAME="SoQt.sln"
devenv $SLN_NAME /Build $BUILD_TYPE /Project INSTALL
check_exit_code($LASTEXITCODE)

#BUILD GMSH
cd ../../..
$GMSH_REPO_PATH="gmsh"
if (!(test-path "$GMSH_REPO_PATH"))
{
	git clone $GMSH_REPO_URL
	cd "$GMSH_REPO_PATH"
	check_exit_code($LASTEXITCODE)
}
else
{
	cd "$GMSH_REPO_PATH"
	git pull
}
git checkout $GMSH_TAG
#TODO patch source
if (!(test-path build)) {mkdir build}
cd build
$CMAKE_INST_PREFIX_ARG="CMAKE_INSTALL_PREFIX=$env:GMSH_DIR"
$CMAKE_BUILD_ARG="CMAKE_BUILD_TYPE=$BUILD_TYPE"
cmake "-D$CMAKE_INST_PREFIX_ARG" "-D$CMAKE_BUILD_ARG" -DENABLE_BLAS_LAPACK=OFF -DENABLE_GMP=OFF -DENABLE_BUILD_LIB=ON -G $GENERATOR_NAME ..
check_exit_code($LASTEXITCODE)
$SLN_NAME="gmsh.sln"
devenv $SLN_NAME /Build $BUILD_TYPE /Project INSTALL
check_exit_code($LASTEXITCODE)

#BUILD VARI
cd ../..
if (!(test-path build)) {mkdir build}
cd build
$CMAKE_BUILD_ARG="CMAKE_BUILD_TYPE=$BUILD_TYPE"
$CMAKE_PREFIX_PATH_ARG="CMAKE_PREFIX_PATH=$env:COINDIR;$env:QTDIR;$env:GMSH_DIR"
cmake "-D$CMAKE_BUILD_ARG" "-D$CMAKE_PREFIX_PATH_ARG" -G $GENERATOR_NAME ../src
check_exit_code($LASTEXITCODE)
$SLN_NAME="vari.sln"
devenv $SLN_NAME /Build $BUILD_TYPE /Project ALL_BUILD
check_exit_code($LASTEXITCODE)

#Create an installer
cd Release
if (test-path Output) {rm Output -r -fo}
iscc win32.vari.install.iss
check_exit_code($LASTEXITCODE)
