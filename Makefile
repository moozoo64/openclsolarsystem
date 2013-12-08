.PHONY: clean All

All:
	@echo "----------Building project:[ OpenCLSolarSystem - Release ]----------"
	@cd "OpenCLSolarSystem" && $(MAKE) -f  "OpenCLSolarSystem.mk" PreBuild && $(MAKE) -f  "OpenCLSolarSystem.mk"
clean:
	@echo "----------Cleaning project:[ OpenCLSolarSystem - Release ]----------"
	@cd "OpenCLSolarSystem" && $(MAKE) -f  "OpenCLSolarSystem.mk" clean
