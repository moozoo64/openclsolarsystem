
OpenCL Solar System by Michael Simmons

This program simulates the Solar System through numerical integration.
It utilises opencl to enable the integration of not only the planets but also 600000 odd asteroids.

A Solex SLF formated data file of the solar system is needed.
A file astrorbsolexsmall.slf is included in the source.
It was generated from the astrorb.dat (http://www.naic.edu/~nolan/astorb.html) and a solex FINAL.SLF of the planets and moon for the same epoch.

A free version of Solex can be found at http://chemistry.unina.it/~alvitagl/solex/
After running the program and loading in bodies look for the file FINAL.SLF in the USERDATA directory.

I wrote a seperate program that can generate a .SLF file of the planets (approx positions) and all of the astrorb asteroids
I will look at releasing that program at sometime in future.

Running
------------------------------------
If, when the program starts for the first time, it displays an error indicating it could not load initial.bin then
Use File->Import SLF to import a Solex SLF formated file.
Then use File->Save Inital to save a binary initialization file initial.bin in the same directory as the program.

Set the Time Delta to 4 hr, The Integration to "Adams Bashforth Moulton 11" , the Gravity to "With Relatvistic corrections" and then Go->Start
To see the moon (if it was in the .SLF file), Center on body 3 and then use the Z key to zoom in.

Tools and libraries require to compile the source under windows
----------------------------------------------------------------
Codelite (http://codelite.org/) download the "codelite IDE + MinGW suite + wxWidgets SDK " install (C:\SDK\CodeLite\ , MinGW & wxWidgets under the codelite directory )
The Library (http://glew.sourceforge.net/) glew-1.9.0 (C:\SDK\glew-1.9.0) you need to compile this from source using msys and mingw
The Khronos opencl headers from http://www.khronos.org/registry/cl/ (put opencl.h in C:\SDK\Khronos\opencl1.2 the rest in a subfolder CL)
The OpenCL.lib from Intel OpenCL SDK (put in C:\SDK\Khronos\opencl1.2\lib)

Stability and Accuracy
------------------------
During the first 16 time steps the program initialises the Adams Bashforth Moulton history.
Currently this is done in a very inaccurate manner. I am implementing a runge kutta 7th order method for this in the future.

The Adams Bashforth Moulton integration method is unstable for higher order methods used with large time steps.
With "Adams Bashforth Moulton 11" and a Time Delta to 4 hr the integration appears stable.
Feel free to experiment.
I believe With "Adams Bashforth Moulton 16" and a Time Delta of 15 mins is stable.
So is "Adams Bashforth Moulton 8" and a Time Delta one day.

The higher the order and the smaller the time step the more accurate the result.

References and Acknowledgements
-------------------------------
The Adams Bashforth Moulton integration method is from Wikipedia http://en.wikipedia.org/wiki/Linear_multistep_method

A seperate program written in C# was used to generated the Adams Bashforth Moulton co-efficents used in adams.cl
The algorithm is given in Fundamentals of Celestrial Mechanics by J.M.A. Danby section 10.7.
The calculation was done using rational numbers. I used my own BigRational class combined with System.Nurmerics.BigInteger.

The inspiration to create this program was from the programs Solex by Aldo Vitagliano (http://chemistry.unina.it/~alvitagl/solex/) and DE118i by Steve Moshier (http://www.moshier.net/ssystem.html) 

The relatvistic correction used is from the paper "NUMERICAL INTEGRATION FOR THE REAL TIME PRODUCTION OF FUNDAMENTAL EPHEMERIDES OVER A WIDE TIME SPAN" by Aldo Vitagliano

astrorb.dat asteroid database can be found at http://www.naic.edu/~nolan/astorb.html and is the work of Dr. Edward Bowell. It was funded principally by NASA grant NAG5-4741, and in part by the Lowell Observatory endowment. 

"The OpenGL Extension Wrangler Library" http://glew.sourceforge.net/ is used by this program.

Other than my own motivation I received no encouragement or support in creating this software.
