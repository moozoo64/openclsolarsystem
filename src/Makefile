BUILD_TYPE ?= Release
ARCH ?= x64

.PHONY: clean all

all:
	@echo "==========Building project:[ OpenCLSolarSystem - $(BUILD_TYPE)_$(ARCH) ]----------"
	@cd "OpenCLSolarSystem" && "$(MAKE)" BUILD_TYPE=$(BUILD_TYPE) ARCH=$(ARCH)
	@cd "OrbToSlf" && "$(MAKE)" BUILD_TYPE=$(BUILD_TYPE)

clean:
	@echo "==========Cleaning project:[ OpenCLSolarSystem - $(BUILD_TYPE)_$(ARCH) ]----------"
	@cd "OpenCLSolarSystem" && "$(MAKE)" clean BUILD_TYPE=$(BUILD_TYPE) ARCH=$(ARCH)
	@cd "OrbToSlf" && "$(MAKE)" clean BUILD_TYPE=$(BUILD_TYPE)