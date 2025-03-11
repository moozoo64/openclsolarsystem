## Copyright 2013-2025 Michael William Simmons (with GitHub Copilot help)  
##
## Build configuration
##
BUILD_TYPE ?= Release
ARCH ?= x64

# Validate BUILD_TYPE
ifeq ($(BUILD_TYPE),$(filter $(BUILD_TYPE),Debug Release))
else
	$(error Invalid BUILD_TYPE "$(BUILD_TYPE)". Must be Debug or Release)
endif

# Validate ARCH
ifeq ($(ARCH),$(filter $(ARCH),x86 x64))
else
	$(error Invalid ARCH "$(ARCH)". Must be x86 or x64)
endif

# Set configuration name from BUILD_TYPE and ARCH
ConfigurationName := $(BUILD_TYPE)_$(ARCH)

# Set build flags based on configuration
ifeq ($(BUILD_TYPE),Debug)
    OptimizeFlag := -O0 -g3 -ggdb
    DebugLevel   := -DwxDEBUG_LEVEL=1
    wxDebug      := yes
    StripFlag    := 
else
    OptimizeFlag := -O2
    DebugLevel   := -DwxDEBUG_LEVEL=0
    wxDebug	 := no
    StripFlag    := -s
endif

ProjectName            :=OpenCLSolarSystem
IntermediateDirectory  :=./$(ConfigurationName)
OutputDirectory        :=$(IntermediateDirectory)
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
LinkerName             :=c++
ObjectSuffix           :=.o
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
SourceSwitch           :=-c 
ObjectSwitch           :=-o 
ObjectsFileList        :="OpenCLSolarSystem.txt"
MakeDirCommand         :="mkdir" -p
RMDirCommand           :="rm" -f -r
RcCompilerName         :=windres.exe
LinkOptions            := $(StripFlag) $(shell wx-config --debug=no --libs all --unicode=yes --libs gl) -lopengl32 -lglu32
IncludePath            := $(IncludeSwitch). $(IncludeSwitch)"$(OPENCL_INCLUDE)" $(IncludeSwitch)"$(GLEW_INCLUDE)"
Libs                   := $(LibrarySwitch)OpenCL $(LibrarySwitch)glew32 
ArLibs                 :=  "OpenCL.dll" "glew32" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)$(OPENCL_LIB64) $(LibraryPathSwitch)$(GLEWLIB64) 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := ar -r
CXX      := c++
CC       := gcc
CXXFLAGS := -std=gnu++11 $(OptimizeFlag) -Wall $(shell wx-config --cxxflags --unicode=yes --debug=$(wxDebug)) -DCL_TARGET_OPENCL_VERSION=300
CFLAGS   := -std=gnu++11 $(OptimizeFlag) -Wall $(shell wx-config --cxxflags --unicode=yes --debug=$(wxDebug))
AS       := as

##
## User defined environment variables
##

Objects0=$(IntermediateDirectory)/application.cpp$(ObjectSuffix) $(IntermediateDirectory)/frame.cpp$(ObjectSuffix) $(IntermediateDirectory)/physicalproperties.cpp$(ObjectSuffix) $(IntermediateDirectory)/initialstate.cpp$(ObjectSuffix) $(IntermediateDirectory)/glcanvas.cpp$(ObjectSuffix) $(IntermediateDirectory)/clmodel.cpp$(ObjectSuffix) $(IntermediateDirectory)/global.cpp$(ObjectSuffix) 
Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile) PostBuild

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	$(LinkerName) $(OutputSwitch)$(OutputFile) $(Objects) $(LibPath) $(Libs) $(LinkOptions)

PostBuild: $(OutputFile)
	@echo Executing Post Build commands ...
	copy adamsfma.cl $(ConfigurationName)
	copy mpcsmall.slf $(ConfigurationName)
	copy $(WXWIN)\lib\$(WXWIN64)\wxbase32u_gcc_cl.dll $(ConfigurationName)
	copy $(WXWIN)\lib\$(WXWIN64)\wxmsw32u_gl_gcc_cl.dll $(ConfigurationName)
	copy $(WXWIN)\lib\$(WXWIN64)\wxmsw32u_core_gcc_cl.dll $(ConfigurationName)
	copy $(GLEWBIN64)\glew32.dll $(ConfigurationName)
	copy $(Mingw64)\mingw64\bin\libgcc_s_seh-1.dll $(ConfigurationName)
	copy "$(Mingw64)\mingw64\bin\libstdc++-6.dll" $(ConfigurationName)
	copy $(Mingw64)\mingw64\bin\libwinpthread-1.dll $(ConfigurationName)
	@echo Done

MakeIntermediateDirs:
	@$(MakeDirCommand) "./$(ConfigurationName)"

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./$(ConfigurationName)"

PreBuild:

##
## Objects
##
$(IntermediateDirectory)/%.cpp$(ObjectSuffix): %.cpp
	$(CXX) $(SourceSwitch) "$<" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/$(@F) $(IncludePath)

##
## Clean
##
clean:
	$(RMDirCommand) $(ConfigurationName)

