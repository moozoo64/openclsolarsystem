##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=OpenCLSolarSystem
ConfigurationName      :=Release
WorkspacePath          := "/home/michael/Documents/CodeLite/OpenCLSolarSystem"
ProjectPath            := "/home/michael/Documents/CodeLite/OpenCLSolarSystem/OpenCLSolarSystem"
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Michael Simmons
Date                   :=12/08/13
CodeLitePath           :="/home/michael/.codelite"
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
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
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -mwindows -s $(shell wx-config --debug=no --libs --unicode=yes --libs gl)
IncludePath            :=  $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)OpenCL $(LibrarySwitch)GL $(LibrarySwitch)GLEW $(LibrarySwitch)GLU 
ArLibs                 :=  "OpenCL" "GL" "GLEW" "GLU" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := g++
CC       := gcc
CXXFLAGS :=  -O2 -Wall $(shell wx-config --cxxflags --unicode=yes --debug=no) $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(shell wx-config --cxxflags --unicode=yes --debug=no) $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/global$(ObjectSuffix) $(IntermediateDirectory)/initialstate$(ObjectSuffix) $(IntermediateDirectory)/physicalproperties$(ObjectSuffix) $(IntermediateDirectory)/glcanvas$(ObjectSuffix) $(IntermediateDirectory)/clmodel$(ObjectSuffix) $(IntermediateDirectory)/frame$(ObjectSuffix) $(IntermediateDirectory)/application$(ObjectSuffix) 



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
	@test -d ./Release || $(MakeDirCommand) ./Release

PreBuild:
	@echo Executing Pre Build commands ...
	cp adamsfma.cl $(ConfigurationName)
	cp astrorbsolexsmall.slf $(ConfigurationName)
	@echo Done


##
## Objects
##
$(IntermediateDirectory)/global$(ObjectSuffix): global.cpp $(IntermediateDirectory)/global$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/michael/Documents/CodeLite/OpenCLSolarSystem/OpenCLSolarSystem/global.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/global$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/global$(DependSuffix): global.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/global$(ObjectSuffix) -MF$(IntermediateDirectory)/global$(DependSuffix) -MM "global.cpp"

$(IntermediateDirectory)/global$(PreprocessSuffix): global.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/global$(PreprocessSuffix) "global.cpp"

$(IntermediateDirectory)/initialstate$(ObjectSuffix): initialstate.cpp $(IntermediateDirectory)/initialstate$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/michael/Documents/CodeLite/OpenCLSolarSystem/OpenCLSolarSystem/initialstate.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/initialstate$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/initialstate$(DependSuffix): initialstate.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/initialstate$(ObjectSuffix) -MF$(IntermediateDirectory)/initialstate$(DependSuffix) -MM "initialstate.cpp"

$(IntermediateDirectory)/initialstate$(PreprocessSuffix): initialstate.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/initialstate$(PreprocessSuffix) "initialstate.cpp"

$(IntermediateDirectory)/physicalproperties$(ObjectSuffix): physicalproperties.cpp $(IntermediateDirectory)/physicalproperties$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/michael/Documents/CodeLite/OpenCLSolarSystem/OpenCLSolarSystem/physicalproperties.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/physicalproperties$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/physicalproperties$(DependSuffix): physicalproperties.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/physicalproperties$(ObjectSuffix) -MF$(IntermediateDirectory)/physicalproperties$(DependSuffix) -MM "physicalproperties.cpp"

$(IntermediateDirectory)/physicalproperties$(PreprocessSuffix): physicalproperties.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/physicalproperties$(PreprocessSuffix) "physicalproperties.cpp"

$(IntermediateDirectory)/glcanvas$(ObjectSuffix): glcanvas.cpp $(IntermediateDirectory)/glcanvas$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/michael/Documents/CodeLite/OpenCLSolarSystem/OpenCLSolarSystem/glcanvas.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/glcanvas$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/glcanvas$(DependSuffix): glcanvas.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/glcanvas$(ObjectSuffix) -MF$(IntermediateDirectory)/glcanvas$(DependSuffix) -MM "glcanvas.cpp"

$(IntermediateDirectory)/glcanvas$(PreprocessSuffix): glcanvas.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/glcanvas$(PreprocessSuffix) "glcanvas.cpp"

$(IntermediateDirectory)/clmodel$(ObjectSuffix): clmodel.cpp $(IntermediateDirectory)/clmodel$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/michael/Documents/CodeLite/OpenCLSolarSystem/OpenCLSolarSystem/clmodel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/clmodel$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/clmodel$(DependSuffix): clmodel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/clmodel$(ObjectSuffix) -MF$(IntermediateDirectory)/clmodel$(DependSuffix) -MM "clmodel.cpp"

$(IntermediateDirectory)/clmodel$(PreprocessSuffix): clmodel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/clmodel$(PreprocessSuffix) "clmodel.cpp"

$(IntermediateDirectory)/frame$(ObjectSuffix): frame.cpp $(IntermediateDirectory)/frame$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/michael/Documents/CodeLite/OpenCLSolarSystem/OpenCLSolarSystem/frame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/frame$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/frame$(DependSuffix): frame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/frame$(ObjectSuffix) -MF$(IntermediateDirectory)/frame$(DependSuffix) -MM "frame.cpp"

$(IntermediateDirectory)/frame$(PreprocessSuffix): frame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/frame$(PreprocessSuffix) "frame.cpp"

$(IntermediateDirectory)/application$(ObjectSuffix): application.cpp $(IntermediateDirectory)/application$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/michael/Documents/CodeLite/OpenCLSolarSystem/OpenCLSolarSystem/application.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/application$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/application$(DependSuffix): application.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/application$(ObjectSuffix) -MF$(IntermediateDirectory)/application$(DependSuffix) -MM "application.cpp"

$(IntermediateDirectory)/application$(PreprocessSuffix): application.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/application$(PreprocessSuffix) "application.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/global$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/global$(DependSuffix)
	$(RM) $(IntermediateDirectory)/global$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/initialstate$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/initialstate$(DependSuffix)
	$(RM) $(IntermediateDirectory)/initialstate$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/physicalproperties$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/physicalproperties$(DependSuffix)
	$(RM) $(IntermediateDirectory)/physicalproperties$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/glcanvas$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/glcanvas$(DependSuffix)
	$(RM) $(IntermediateDirectory)/glcanvas$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/clmodel$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/clmodel$(DependSuffix)
	$(RM) $(IntermediateDirectory)/clmodel$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/frame$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/frame$(DependSuffix)
	$(RM) $(IntermediateDirectory)/frame$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/application$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/application$(DependSuffix)
	$(RM) $(IntermediateDirectory)/application$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) "../.build-release/OpenCLSolarSystem"


