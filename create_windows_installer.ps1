$GENERATOR_NAME="Visual Studio 12 2013 Win64"
$BUILD_TYPE="Release"

$COIN_REPO_URL="https://bitbucket.org/Coin3D/coin"
$SOQT_REPO_URL="https://bitbucket.org/Coin3D/soqt"
$COIN_CHANGESET_HASH="11927:e74da184f75b"
$SOQT_CHANGESET_HASH="2011:ea5cd7646087"

$env:COINDIR="C:\coin3d"

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
	hg update default
}
else
{
	cd "$COIN_REPO_PATH"
	hg pull
	hg merge
}
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
	hg update default
}
else
{
	cd "$SOQT_REPO_PATH"
	hg pull
	hg merge
}
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

#BUILD VARI
cd ../../..
if (!(test-path build)) {mkdir build}
cd build
$CMAKE_BUILD_ARG="CMAKE_BUILD_TYPE=$BUILD_TYPE"
$CMAKE_PREFIX_PATH_ARG="CMAKE_PREFIX_PATH=$env:COINDIR;$env:QTDIR"
cmake "-D$CMAKE_BUILD_ARG" "-D$CMAKE_PREFIX_PATH_ARG" -G $GENERATOR_NAME ../src
check_exit_code($LASTEXITCODE)
$SLN_NAME="vari.sln"
devenv $SLN_NAME /Build $BUILD_TYPE /Project ALL_BUILD
check_exit_code($LASTEXITCODE)

#Create an installer
cd Release
iscc win32.vari.install.iss
check_exit_code($LASTEXITCODE)
