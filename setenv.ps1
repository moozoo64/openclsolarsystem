#Set $base to where you have the tools installed then
#.\setenv.ps1
#$base = (Split-Path $PSScriptRoot)
$env:Path = "$env:SystemRoot;$env:SystemRoot\System32"
$env:Mingw64 = "$base\x86_64-14.2.0-release-posix-seh-msvcrt-rt_v12-rev1"
$env:Path = "$env:Mingw64\mingw64\bin;$base\mkdir;" + $env:Path
$env:DotNet = "$base\dotnet-sdk-8.0.406-win-x64"
$env:Path = "$env:DotNet;" + $env:Path
$env:CMake = "$base\cmake-3.31.6-windows-x86_64"
$env:Path = "$env:CMake\bin;" + $env:Path
$env:WxWidgets = "$base\wxWidgets-3.2.6"
$env:GLEW = "$base\glew-2.1.0"
$env:GLEWBIN32 = "$env:GLEW\bin\Release\Win32"
$env:GLEWBIN64 = "$env:GLEW\bin\Release\x64"
$env:GLEWLIB32 = "$env:GLEW\lib\Release\Win32"
$env:GLEWLIB64 = "$env:GLEW\lib\Release\x64"
$env:GLEW_INCLUDE = "$env:GLEW\include"
$env:LC_ALL = "C"
$env:OPENCL_INCLUDE = "$base\OpenCL-SDK-v2024.10.24-Win-x64\include"
$env:OPENCL_LIB64 = "$base\OpenCL"
$env:TERM = "xterm-256color"
$env:WXWIN = $env:WxWidgets
$env:WXWIN64 = "gcc_dllx86_64"
$env:WXCFG = "$env:WXWIN64/mswu"
$env:PATH += ";$env:WXWIN\lib\$env:WXWIN64;$env:GLEWBIN64"
#cd src
#mingw32-make -j8 BUILD_TYPE=Release ARCH=x64
