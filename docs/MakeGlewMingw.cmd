rem From http://stackoverflow.com/questions/6005076/building-glew-on-windows-with-mingw
rem Answer given by Barak Itkin (LightningIsMyName) on  May 14 '11 at 22:44
rem call C:\TDM-GCC-64\mingwvars.bat
rd /s /q lib
rd /s /q bin
rd /s /q obj
mkdir "lib"
mkdir "lib/Release"
mkdir "lib/Release MX"
mkdir "lib/Release/Win32"
mkdir "lib/Release MX/Win32"
mkdir "lib/Release/x64"
mkdir "lib/Release MX/x64"
mkdir "bin"
mkdir "bin/Release"
mkdir "bin/Release MX"
mkdir "bin/Release/Win32"
mkdir "bin/Release MX/Win32"
mkdir "bin/Release/x64"
mkdir "bin/Release MX/x64"
mkdir "obj"
rem 64 bit
gcc -DGLEW_NO_GLU -O2 -Wall -W -Iinclude  -DGLEW_BUILD -o obj/glew.o -c src/glew.c
gcc -shared -Wl,-soname,libglew32.dll -Wl,--out-implib,lib/Release/x64/libglew32.dll.a -o bin/Release/x64/glew32.dll obj/glew.o -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32
ar cr lib/Release/x64/libglew32.a obj/glew.o
gcc -DGLEW_NO_GLU -DGLEW_MX -O2 -Wall -W -Iinclude  -DGLEW_BUILD -o obj/glew.mx.o -c src/glew.c
gcc -shared -Wl,-soname,libglew32mx.dll -Wl,--out-implib,"lib/Release MX/x64/libglew32mx.dll.a" -o "bin/Release MX/x64/glew32mx.dll" obj/glew.mx.o -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32
ar cr "lib/Release MX/x64/libglew32mx.a" obj/glew.mx.o
gcc -c -O2 -Wall -W -Iinclude  -o obj/glewinfo.o src/glewinfo.c
gcc -O2 -Wall -W -Iinclude  -o bin/Release/x64/glewinfo.exe obj/glewinfo.o -Llib/Release/x64  -lglew32 -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32
gcc -c -O2 -Wall -W -Iinclude  -o obj/visualinfo.o src/visualinfo.c
gcc -O2 -Wall -W -Iinclude  -o bin/Release/x64/visualinfo.exe obj/visualinfo.o -Llib/Release/x64  -lglew32 -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32
rem 32 bit
rd /s /q obj
mkdir "obj"
gcc -m32 -DGLEW_NO_GLU -DGLEWAPIENTRY=__cdecl -O2 -Wall -W -Iinclude  -DGLEW_BUILD -o obj/glew.o -c src/glew.c
gcc -m32 -shared -Wl,-soname,libglew32.dll -Wl,--out-implib,lib/Release/Win32/libglew32.dll.a -o bin/Release/Win32/glew32.dll obj/glew.o -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32
ar cr lib/Release/Win32/libglew32.a obj/glew.o
gcc -m32 -DGLEW_NO_GLU -DGLEW_MX -DGLEWAPIENTRY=__cdecl -O2 -Wall -W -Iinclude  -DGLEW_BUILD -o obj/glew.mx.o -c src/glew.c
gcc -m32 -shared -Wl,-soname,libglew32mx.dll -Wl,--out-implib,"lib/Release MX/Win32/libglew32mx.dll.a" -o "bin/Release MX/Win32/glew32mx.dll" obj/glew.mx.o -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32
ar cr "lib/Release MX/Win32/libglew32mx.a" obj/glew.mx.o
gcc -m32 -DGLEWAPIENTRY=__cdecl -c -O2 -Wall -W -Iinclude  -o obj/glewinfo.o src/glewinfo.c
gcc -m32 -O2 -Wall -W -Iinclude  -o bin/Release/Win32/glewinfo.exe obj/glewinfo.o -Llib/Release/Win32  -lglew32 -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32
gcc -m32 -DGLEWAPIENTRY=__cdecl -c -O2 -Wall -W -Iinclude  -o obj/visualinfo.o src/visualinfo.c
gcc -m32 -O2 -Wall -W -Iinclude  -o bin/Release/Win32/visualinfo.exe obj/visualinfo.o -Llib/Release/Win32  -lglew32 -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32
rd /s /q obj
rem Done
