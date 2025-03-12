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
    DebugLevel   := 
    StripFlag    := 
else
    OptimizeFlag := -O2
    DebugLevel   := -DNDEBUG
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
LinkOptions            := $(StripFlag) $(shell wx-config --libs core,base,gl --unicode=yes) -lopengl32 -lglu32
IncludePath            := $(IncludeSwitch).
Libs                   := $(LibrarySwitch)OpenCL $(LibrarySwitch)glew32 
ArLibs                 :=  "OpenCL.dll" "glew32" 
LibPath                := $(LibraryPathSwitch).

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := ar -r
CXX      := c++
CC       := gcc
CXXFLAGS := -std=gnu++17 -faligned-new $(OptimizeFlag) -Wall $(DebugLevel) $(shell wx-config --cxxflags --unicode=yes) -DCL_TARGET_OPENCL_VERSION=300
CFLAGS   := -std=gnu++17 $(OptimizeFlag) -Wall $(DebugLevel) $(shell wx-config --cxxflags --unicode=yes)
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
	cp adamsfma.cl $(ConfigurationName)
	cp mpcsmall.slf $(ConfigurationName)
	@for dll in $$(ldd "$(ConfigurationName)/OpenCLSolarSystem.exe" | grep -E "ucrt64|glew" | awk '{print $$3}'); do \
			echo "Copying $$dll"; \
			cp "$$dll" $(ConfigurationName); \
	done
	@echo Cleaning object files...
	@rm -f $(ConfigurationName)/*.o
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

