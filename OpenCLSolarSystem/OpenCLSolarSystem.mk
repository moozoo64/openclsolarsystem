##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release_x64
ProjectName            :=OpenCLSolarSystem
ConfigurationName      :=Release_x64
WorkspacePath          :=C:/Users/michael/Documents/CodeLite/openclsolarsyst-code-0
ProjectPath            :=C:/Users/michael/Documents/CodeLite/openclsolarsyst-code-0/OpenCLSolarSystem
IntermediateDirectory  :=./$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=michael
Date                   :=02/04/2018
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :="C:/Program Files/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/g++.exe"
SharedObjectLinkerName :="C:/Program Files/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/g++.exe" -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="OpenCLSolarSystem.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := $(shell wx-config --rcflags)
RcCompilerName         :="C:/Program Files/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/windres.exe"
LinkOptions            :=  -mwindows -s $(shell wx-config --debug=no --libs --unicode=yes --libs gl)
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)"$(OPENCL_INCLUDE)" $(IncludeSwitch)"$(GLEW_INCLUDE)" 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)OpenCL $(LibrarySwitch)glew32 
ArLibs                 :=  "OpenCL.dll" "glew32" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)"$(OPENCL_LIB64)" $(LibraryPathSwitch)$(GLEWLIB64) 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := "C:/Program Files/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/ar.exe" rcu
CXX      := "C:/Program Files/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/g++.exe"
CC       := "C:/Program Files/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcc.exe"
CXXFLAGS :=  -std=gnu++11 -O2 -Wall -DwxDEBUG_LEVEL=0 $(shell wx-config --cxxflags --unicode=yes --debug=no) $(Preprocessors)
CFLAGS   :=  -std=gnu++11  -O2 -Wall -DwxDEBUG_LEVEL=0 $(shell wx-config --cxxflags --unicode=yes --debug=no) $(Preprocessors)
ASFLAGS  := 
AS       := "C:/Program Files/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/as.exe"


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
Objects0=$(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix) $(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix) $(IntermediateDirectory)/global.cpp$(ObjectSuffix) $(IntermediateDirectory)/frame.cpp$(ObjectSuffix) $(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix) $(IntermediateDirectory)/application.cpp$(ObjectSuffix) $(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix) 



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
	copy adamsfma.cl Release_x64
	copy astrorbsolexsmall.slf Release_x64
	copy C:\wxWidgets-3.1.1\lib\gcc_dllx86_64\wxbase311u_gcc_cl.dll Release_x64
	@echo Done

MakeIntermediateDirs:
	@$(MakeDirCommand) "./$(ConfigurationName)"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./$(ConfigurationName)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix): physicalproperties.cpp $(IntermediateDirectory)/physicalproperties.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/michael/Documents/CodeLite/openclsolarsyst-code-0/OpenCLSolarSystem/physicalproperties.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/physicalproperties.cpp$(DependSuffix): physicalproperties.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/physicalproperties.cpp$(DependSuffix) -MM physicalproperties.cpp

$(IntermediateDirectory)/physicalproperties.cpp$(PreprocessSuffix): physicalproperties.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/physicalproperties.cpp$(PreprocessSuffix) physicalproperties.cpp

$(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix): initialstate.cpp $(IntermediateDirectory)/initialstate.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/michael/Documents/CodeLite/openclsolarsyst-code-0/OpenCLSolarSystem/initialstate.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/initialstate.cpp$(DependSuffix): initialstate.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/initialstate.cpp$(DependSuffix) -MM initialstate.cpp

$(IntermediateDirectory)/initialstate.cpp$(PreprocessSuffix): initialstate.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/initialstate.cpp$(PreprocessSuffix) initialstate.cpp

$(IntermediateDirectory)/global.cpp$(ObjectSuffix): global.cpp $(IntermediateDirectory)/global.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/michael/Documents/CodeLite/openclsolarsyst-code-0/OpenCLSolarSystem/global.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/global.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/global.cpp$(DependSuffix): global.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/global.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/global.cpp$(DependSuffix) -MM global.cpp

$(IntermediateDirectory)/global.cpp$(PreprocessSuffix): global.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/global.cpp$(PreprocessSuffix) global.cpp

$(IntermediateDirectory)/frame.cpp$(ObjectSuffix): frame.cpp $(IntermediateDirectory)/frame.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/michael/Documents/CodeLite/openclsolarsyst-code-0/OpenCLSolarSystem/frame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/frame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/frame.cpp$(DependSuffix): frame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/frame.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/frame.cpp$(DependSuffix) -MM frame.cpp

$(IntermediateDirectory)/frame.cpp$(PreprocessSuffix): frame.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/frame.cpp$(PreprocessSuffix) frame.cpp

$(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix): glcanvas.cpp $(IntermediateDirectory)/glcanvas.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/michael/Documents/CodeLite/openclsolarsyst-code-0/OpenCLSolarSystem/glcanvas.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/glcanvas.cpp$(DependSuffix): glcanvas.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/glcanvas.cpp$(DependSuffix) -MM glcanvas.cpp

$(IntermediateDirectory)/glcanvas.cpp$(PreprocessSuffix): glcanvas.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/glcanvas.cpp$(PreprocessSuffix) glcanvas.cpp

$(IntermediateDirectory)/application.cpp$(ObjectSuffix): application.cpp $(IntermediateDirectory)/application.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/michael/Documents/CodeLite/openclsolarsyst-code-0/OpenCLSolarSystem/application.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/application.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/application.cpp$(DependSuffix): application.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/application.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/application.cpp$(DependSuffix) -MM application.cpp

$(IntermediateDirectory)/application.cpp$(PreprocessSuffix): application.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/application.cpp$(PreprocessSuffix) application.cpp

$(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix): clmodel.cpp $(IntermediateDirectory)/clmodel.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/michael/Documents/CodeLite/openclsolarsyst-code-0/OpenCLSolarSystem/clmodel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/clmodel.cpp$(DependSuffix): clmodel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/clmodel.cpp$(DependSuffix) -MM clmodel.cpp

$(IntermediateDirectory)/clmodel.cpp$(PreprocessSuffix): clmodel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/clmodel.cpp$(PreprocessSuffix) clmodel.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./$(ConfigurationName)/


