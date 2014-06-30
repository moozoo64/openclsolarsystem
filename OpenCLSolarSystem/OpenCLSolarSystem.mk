##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=OpenCLSolarSystem
ConfigurationName      :=Release
WorkspacePath          := "C:\SDK\OpenCLSolarSystem\trunk"
ProjectPath            := "C:\SDK\OpenCLSolarSystem\trunk\OpenCLSolarSystem"
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Michael
Date                   :=06/30/14
CodeLitePath           :="C:\Program Files (x86)\CodeLite"
LinkerName             :=C:\MinGW-4.8.1\bin\g++.exe 
SharedObjectLinkerName :=C:\MinGW-4.8.1\bin\g++.exe -shared -fPIC
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
RcCompilerName         :=C:\MinGW-4.8.1\bin\windres.exe 
LinkOptions            :=  -mwindows -s $(shell wx-config --debug=no --libs --unicode=yes --libs gl)
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)/SDK/Khronos/opencl1.2/include $(IncludeSwitch)/SDK/glew-1.10.0/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)OpenCL $(LibrarySwitch)glew32 
ArLibs                 :=  "OpenCL" "glew32" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)/SDK/Khronos/opencl1.2/lib/ $(LibraryPathSwitch)/SDK/glew-1.10.0/lib 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:\MinGW-4.8.1\bin\ar.exe rcu
CXX      := C:\MinGW-4.8.1\bin\g++.exe 
CC       := C:\MinGW-4.8.1\bin\gcc.exe 
CXXFLAGS :=  -O2 -Wall -DwxDEBUG_LEVEL=0 $(shell wx-config --cxxflags --unicode=yes --debug=no) $(Preprocessors)
CFLAGS   :=  -O2 -Wall -DwxDEBUG_LEVEL=0 $(shell wx-config --cxxflags --unicode=yes --debug=no) $(Preprocessors)
ASFLAGS  := 
AS       := C:\MinGW-4.8.1\bin\as.exe 


##
## User defined environment variables
##
CodeLiteDir:=C:\SDK\CodeLite
WXWIN:=C:\wxWidgets-3.0.0
PATH:=$(WXWIN)\lib\gcc_dll;C:\SDK\glew-1.10.0\bin;$(PATH)
WXCFG:=gcc_dll\mswu
UNIT_TEST_PP_SRC_DIR:=C:\SDK\CodeLite\UnitTest++-1.3
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

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Release"

PreBuild:
	@echo Executing Pre Build commands ...
	copy adamsfma.cl $(ConfigurationName)
	copy astrorbsolexsmall.slf $(ConfigurationName)
	@echo Done


##
## Objects
##
$(IntermediateDirectory)/global.cpp$(ObjectSuffix): global.cpp $(IntermediateDirectory)/global.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/trunk/OpenCLSolarSystem/global.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/global.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/global.cpp$(DependSuffix): global.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/global.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/global.cpp$(DependSuffix) -MM "global.cpp"

$(IntermediateDirectory)/global.cpp$(PreprocessSuffix): global.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/global.cpp$(PreprocessSuffix) "global.cpp"

$(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix): initialstate.cpp $(IntermediateDirectory)/initialstate.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/trunk/OpenCLSolarSystem/initialstate.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/initialstate.cpp$(DependSuffix): initialstate.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/initialstate.cpp$(DependSuffix) -MM "initialstate.cpp"

$(IntermediateDirectory)/initialstate.cpp$(PreprocessSuffix): initialstate.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/initialstate.cpp$(PreprocessSuffix) "initialstate.cpp"

$(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix): physicalproperties.cpp $(IntermediateDirectory)/physicalproperties.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/trunk/OpenCLSolarSystem/physicalproperties.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/physicalproperties.cpp$(DependSuffix): physicalproperties.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/physicalproperties.cpp$(DependSuffix) -MM "physicalproperties.cpp"

$(IntermediateDirectory)/physicalproperties.cpp$(PreprocessSuffix): physicalproperties.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/physicalproperties.cpp$(PreprocessSuffix) "physicalproperties.cpp"

$(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix): glcanvas.cpp $(IntermediateDirectory)/glcanvas.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/trunk/OpenCLSolarSystem/glcanvas.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/glcanvas.cpp$(DependSuffix): glcanvas.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/glcanvas.cpp$(DependSuffix) -MM "glcanvas.cpp"

$(IntermediateDirectory)/glcanvas.cpp$(PreprocessSuffix): glcanvas.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/glcanvas.cpp$(PreprocessSuffix) "glcanvas.cpp"

$(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix): clmodel.cpp $(IntermediateDirectory)/clmodel.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/trunk/OpenCLSolarSystem/clmodel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/clmodel.cpp$(DependSuffix): clmodel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/clmodel.cpp$(DependSuffix) -MM "clmodel.cpp"

$(IntermediateDirectory)/clmodel.cpp$(PreprocessSuffix): clmodel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/clmodel.cpp$(PreprocessSuffix) "clmodel.cpp"

$(IntermediateDirectory)/frame.cpp$(ObjectSuffix): frame.cpp $(IntermediateDirectory)/frame.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/trunk/OpenCLSolarSystem/frame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/frame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/frame.cpp$(DependSuffix): frame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/frame.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/frame.cpp$(DependSuffix) -MM "frame.cpp"

$(IntermediateDirectory)/frame.cpp$(PreprocessSuffix): frame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/frame.cpp$(PreprocessSuffix) "frame.cpp"

$(IntermediateDirectory)/application.cpp$(ObjectSuffix): application.cpp $(IntermediateDirectory)/application.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/SDK/OpenCLSolarSystem/trunk/OpenCLSolarSystem/application.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/application.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/application.cpp$(DependSuffix): application.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/application.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/application.cpp$(DependSuffix) -MM "application.cpp"

$(IntermediateDirectory)/application.cpp$(PreprocessSuffix): application.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/application.cpp$(PreprocessSuffix) "application.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/global.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/global.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/global.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/initialstate.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/initialstate.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/physicalproperties.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/physicalproperties.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/glcanvas.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/glcanvas.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/clmodel.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/clmodel.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/frame.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/frame.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/frame.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/application.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/application.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/application.cpp$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile).exe
	$(RM) "../.build-release/OpenCLSolarSystem"


