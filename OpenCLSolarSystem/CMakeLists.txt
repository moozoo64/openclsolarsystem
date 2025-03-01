cmake_minimum_required(VERSION 3.10)
project(OpenCLSolarSystem)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()

find_package(wxWidgets REQUIRED gl core base)
include(${wxWidgets_USE_FILE})

find_package(OpenCL REQUIRED)

include_directories(
    .
    "C:/SDK/OpenCL-Headers/opencl22"
    "C:/SDK/glew-2.1.0/include"
)

link_directories(
    "C:/SDK/glew-2.1.0/lib/Release/x64"
)

set(SOURCES
    physicalproperties.cpp
    initialstate.cpp
    global.cpp
    frame.cpp
    glcanvas.cpp
    application.cpp
    clmodel.cpp
)

add_executable(OpenCLSolarSystem ${SOURCES})

target_link_libraries(OpenCLSolarSystem
    ${wxWidgets_LIBRARIES}
    OpenCL::OpenCL
    glew32
)

add_custom_command(TARGET OpenCLSolarSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/adamsfma.cl"
        "$<TARGET_FILE_DIR:OpenCLSolarSystem>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/astrorbsolexsmall.slf"
        "$<TARGET_FILE_DIR:OpenCLSolarSystem>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/wxWidgets-3.1.1/lib/gcc_dllx86_64/wxbase311u_gcc_cl.dll"
        "$<TARGET_FILE_DIR:OpenCLSolarSystem>"
)