#Set $base to where you have the tools installed then
#.\fetch.ps1
# Currently broken, but the idea is to download and extract the tools needed to build the project.
exit
Set-location $base
New-Item -Path "$base\bin" -ItemType Directory
$env:Path = "$env:SystemRoot;$env:SystemRoot\System3;$base\bin"

$sourceUrl = "https://github.com/ip7z/7zip/releases/download/24.09/7zr.exe"
$destinationPath = "$base\bin\7zr.exe"
Invoke-WebRequest -Uri $sourceUrl -OutFile $destinationPath

$7zipPath = "$destinationPath"
if (-not (Test-Path -Path $7zipPath -PathType Leaf)) {
  throw "7 zip executable '$7zipPath' not found"
}

New-Item -Path "$base\zips" -ItemType Directory

$sourceUrl = "https://github.com/git-for-windows/git/releases/download/v2.48.1.windows.1/PortableGit-2.48.1-64-bit.7z.exe"
$destinationPath = "$base\zips\PortableGit-2.48.1-64-bit.7z.exe"
Invoke-WebRequest -Uri $sourceUrl -OutFile $destinationPath
7zr.exe x "$base\zips\PortableGit-2.48.1-64-bit.7z.exe" -o"$base\PortableGit-2.48.1-64-bit"
$env:Path = "$base\PortableGit-2.48.1-64-bit;$base\PortableGit-2.48.1-64-bit\bin;" + $env:Path

$sourceUrl = "https://github.com/Kitware/CMake/releases/download/v3.31.6/cmake-3.31.6-windows-x86_64.zip"
$destinationPath = "$base\zips\cmake-3.31.6-windows-x86_64.zip"
Invoke-WebRequest -Uri $sourceUrl -OutFile $destinationPath
Expand-Archive -LiteralPath "$base\zips\cmake-3.31.6-windows-x86_64.zip" -DestinationPath "$base\cmake-3.31.6-windows-x86_64"
$env:Path = "$base\cmake-3.31.6-windows-x86_64\cmake-3.31.6-windows-x86_64\bin;" + $env:Path
$env:CMAKE_GENERATOR = "MinGW Makefiles"

$sourceUrl = "https://github.com/niXman/mingw-builds-binaries/releases/download/14.2.0-rt_v12-rev1/x86_64-14.2.0-release-posix-seh-msvcrt-rt_v12-rev1.7z"
$destinationPath = "$base\zips\x86_64-14.2.0-release-posix-seh-msvcrt-rt_v12-rev1.7z"
Invoke-WebRequest -Uri $sourceUrl -OutFile $destinationPath
7zr.exe x "$base\zips\x86_64-14.2.0-release-posix-seh-msvcrt-rt_v12-rev1.7z" -o"$base\x86_64-14.2.0-release-posix-seh-msvcrt-rt_v12-rev1"
$env:Path = "$base\x86_64-14.2.0-release-posix-seh-msvcrt-rt_v12-rev1\mingw64\bin;" + $env:Path

Set-location $base
git clone https://github.com/alantsd/mingw32.git mkdir
Set-Location mkdir
.\build.bat
Set-location $base
$env:Path = "$base\mkdir;" + $env:Path

$sourceUrl = "https://github.com/nigels-com/glew/archive/refs/tags/glew-2.2.0.zip"
$destinationPath = "$base\zips\glew-2.2.0.zip"
Invoke-WebRequest -Uri $sourceUrl -OutFile $destinationPath
Expand-Archive -LiteralPath "$base\zips\glew-2.2.0.zip" -DestinationPath "$base\glew-2.2.0"
Set-Location "$base\glew-2.2.0\glew-glew-2.2.0\build\cmake"
cmake
Set-location $base

git clone --recursive https://github.com/KhronosGroup/OpenCL-SDK.git
Set-location "$base\OpenCL-SDK"
makedir build
Set-location build
cmake ..
mingw32-make -j8

Set-location $base
$sourceUrl = "https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.6/wxWidgets-3.2.6.7z"
$destinationPath = "$base\zips\wxWidgets-3.2.6.7z"
Invoke-WebRequest -Uri $sourceUrl -OutFile $destinationPath
7zr.exe x "$base\zips\wxWidgets-3.2.6.7z" -o"$base\wxWidgets-3.2.6"

$env:WxWidgets = "$base\wxWidgets-3.2.6"
Set-Location "$env:WxWidgets\build\msw"
mingw32-make -f makefile.gcc setup_h SHARED=1 UNICODE=1 BUILD=release VENDOR=cl CFG=x86_64
#notepad.exe "$env:WxWidgets\lib\gcc_dllx86_64\mswu\wx\setup.h"
#find wxUSE_GRAPHICS_CONTEXT and change the number from 0 to 1 for all of them
mingw32-make -j8 -f Makefile.gcc SHARED=1 UNICODE=1 BUILD=release VENDOR=cl CFG=x86_64 CXXFLAGS="-fno-keep-inline-dllexport -std=c++11"
mingw32-make -f makefile.gcc setup_h SHARED=1 UNICODE=1 BUILD=debug VENDOR=cl CFG=x86_64
#notepad.exe "$env:WxWidgets\lib\gcc_dllx86_64\mswud\wx\setup.h"
#find wxUSE_GRAPHICS_CONTEXT and change the number from 0 to 1 for all of them
mingw32-make -j8 -f Makefile.gcc SHARED=1 UNICODE=1 BUILD=debug VENDOR=cl CFG=x86_64 CXXFLAGS="-fno-keep-inline-dllexport -std=c++11"
Set-location $base
