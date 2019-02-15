# VARI Project
Software for simulating the VARI process

## Ubuntu build dependencies
```
sudo apt install make g++ cmake fakeroot qt5-default qt5-qmake mercurial doxygen libc-bin libopencv-dev  
```
For Ubuntu 18.04 or higher:  
```
sudo apt install libopencv-videoio-dev  
```
For Ubuntu with lower version:  
```
sudo apt install libopencv-highgui-dev  
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
- **\-h** \- print help text.  
  

## Building for Windows
Building with MSVC **(Visual Studio 12 2013 Win64)**.  
A directory containing *devenv* should be added to a *PATH* environment variable.  
  
**Dependencies:**  
- [Qt 5.9.5](https://download.qt.io/official_releases/qt/5.9/5.9.5/)  
- [CMake (>= 3.1)](https://cmake.org/download/)  
- [Git](https://git-scm.com/download/win)  
- [Doxygen](http://doxygen.nl/files/doxygen-1.8.15-setup.exe)  
- [TortoiseHG](https://tortoisehg.bitbucket.io/download/index.html)  
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

## Building from scratch:
1. Install dependencies for your platform.  
2. Set environment variable *QTDIR* to directory with Qt files.  
3. Get Coin3D sources using Mercurial:  
    ```
    hg clone https://bitbucket.org/Coin3D/coin
    ```  
4. Build and install it. Set environment variable *COINDIR* pointing to the installation path.  
5. Get SoQt sources using Mercurial:  
    ```
    hg clone https://bitbucket.org/Coin3D/soqt
    ```
6. Build and install it to *COINDIR*. When building, variables *COINDIR* and *QTDIR* should be added to CMAKE\_PREFIX\_PATH.  
7. Get GMSH sources using Git:  
    ```
    git clone https://gitlab.onelab.info/gmsh/gmsh.git
    ```  
    Checkout to tag *gmsh_3_0_6*.  
8. Build and install it. Use flags: *-DENABLE_BUILD_LIB=ON -DENABLE_BLAS_LAPACK=OFF -DENABLE_GMP=OFF* . Set environment variable *GMSH_DIR* pointing to the installation path.  
9. Get OpenCV sources using Git. On Linux you can use OpenCV installed using your package manager (dev packages should be installed). In this case you can skip steps 9 and 10.  
    ```
    git clone https://github.com/opencv/opencv.git
    ```  
    Checkout to tag *3.2.0*.  
10. Build and install it. Set environment variable *OPENCV_DIR* pointing to the installation path.  
11. Build VARI. Main *CMakeLists.txt* file is placed in *src* directory.  

## Installing on Windows
Before installation of VARI, install [PostgresSQL](https://www.openscg.com/bigsql/postgresql/installers.jsp/) . Make sure that it is added to *PATH*.

