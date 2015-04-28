.PHONY: clean All

All:
	@echo "----------Building project:[ OpenCLSolarSystem - Debug_x64 ]----------"
	@cd "OpenCLSolarSystem" && $(MAKE) -f  "OpenCLSolarSystem.mk" && $(MAKE) -f  "OpenCLSolarSystem.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ OpenCLSolarSystem - Debug_x64 ]----------"
	@cd "OpenCLSolarSystem" && $(MAKE) -f  "OpenCLSolarSystem.mk" clean
