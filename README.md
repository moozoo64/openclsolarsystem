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
1. Download the latest Release from [GitHub](https://github.com/moozoo64/openclsolarsystem)
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

## Tools and Libraries Required to Compile the Source Under Windows

See the build configuration at: https://github.com/moozoo64/openclsolarsystem/blob/master/.github/workflows/build.yaml

### 1. Install MSYS2 Base Requirements
Install these packages using pacman:
```bash
pacman -S base-devel git zip
```

### 2. Install Development Tools
Choose either UCRT64 or CLANG64 toolchain. Below example uses UCRT64:
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-gdb
pacman -S mingw-w64-ucrt-x86_64-toolchain
pacman -S mingw-w64-ucrt-x86_64-glew
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-opencl-headers
pacman -S mingw-w64-ucrt-x86_64-opencl-icd
pacman -S mingw-w64-ucrt-x86_64-wxwidgets3.2-common
pacman -S mingw-w64-ucrt-x86_64-wxwidgets3.2-msw
pacman -S mingw-w64-ucrt-x86_64-wxwidgets3.2-msw-cb_headers
```

> **Note**: For CLANG64, replace `ucrt` with `clang` in the above commands

### 3. Building OpenCLSolarSystem
```bash
git clone https://github.com/moozoo64/openclsolarsystem.git
cd openclsolarsystem/src/OpenCLSolarSystem
mingw32-make -j8 BUILD_TYPE=Release ARCH=x64 clean
mingw32-make -j8 BUILD_TYPE=Release ARCH=x64
```

### 4. Building OrbToSlf
First install .NET 8 SDK, then:
```bash
cd src/OrbToSlf
dotnet build --configuration Release --framework net8.0
```

## Creating an initial.bin datafile

A Solex SLF formatted data file of the solar system is needed. 

A file `mpcsmall.slf` is included in the source. It was generated from:
- The MPC MPCORB.DAT
- JPLHorizons API for the planets and moon
- Both sources use the same epoch
- Generated using the program found in the OrbToSlf directory

The OrbToSlf program generates a .slf file from either:
- The Minor Planet Centre's MPCORB.DAT
- or astrorb.dat

Command line options are:
- `ASTORB` - use astorb.dat instead of MPC
- `NEOFirst` - Add NEO and NEO! asteroids before other types
- `AddDuplicate` - creates and adds a duplicate solar system
- `Numbers` - in order
- `maxbodies` - the maximum number of asteroids included in the output
- `numOort` - the number of random oort cloud bodies to add
- `xOffset` - The x axis to offset the duplicate system (if AddDuplicate is on)
- `yOffset` - The y axis to offset the duplicate system (if AddDuplicate is on)
- `vxOffset` - The x velocity offset of the duplicate system (if AddDuplicate is on)

e.g. OrbToSlf.exe 600000 300000 NEOFirst
Will add at most 600000 asteroids (real), 300000 random oort cloud bodies. NEO and NEO! objects will be added before other types

Alternatively SLF files can be made by the free version of Solex which can be found at http://www.solexorb.it/  
After running the program and loading in bodies look for the file FINAL.SLF in the USERDATA directory.

## Fun Stuff 🚀
Because SLF files are text files you can edit them to add additional bodies like:
- Rogue stars passing through the solar system
- Extra planets
- Higher mass Jupiter
- And more!

> **Note**: As long as something very massive is not added (e.g., massive black hole), the simulation should remain highly accurate.

## References and Acknowledgements 📚

### Integration Methods
- Adams Bashforth Moulton integration method from [Wikipedia](http://en.wikipedia.org/wiki/Linear_multistep_method)
- Coefficients generation algorithm from "Fundamentals of Celestrial Mechanics" by J.M.A. Danby (section 10.7)
- Relativistic corrections from "NUMERICAL INTEGRATION FOR THE REAL TIME PRODUCTION OF FUNDAMENTAL EPHEMERIDES OVER A WIDE TIME SPAN" by Aldo Vitagliano

### Inspirations
- [Solex](http://www.solexorb.it) by Aldo Vitagliano
- [DE118i](http://www.moshier.net/ssystem.html) by Steve Moshier

### Data Sources
- [Minor Planet Center](http://www.minorplanetcenter.net) - mpcorb.dat asteroid database
- [Astorb Database](https://asteroid.lowell.edu/astorb) by Dr. Edward Bowell
  > Funded by NASA grant NAG5-4741 and Lowell Observatory endowment

### Third-Party Software and Trademarks
- [The OpenGL Extension Wrangler Library](http://glew.sourceforge.net/)
- OpenCL® and the OpenCL logo are trademarks of Apple Inc. used by permission by Khronos
- OpenGL® and the oval logo are trademarks of Silicon Graphics, Inc.
- [MSYS2](https://www.msys2.org/) - Software Distribution and Building Platform for Windows
- [wxWidgets](https://www.wxwidgets.org/) - Cross-Platform GUI Library (Version 3.2)

### Licensing
This software includes several third-party works. Their licenses can be found in the Licenses folder.  
Only my original code is covered under the top-level license.txt file.

> **Note**: Please notify me if I've missed any required credit or license acknowledgements.
