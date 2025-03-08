cmake_minimum_required(VERSION 3.10)
project(OpenCLSolarSystem)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH "x64")
else()
    set(ARCH "x86")
endif()

# Compiler flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -ggdb -DwxDEBUG_LEVEL=1")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DwxDEBUG_LEVEL=0 -s")
endif()

# Find required packages
find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(GLEW REQUIRED)
find_package(wxWidgets REQUIRED xrc webview html adv core xml net base gl)

# Include directories
include_directories(
    ${wxWidgets_INCLUDE_DIRS}
    ${OPENCL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    application.cpp
    frame.cpp
    physicalproperties.cpp 
    initialstate.cpp
    glcanvas.cpp
    clmodel.cpp
    global.cpp
)

# Create executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${wxWidgets_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${OpenCL_LIBRARIES}
    ${GLEW_LIBRARIES}
)

# Post-build commands
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/adamsfma.cl $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/mpcsmall.slf $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:wxWidgets::base>
        $<TARGET_FILE:wxWidgets::gl>
        $<TARGET_FILE:wxWidgets::core>
        $<TARGET_FILE:GLEW::GLEW>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)