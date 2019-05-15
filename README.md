# lslpub_ESP
[Documentation](https://aightech.github.io/lslpub_ESP/html/index.html)

## 1 Presentation
C++ programs that gets the data from ESP device and publishes them in a LSL stream.

## 2 Installation
### 2.1 Ubuntu 18
#### 2.1.1 **Requirements**
None.
#### 2.1.2 Steps
- Clone the repository and go inside.
```bash
git clone https://github.com/Aightech/lslpub_ESP.git && cd lslpub_ESP
```
- Create a build directory and go inside.
- Configure the project.
- Build the project.
```bash
mkdir build && cd build && cmake .. && cmake --build .
```
#### create wired connection
Sttings>Network> add a new wired connection
IPv4> Manual: address: 169.254.1.0 | netmask: 255.255.255.0 | gateway: 169.254.1.0

**Rq:** *The exe file is called lslpub_ESP. This file has also been copied in the bin floder of the git repository root.*

### 2.2 Windows 10
#### 2.2.1 **Requirements**
- [CMake](https://cmake.org/download/) (download and execute the installer for windows , add to the PATH variable)
- [MinGW32](https://sourceforge.net/projects/mingw-w64/) (download and execute the installer for windows, chose i686_64 architecture, add the the bin folder address of minGW to the PATH environement variable).
- [**Git**](https://git-for-windows.github.io/) Download and install git.
#### Steps
- Clone the repository and go inside.
```bash
git clone https://github.com/Aightech/lslpub_ESP.git && cd lslpub_ESP
```
- Create a build directory.
- Configure and generate the CMake project.
- Build the project.
```bash
mkdir build && cd build && cmake .. -G "MinGW Makefiles" && mingw32-make
```
**Rq:** *The exe file is called lslpub_ESP. This file has also been copied in the bin floder of the git repository root.*

#### 2.2.3 Build LSL library on windows
You can also follow this guide https://github.com/sccn/labstreaminglayer/blob/master/doc/BUILD.md.
##### 2.2.3.1 **Requirements**
- [**Cmake**](https://cmake.org/download/)
Download and install cmake.
- [**Qt**](https://www.qt.io/download)
Download qt installer (open source version).
- [**Boost libraries**](https://sourceforge.net/projects/boost/files/boost-binaries/)
Download boost lib (last binaries version).
- [**Git**](https://git-for-windows.github.io/)
Download and install git.
- [**MinGW32**](https://sourceforge.net/projects/mingw-w64/) (download and execute the installer for windows, chose i686_64 architecture, add the the bin folder address of minGW to the PATH environement variable) 

##### 2.2.3.2 Steps 
- Clone the repository and go inside.
```bash
git clone --recurse-submodules https://github.com/labstreaminglayer/labstreaminglayer.git &&cd labstreaminglayer
```
- Create a build repository and go inside.
```bash
mkdir build && cd build
```
Configure and generate the project with the GUI.
```bash
cmake-gui
```
Click on "configure". Select the MinGW MakeFile.
Check if the boost libraries and Qt are correct.
Then click on "generate".
In the command prompt, build the project.
```bash
cmake --build . --config Release --target install
```

