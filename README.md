# Solar System Simulation 🌠
> A high-performance OpenCL-based solar system simulator by Michael Simmons
> Highly accurate gravity based simulation.

## Overview 📝
This program simulates the Solar System through numerical integration.
It utilises OpenCL™ to enable the integration of not only the planets but also 1,400,000+ asteroids.

📥 The latest version of this program and its source can be found at:  
https://github.com/moozoo64/openclsolarsystem

## Getting Started 🚀

### Prerequisites
* Latest graphics card drivers
* Modern Graphics card with OpenCL support. Version 1.2 or greater.
* OpenCL double precision floating point support

### Installation
1. Download from [GitHub](https://github.com/moozoo64/openclsolarsystem)
2. First run requires OpenCL device selection:
   ```powershell
   OpenCLSolarSystem.exe -nvidia  # For NVIDIA GPUs
   OpenCLSolarSystem.exe -amd     # For AMD GPUs
   OpenCLSolarSystem.exe -intel   # For Intel GPUs
   OpenCLSolarSystem.exe -cpu     # For CPU computation
   ```

### Initial Setup
If you see an `initial.bin` error on first launch:
1. Go to `File -> Import SLF`
2. Select the included `mpcsmall.slf` (contains 32,768 objects)
3. Save with `File -> Save Initial`

> 💡 **Tip**: For full 1.4 million objects simulation:
> 1. Run `OrbToSlfConsole.exe` in the OrbToSlf folder
> 2. Import the generated `Final.slf`

### Recommended Settings ⚙️
* Time Delta: 4 hr
* Integration: "Adams Bashforth Moulton 11"
* Gravity: "With Relativistic corrections"
* Start with: `Go->Start`

> ⚠️ Some combination of settings are unstable and the solar system will fall apart.
> See "Stability and Accuracy" below

### To view the moon:🌙
1. Center on body Earth
2. Use the mouse scroll wheel to zoom in

> ⚠️ **Known Issue**: Initial "Number of Bodies" shows maximum but is actually 2560.  
> **Workaround**: Change the value to another number and back.

## Command Line Options

| Option    | Description |
|-----------|-------------|
| `-stereo` | Enable 3D stereoscopic mode (requires compatible hardware) |
| `-cpu`    | Use CPU OpenCL driver |
| `-amd`    | Use AMD OpenCL device |
| `-nvidia` | Use NVIDIA OpenCL device |
| `-intel`  | Use Intel OpenCL device |

> **Note**: For `-stereo`, manual switch back to 2D mode may be required. Tested with AMD HD3D.

## Stability and Accuracy
During the first 16 time steps the program initialises the Adams Bashforth Moulton history.
Currently this is done in a very inaccurate manner. I plan to implement a runge kutta 7th order method for this in the future.

The Adams Bashforth Moulton integration method is unstable for higher order methods used with large time steps.
With "Adams Bashforth Moulton 11" and a Time Delta to 4 hr the integration appears stable.
Feel free to experiment.
I believe With "Adams Bashforth Moulton 16" and a Time Delta of 15 mins is stable.
So is "Adams Bashforth Moulton 8" and a Time Delta one day.

The higher the order and the smaller the time step the more accurate the result.

The option "Detect Close Encounters" combined with Center on Earth can be used to find Close earth encounters.
This can be compared with the lists from http://neo.jpl.nasa.gov/cgi-bin/neo_ca

## Tools and libraries require to compile the source under windows
see https://github.com/moozoo64/openclsolarsystem/blob/master/.github/workflows/build.yaml
Install MSYS2 and then pacman install these
  base-devel
  git
  zip
then pacman install these pacman install these for either ucrt-x86_64 or clang-x86_64
below is for ucrt-x86_64 for CLANG64 substitute clang-x86_64
  mingw-w64-ucrt-x86_64-gcc
  mingw-w64-ucrt-x86_64-gdb
  mingw-w64-ucrt-x86_64-toolchain
  mingw-w64-ucrt-x86_64-glew
  mingw-w64-ucrt-x86_64-cmake
  mingw-w64-ucrt-x86_64-opencl-headers
  mingw-w64-ucrt-x86_64-opencl-icd
  mingw-w64-ucrt-x86_64-wxwidgets3.2-common
  mingw-w64-ucrt-x86_64-wxwidgets3.2-msw
  mingw-w64-ucrt-x86_64-wxwidgets3.2-msw-cb_headers
git clone https://github.com/moozoo64/openclsolarsystem.git
cd openclsolarsystem/src/OpenCLSolarSystem
mingw32-make -j8 BUILD_TYPE=Release ARCH=x64 clean
mingw32-make -j8 BUILD_TYPE=Release ARCH=x64  

for OrbToSlf install dotnet 8 SDK and then
cd src/OrbToSlf
dotnet build --configuration Release --framework net8.0

## Creating an initial.bin datafile
A Solex SLF formated data file of the solar system is needed.
A file mpcsmall.slf is included in the source.
It was generated from the MPC MPCORB.DAT and JPLHorizons api for the planets and moon for the same epoch using the program found in the OrbToSlf directory.
It generates a .slf file from either the Minor Planet Centres MPCORB.DAT or astrorb.dat.
Command line options are
ASTORB use astorb.dat instead of MPC
NEOFirst Add NEO and NEO! asteroids before other types
AddDuplicate - creates and adds a duplicate solar system
Numbers in order
maxbodies the maximum number of asteroids included in the output
numOort - the number of random oort cloud bodies to add
xOffset - The x axis to offset the duplicate system (if AddDuplicate is on)
yOffset - The y axis to offset the duplicate system (if AddDuplicate is on)
vxOffset - The x velocity offset of the duplicate system (if AddDuplicate is on)

e.g. OrbToSlf.exe 600000 300000 NEOFirst
Will add at most 600000 asteroids (real), 300000 random oort cloud bodies. NEO and NEO! objects will be added before other types

Alternatively free version of Solex can be found at http://chemistry.unina.it/~alvitagl/solex/
After running the program and loading in bodies look for the file FINAL.SLF in the USERDATA directory.

## Fun Stuff
Because SLF files are text files you can edit them to add additional bodies like rogue stars passing through the solar system, extra planets or a higher mass Jupiter and so on.
So long something very massive is not added (eg massive black hole) the simulation should be highly accurate.

## References and Acknowledgements
The Adams Bashforth Moulton integration method is from Wikipedia http://en.wikipedia.org/wiki/Linear_multistep_method

A separate program written in C# was used to generated the Adams Bashforth Moulton co-efficents used in adamsfma.cl
The algorithm is given in "Fundamentals of Celestrial Mechanics" by J.M.A. Danby section 10.7.
The calculation was done using rational numbers. I used my own BigRational class combined with System.Nurmerics.BigInteger.

The inspiration to create this program was from the programs Solex by Aldo Vitagliano (http://chemistry.unina.it/~alvitagl/solex/) and DE118i by Steve Moshier (http://www.moshier.net/ssystem.html) 

The relativistic correction used is from the paper "NUMERICAL INTEGRATION FOR THE REAL TIME PRODUCTION OF FUNDAMENTAL EPHEMERIDES OVER A WIDE TIME SPAN" by Aldo Vitagliano

The binary distribution contains the file initial.bin.
This file contains asteroid data based on the mpcorb.dat asteroid database and planet data from the program Solex.
mpcorb.dat asteroid database can be found at the Minor Planet Center website http://www.minorplanetcenter.net
astorb.dat asteroid database can be found at http://www.naic.edu/~nolan/astorb.html and is the work of Dr. Edward Bowell. It was funded principally by NASA grant NAG5-4741, and in part by the Lowell Observatory endowment. 

OpenCL® and the OpenCL logo are trademarks of Apple Inc. used by permission by Khronos
OpenGL® and the oval logo are trademarks or registered trademarks of Silicon Graphics, Inc. in the United States and/or other countries worldwide.

"The OpenGL Extension Wrangler Library" http://glew.sourceforge.net/ is used by this program.

Other than my own motivation I received no encouragement or support in creating this software.
