# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(OpenCLSolarSystem)

# Handle build type (Debug or Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build: Debug or Release" FORCE)
endif()
if(NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release)$")
    message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}. Must be Debug or Release")
endif()

# Set C++ standard to gnu++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)  # Enables GNU extensions, matching -std=gnu++11

# Find required libraries
find_package(wxWidgets REQUIRED COMPONENTS core base gl)
include(${wxWidgets_USE_FILE})
find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(GLEW REQUIRED)

# Set compiler flags based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g3 -ggdb)  # Debugging flags
else()
    add_compile_options(-O2 -DNDEBUG)   # Optimization and no-debug macro
endif()
add_compile_options(-Wall -faligned-new)  # Common flags
add_compile_definitions(CL_TARGET_OPENCL_VERSION=300)  # OpenCL version macro

# List source files
set(SOURCES
    application.cpp
    frame.cpp
    physicalproperties.cpp
    initialstate.cpp
    glcanvas.cpp
    clmodel.cpp
    global.cpp
)

# Define the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${wxWidgets_LIBRARIES}
    OpenGL::GL
    OpenGL::GLU
    OpenCL::OpenCL
    GLEW::GLEW
)

# Apply strip flag (-s) for Release builds with GNU compilers
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(${PROJECT_NAME} PRIVATE -s)
endif()

# Post-build commands: copy resource files to the build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/adamsfma.cl ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/mpcsmall.slf ${CMAKE_BINARY_DIR}
    COMMENT "Copying resource files to build directory"
)
