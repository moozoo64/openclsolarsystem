.PHONY: clean All

## Build configuration
##
BUILD_TYPE ?= Release

# Validate BUILD_TYPE
ifeq ($(BUILD_TYPE),$(filter $(BUILD_TYPE),Debug Release))
else
	$(error Invalid BUILD_TYPE "$(BUILD_TYPE)". Must be Debug or Release)
endif

All:
	@echo "==========Building project:[ OrbToSlf ]----------"
	@dotnet build --configuration $(BUILD_TYPE) --framework net8.0
	@dotnet build --configuration $(BUILD_TYPE) --framework net48
clean:
	@echo "==========Cleaning project:[ OrbToSlf]----------"
	@dotnet clean --configuration $(BUILD_TYPE) --framework net8.0
	@dotnet clean --configuration $(BUILD_TYPE) --framework net48