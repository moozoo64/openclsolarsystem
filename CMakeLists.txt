cmake_minimum_required(VERSION 3.15)
project(OpenSolarSystem VERSION 1.0)

# Add compiler flag for aligned new support
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-faligned-new)
endif()

# Add subprojects
add_subdirectory(src/OpenCLSolarSystem)
add_subdirectory(src/OrbToSlf)