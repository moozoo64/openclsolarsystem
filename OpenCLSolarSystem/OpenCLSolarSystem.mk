##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug_x64
ProjectName            :=OpenCLSolarSystem
ConfigurationName      :=Debug_x64
WorkspaceConfiguration :=Debug_x64
WorkspacePath          :=C:/SDK/openclsolarsystem
ProjectPath            :=C:/SDK/openclsolarsystem/OpenCLSolarSystem
IntermediateDirectory  :=./$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=michael
Date                   :=02/03/2025
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=C:/SDK/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/g++.exe
SharedObjectLinkerName :=C:/SDK/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputDirectory        :=$(IntermediateDirectory)
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="OpenCLSolarSystem.txt"
PCHCompileFlags        :=
MakeDirCommand         :="C:/Program Files/CodeLite/mkdir.exe" -p
RcCmpOptions           := $(shell wx-config --rcflags)
RcCompilerName         :=C:/SDK/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/windres.exe
LinkOptions            :=  -mwindows -LC:/wxWidgets-3.1.1/lib/gcc_dllx86_64 -pipe -lwxmsw31u_xrc -lwxmsw31u_webview -lwxmsw31u_html -lwxmsw31u_adv -lwxmsw31u_core -lwxbase31u_xml -lwxbase31u_net -lwxbase31u -lwxmsw31u_gl -lwxbase31u -lopengl32 -lglu32
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)$(OPENCL_INCLUDE) $(IncludeSwitch)$(GLEW_INCLUDE) 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)OpenCL $(LibrarySwitch)glew32 
ArLibs                 :=  "OpenCL" "glew32" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)$(OPENCL_LIB64) $(LibraryPathSwitch)$(GLEWLIB64) 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := C:/SDK/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/ar.exe -r
CXX      := C:/SDK/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/g++.exe
CC       := C:/SDK/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcc.exe
CXXFLAGS :=  -O0 -Wall -std=gnu++11 -g -DwxDEBUG_LEVEL=1 -IC:/wxWidgets-3.1.1/lib/gcc_dllx86_64/mswu -IC:/wxWidgets-3.1.1/include -mthreads -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXMSW__ -DHAVE_W32API_H -D_UNICODE -fmessage-length=0 -pipe -fno-keep-inline-dllexport -std=c++11 $(Preprocessors)
CFLAGS   :=  -O0 -Wall -std=gnu++11 -g -DwxDEBUG_LEVEL=1 $(shell wx-config --cxxflags --unicode=yes ) $(Preprocessors)
ASFLAGS  := 
AS       := C:/SDK/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
WXWIN32:=gcc510TDMDev_dll
WXWIN64:=gcc_dllx86_64
WXWIN:=C:\wxWidgets-3.1.1
WXCFG:=$WXWIN64/mswu
GLEW_INCLUDE:=C:\SDK\glew-2.1.0\include
GLEWBIN32:=C:\SDK\glew-2.1.0\bin\Release\Win32
GLEWBIN64:=C:\SDK\glew-2.1.0\bin\Release\x64
GLEWLIB32:=/SDK/glew-2.1.0/lib/Release/Win32
GLEWLIB64:=/SDK/glew-2.1.0/lib/Release/x64
GLM_INCLUDE:=C:\SDK\glm
OPENCL_INCLUDE:=C:\SDK\OpenCL-Headers\opencl22
OPENCL_LIB64:=C:\SDK\OpenCL
OPENCL_LIB32:=C:\Program Files (x86)\AMD APP SDK\3.0\lib\x86
PATH:=%WXWIN%\lib\%WXWIN64%;%GLEWBIN64%;%SystemRoot%;%SystemRoot%\System32;%CodeLiteDir%;C:\Program Files\SlikSvn\bin
PATH:=$WXWIN\lib\$WXWIN64;$GLEWBIN64;$PATH
Objects0=$(IntermediateDirectory)/application.cpp$(ObjectSuffix) $(IntermediateDirectory)/frame.cpp$(ObjectSuffix) $(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix) $(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix) $(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix) $(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix) $(IntermediateDirectory)/global.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

PostBuild:
	@echo Executing Post Build commands ...
	copy adamsfma.cl Debug_x64
	copy astrorbsolexsmall.slf Debug_x64
	copy C:\wxWidgets-3.1.1\lib\gcc_dllx86_64\wxbase311ud_gcc_cl.dll Debug_x64
	@echo Done

MakeIntermediateDirs:
	@$(MakeDirCommand) "./$(ConfigurationName)"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./$(ConfigurationName)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/application.cpp$(ObjectSuffix): application.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/openclsolarsystem/OpenCLSolarSystem/application.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/application.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/application.cpp$(PreprocessSuffix): application.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/application.cpp$(PreprocessSuffix) application.cpp

$(IntermediateDirectory)/frame.cpp$(ObjectSuffix): frame.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/openclsolarsystem/OpenCLSolarSystem/frame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/frame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/frame.cpp$(PreprocessSuffix): frame.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/frame.cpp$(PreprocessSuffix) frame.cpp

$(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix): physicalproperties.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/openclsolarsystem/OpenCLSolarSystem/physicalproperties.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/physicalproperties.cpp$(PreprocessSuffix): physicalproperties.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/physicalproperties.cpp$(PreprocessSuffix) physicalproperties.cpp

$(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix): initialstate.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/openclsolarsystem/OpenCLSolarSystem/initialstate.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/initialstate.cpp$(PreprocessSuffix): initialstate.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/initialstate.cpp$(PreprocessSuffix) initialstate.cpp

$(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix): glcanvas.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/openclsolarsystem/OpenCLSolarSystem/glcanvas.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/glcanvas.cpp$(PreprocessSuffix): glcanvas.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/glcanvas.cpp$(PreprocessSuffix) glcanvas.cpp

$(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix): clmodel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/openclsolarsystem/OpenCLSolarSystem/clmodel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/clmodel.cpp$(PreprocessSuffix): clmodel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/clmodel.cpp$(PreprocessSuffix) clmodel.cpp

$(IntermediateDirectory)/global.cpp$(ObjectSuffix): global.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/openclsolarsystem/OpenCLSolarSystem/global.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/global.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/global.cpp$(PreprocessSuffix): global.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/global.cpp$(PreprocessSuffix) global.cpp

##
## Clean
##
clean:
	$(RM) -r ./$(ConfigurationName)/


