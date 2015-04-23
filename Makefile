.PHONY: clean All

All:
	@echo "----------Building project:[ OpenCLSolarSystem - Release_x86_64 ]----------"
	@cd "OpenCLSolarSystem" && $(MAKE) -f  "OpenCLSolarSystem.mk" && $(MAKE) -f  "OpenCLSolarSystem.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ OpenCLSolarSystem - Release_x86_64 ]----------"
	@cd "OpenCLSolarSystem" && $(MAKE) -f  "OpenCLSolarSystem.mk" clean
