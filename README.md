Solar System Simulation by Michael Simmons

WARNING Work in progress
Currently being moved across to github and switching to VS Code.
Plan is to use GitHub actions to build releases.

This program simulates the Solar System through numerical integration.
It utilises OpenCL(tm) to enable the integration of not only the planets but also 500000+ asteroids.

The latest version of this program and its source can be found at
https://github.com/moozoo64/openclsolarsystem

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
Currently being updated
docs/fetch.ps1 is currently broken

Creating an initial.bin datafile
--------------------------------
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
