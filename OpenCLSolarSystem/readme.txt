Solar System Simulation by Michael Simmons

This program simulates the Solar System through numerical integration.
It utilises OpenCL(tm) to enable the integration of not only the planets but also 500000+ asteroids.

The latest version of this program and its source can be found at
https://sourceforge.net/projects/openclsolarsyst

Running
------------------------------------
Install the latest graphics card drivers for your graphics card.
For Nvidia cards install the geforce 350.12 driver or later (with opencl 1.2 support)

This software requires opencl double precision floating point support.

If, when the program starts for the first time, it displays an error indicating it could not load initial.bin then
Use File->Import SLF to import a Solex SLF formated file.
Then use File->Save Inital to save a binary initialization file initial.bin in the same directory as the program.

Set the Time Delta to 4 hr, The Integration to "Adams Bashforth Moulton 11" , the Gravity to "With Relatvistic corrections" and then Go->Start
To see the moon (if it was in the .SLF file), Center on body 3 and then use the Z key to zoom in.

There is a bug where the Number of Bodies on initial startup is set to maximum but is actually 2560. Change the Number of Bodies to some other value and back.

Command Line options
---------------------
-stereo = switch to 3d stereoscopic mode (requires a graphics card and a 3d monitor). I have only tested this with AMD HD3D. Note you will probably need to manually switch back to 2D mode afterwards.
-cpu = attempt to use a CPU type opencl driver
-amd = attempt to use and AMD opencl device
-nvidia = attempt to use a NVIDIA opencl device
-intel = attempt to use a Intel opencl device

Stability and Accuracy
------------------------
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

Tools and libraries require to compile the source under windows
----------------------------------------------------------------
Checkout the source via svn from https://sourceforge.net/projects/openclsolarsyst
You will need:
Codelite (http://codelite.org/) version 7+ 64 bit
Both wxWidgets libraries from the Codelite download section (wxWidgets-3.0.2 for 32 bit and wxWidgets-3.1 for 64 bit....)
TDM-GCC-64 web installer. Install for 32bit and 64bit development. I put it in C:\TDM-GCC-64
The Library (http://glew.sourceforge.net/) glew-1.12.0 (compile by putting MakeGlewMingw.cmd in its folder and running it)
The Khronos opencl headers from http://www.khronos.org/registry/cl/ or from the AMD or Intel SDK
The OpenCL.lib from Intel OpenCL SDK or AMD SDKs

This is my current Codelite Enviroment Settings
CodeLiteDir=C:\Program Files\CodeLite
WXWIN32=C:\wxWidgets-3.0.2
WXWIN64=C:\wxWidgets-3.1
GLEW_INCLUDE=C:\SDK\glew-1.12.0\include
GLEWBIN32=C:\SDK\glew-1.12.0\bin\Release\Win32
GLEWBIN64=C:\SDK\glew-1.12.0\bin\Release\x64
GLEWLIB32=/SDK/glew-1.12.0/lib/Release/Win32
GLEWLIB64=/SDK/glew-1.12.0/lib/Release/x64
OPENCL_INCLUDE=C:\Program Files (x86)\AMD APP SDK\3.0-0-Beta\include
OPENCL_LIB64=C:\Program Files (x86)\AMD APP SDK\3.0-0-Beta\lib\x86_64
OPENCL_LIB32=C:\Program Files (x86)\AMD APP SDK\3.0-0-Beta\lib\x86

To Compile under Ubuntu 12.04 LTS open the OpenCLSolarSystemUbuntu.project and set enviroment enviroment variables as required.
The program runs much slower under linux. Try disabling the status bar updates and running it at an elevated priority.

Creating an initial.bin datafile
--------------------------------
A Solex SLF formated data file of the solar system is needed.
A file astrorbsolexsmall.slf is included in the source.
It was generated from the astrorb.dat (http://www.naic.edu/~nolan/astorb.html) and a solex FINAL.SLF of the planets and moon for the same epoch.

A free version of Solex can be found at http://chemistry.unina.it/~alvitagl/solex/
After running the program and loading in bodies look for the file FINAL.SLF in the USERDATA directory.

I wrote a separate program that can generate a .SLF file of the planets (approx positions) and all of the astrorb or mpcorb asteroids
This program can be found in the OrbToSlf directory.
It will generate a .slf file from either the Minor Planet Centres MPCORB.DAT or astrorb.dat.
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

Currently the .slf file generated by OrbToSlf.exe does not have the moon or accurate planet positions.
To include this, use Solex to generate a FINAL.SLF for the relevant epoch and then copy and paste into the FINAL.SLF file generated by OrbToSlf.exe the required bodies.

References and Acknowledgements
-------------------------------
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
