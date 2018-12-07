# VARI Project
Software for simulating the VARI process

## BUILD dependencies
sudo apt install make g++ cmake fakeroot qt5-default qt5-qmake mercurial doxygen

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
Building by MSVC (Visual Studio 12 2013 Win64).  
A directory with *devenv*  should be added to *PATH* environment variable.  
  
**Dependencies:**  
- Qt 5.9.5  
- CMake (>= 3.1)  
- Git  
- Doxygen  
- TortoiseHG  
- Inno Setup Compiler  
  
*QTDIR* environment variable should be specified.  
  
For building an **EXE** installer, run script **create_windows_installer.ps1**.  
For using it, you should enable PowerShell script execution first (as administrator):  
```
Set-ExecutionPolicy remotesigned
```  
Usage:  
```
.\create_windows_installer.ps1
```

## Installing on Windows
Before installation of VARI, install [PostgresSQL](https://www.openscg.com/bigsql/postgresql/installers.jsp/) . Make sure that it is added to *PATH*.