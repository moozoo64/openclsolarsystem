##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug_x64
ProjectName            :=OpenCLSolarSystem
ConfigurationName      :=Debug_x64
WorkspacePath          := "C:\SDK\OpenCLSolarSystem"
ProjectPath            := "C:\SDK\OpenCLSolarSystem\OpenCLSolarSystem"
IntermediateDirectory  :=./$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=00025202
Date                   :=04/28/15
CodeLitePath           :="C:\Program Files\CodeLite"
LinkerName             :=C:/TDM-GCC-64/bin/g++.exe 
SharedObjectLinkerName :=C:/TDM-GCC-64/bin/g++.exe -shared -fPIC
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
RcCompilerName         :=C:/TDM-GCC-64/bin/windres.exe 
LinkOptions            :=  -mwindows $(shell wx-config --debug=yes --libs --unicode=yes --libs gl)
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)"$(OPENCL_INCLUDE)" $(IncludeSwitch)"$(GLEW_INCLUDE)" 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)OpenCL $(LibrarySwitch)glew32 
ArLibs                 :=  "OpenCL" "glew32" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)"$(OPENCL_LIB64)" $(LibraryPathSwitch)$(GLEWLIB64) 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/TDM-GCC-64/bin/ar.exe rcu
CXX      := C:/TDM-GCC-64/bin/g++.exe 
CC       := C:/TDM-GCC-64/bin/gcc.exe 
CXXFLAGS :=  -std=gnu++11 -g -O0 -Wall $(shell wx-config --cxxflags --unicode=yes --debug=yes) $(Preprocessors)
CFLAGS   :=  -std=gnu++11 -g -O0 -Wall $(shell wx-config --cxxflags --unicode=yes --debug=yes) $(Preprocessors)
ASFLAGS  := 
AS       := C:/TDM-GCC-64/bin/as.exe 


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files (x86)\CodeLite
WXWIN32:=C:\wxWidgets-3.0.2
WXWIN64:=C:\wxWidgets-3.1
GLEW_INCLUDE:=C:\SDK\glew-1.12.0\include
GLEWBIN32:=C:\SDK\glew-1.12.0\bin\Release\Win32
GLEWBIN64:=C:\SDK\glew-1.12.0\bin\Release\x64
GLEWLIB32:=/SDK/glew-1.12.0/lib/Release/Win32
GLEWLIB64:=/SDK/glew-1.12.0/lib/Release/x64
OPENCL_INCLUDE:=C:\Program Files (x86)\Intel\OpenCL SDK\4.6\include
OPENCL_LIB32:=C:\Program Files (x86)\Intel\OpenCL SDK\4.6\lib\x86
OPENCL_LIB64:=C:\Program Files (x86)\Intel\OpenCL SDK\4.6\lib\x64
WXWIN:=$WXWIN64
PATH:=$WXWIN\lib\gcc_dll;$GLEWBIN64;$PATH
Objects0=$(IntermediateDirectory)/global.cpp$(ObjectSuffix) $(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix) $(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix) $(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix) $(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix) $(IntermediateDirectory)/frame.cpp$(ObjectSuffix) $(IntermediateDirectory)/application.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

PostBuild:
	@echo Executing Post Build commands ...
	copy adamsfma.cl $(ConfigurationName)
	copy astrorbsolexsmall.slf $(ConfigurationName)
	@echo Done

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./$(ConfigurationName)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/global.cpp$(ObjectSuffix): global.cpp $(IntermediateDirectory)/global.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/OpenCLSolarSystem/global.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/global.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/global.cpp$(DependSuffix): global.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/global.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/global.cpp$(DependSuffix) -MM "global.cpp"

$(IntermediateDirectory)/global.cpp$(PreprocessSuffix): global.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/global.cpp$(PreprocessSuffix) "global.cpp"

$(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix): initialstate.cpp $(IntermediateDirectory)/initialstate.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/OpenCLSolarSystem/initialstate.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/initialstate.cpp$(DependSuffix): initialstate.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/initialstate.cpp$(DependSuffix) -MM "initialstate.cpp"

$(IntermediateDirectory)/initialstate.cpp$(PreprocessSuffix): initialstate.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/initialstate.cpp$(PreprocessSuffix) "initialstate.cpp"

$(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix): physicalproperties.cpp $(IntermediateDirectory)/physicalproperties.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/OpenCLSolarSystem/physicalproperties.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/physicalproperties.cpp$(DependSuffix): physicalproperties.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/physicalproperties.cpp$(DependSuffix) -MM "physicalproperties.cpp"

$(IntermediateDirectory)/physicalproperties.cpp$(PreprocessSuffix): physicalproperties.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/physicalproperties.cpp$(PreprocessSuffix) "physicalproperties.cpp"

$(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix): glcanvas.cpp $(IntermediateDirectory)/glcanvas.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/OpenCLSolarSystem/glcanvas.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/glcanvas.cpp$(DependSuffix): glcanvas.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/glcanvas.cpp$(DependSuffix) -MM "glcanvas.cpp"

$(IntermediateDirectory)/glcanvas.cpp$(PreprocessSuffix): glcanvas.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/glcanvas.cpp$(PreprocessSuffix) "glcanvas.cpp"

$(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix): clmodel.cpp $(IntermediateDirectory)/clmodel.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/OpenCLSolarSystem/clmodel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/clmodel.cpp$(DependSuffix): clmodel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/clmodel.cpp$(DependSuffix) -MM "clmodel.cpp"

$(IntermediateDirectory)/clmodel.cpp$(PreprocessSuffix): clmodel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/clmodel.cpp$(PreprocessSuffix) "clmodel.cpp"

$(IntermediateDirectory)/frame.cpp$(ObjectSuffix): frame.cpp $(IntermediateDirectory)/frame.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/OpenCLSolarSystem/frame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/frame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/frame.cpp$(DependSuffix): frame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/frame.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/frame.cpp$(DependSuffix) -MM "frame.cpp"

$(IntermediateDirectory)/frame.cpp$(PreprocessSuffix): frame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/frame.cpp$(PreprocessSuffix) "frame.cpp"

$(IntermediateDirectory)/application.cpp$(ObjectSuffix): application.cpp $(IntermediateDirectory)/application.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/OpenCLSolarSystem/application.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/application.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/application.cpp$(DependSuffix): application.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/application.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/application.cpp$(DependSuffix) -MM "application.cpp"

$(IntermediateDirectory)/application.cpp$(PreprocessSuffix): application.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/application.cpp$(PreprocessSuffix) "application.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./$(ConfigurationName)/


