# VARI Project
Software for simulating the VARI process  
![Screenshot](screen.png)  

## Database location
Database is created on first launch of the application. Two tables are created from file *create.sql*. In the repository, it is located in the following path:  
**share/vari/sql/create.sql**  

## Ubuntu build dependencies
```
sudo apt install make g++ cmake fakeroot qt5-default qt5-qmake doxygen libc-bin  
```
For Ubuntu 18.04 or higher, for building with OpenCV from package manager (with *\-\-system_opencv* flag):  
```
sudo apt install libopencv-dev libopencv-videoio-dev libopencv-imgproc-dev  
```

## Building a DEB package
For building a **DEB** package, run script **make_deb.sh**.  
Usage:  
```bash
./make_deb.sh
```
Possible arguments:  
- **\-j THREADS** \- how many threads should be used by make process.  
- **\-d BUILD_DIRECTORY** \- specify directory, where binary files are being built.  
- **\-v VERSION**  \- specify version of the package.  
- **\-\-system_opencv**  \- do not build OpenCV, enable dynamic linking with OpenCV installed to system using package manager  
- **\-h** \- print help text.  
  

## Building for Windows
Building with MSVC **(Visual Studio 12 2013 Win64)**.  
A directory containing *devenv* should be added to a *PATH* environment variable.  
  
**Dependencies:**  
- [Qt 5.9.5](https://download.qt.io/official_releases/qt/5.9/5.9.5/)  
- [CMake (>= 3.1)](https://cmake.org/download/)  
- [Git](https://git-scm.com/download/win)  
- [Doxygen](http://doxygen.nl/files/doxygen-1.8.15-setup.exe)  
- [Sed](http://sourceforge.net/projects/gnuwin32/files//sed/4.2.1/sed-4.2.1-setup.exe/download)  
- [Inno Setup Compiler](http://www.jrsoftware.org/isdl.php)  
  
*QTDIR* environment variable should be specified.  
For all dependencies, all directories, which binary files (exe and dll) should be added to *PATH*.  
  
For building an **EXE** installer, run script **create_windows_installer.ps1**.  
For using it, you should enable PowerShell script execution first (as administrator):  
```
Set-ExecutionPolicy remotesigned
```  
Usage:  
```
.\create_windows_installer.ps1
```

## Building for macOS
Dependencies:  
- Xcode Command Line Tools  
- Qt 5.9.5  
- cmake  
- git  
- coreutils  
- doxygen  

Some dependencies can be installed with [Homebrew](https://brew.sh/):  
```
brew install coreutils cmake doxygen  
```
  
For building a macOS app, run script **make_mac.sh**.  
Usage:  
```bash
./make_mac.sh
```
Possible arguments:  
- **\-j THREADS** \- how many threads should be used by make process.  
- **\-d BUILD_DIRECTORY** \- specify directory, where binary files are being built.  
- **\-x OSX_SYSROOT** \- specify macOS SDK path.  
- **\-h** \- print help text.  
App will be placed to directory Applications in build directory.  

## Building from scratch:
1. Install dependencies for your platform.  
2. Set environment variable *QTDIR* to directory with Qt files.  
3. Get Coin3D sources using Git:  
    ```
    git clone https://github.com/coin3d/coin.git
    ```  
4. Build and install it. Set environment variable *COINDIR* pointing to the installation path.  
5. Get SoQt sources using Git:  
    ```
    git clone https://github.com/coin3d/soqt.git
    ```
6. Build and install it to *COINDIR*. When building, variables *COINDIR* and *QTDIR* should be added to CMAKE\_PREFIX\_PATH.  
7. Get GMSH sources using Git:  
    ```
    git clone https://github.com/sasobadovinac/gmsh.git
    ```  
    Checkout to tag *gmsh_3_0_6*.  
8. Build and install it. Use flags: *-DENABLE_BUILD_LIB=ON -DENABLE_BLAS_LAPACK=OFF -DENABLE_GMP=OFF* . Set environment variable *GMSH_DIR* pointing to the installation path.  
9. Get OpenCV sources using Git. On Linux you can use OpenCV 3.2.0 installed using your package manager (dev packages should be installed). In this case you can skip steps 9 and 10.  
    ```
    git clone https://github.com/opencv/opencv.git
    ```  
    Checkout to tag *3.4.0*.  
10. Build and install it. Better use static linking, but it is necessary to use dynamic CRT when building with MSVC. Use flags *-DBUILD_SHARED_LIBS=OFF -DBUILD_WITH_STATIC_CRT=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_DOCS=OFF -DWITH_FFMPEG=OFF -DWITH_OPENCLAMDBLAS=OFF -DWITH_OPENCLAMDFFT=OFF -DWITH_CUDA=OFF -DWITH_GPHOTO2=OFF -DWITH_VTK=OFF*.
Set environment variable *OPENCV_DIR* pointing to the installation path.  
11. Build VARI. Main *CMakeLists.txt* file is placed in *src* directory.  


