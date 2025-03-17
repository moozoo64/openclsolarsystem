cmake_minimum_required(VERSION 3.15)
project(OrbToSlf)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Validate build type
if(NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release)$")
    message(FATAL_ERROR "Invalid BUILD_TYPE '${CMAKE_BUILD_TYPE}'. Must be Debug or Release")
endif()

# Find dotnet SDK
find_program(DOTNET_EXECUTABLE dotnet)
if(NOT DOTNET_EXECUTABLE)
    message(FATAL_ERROR "dotnet SDK not found!")
endif()

# Define solution path variable
set(SOLUTION_PATH "${CMAKE_CURRENT_SOURCE_DIR}/OrbToSlfConsole/OrbToSlfConsole.csproj")

# Define output paths
set(OUTPUT_PATH_NET8 "${CMAKE_BINARY_DIR}/bin/OrbToSlf")

# Custom target for .NET build
add_custom_target(OrbToSlf ALL
    COMMAND ${DOTNET_EXECUTABLE} build --configuration ${CMAKE_BUILD_TYPE} --framework net8.0 --output "${OUTPUT_PATH_NET8}" "${SOLUTION_PATH}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building OrbToSlf for .NET 8.0"
    VERBATIM
)

# Custom target for clean
add_custom_target(clean_orbtoslf
    COMMAND ${DOTNET_EXECUTABLE} clean --configuration ${CMAKE_BUILD_TYPE} --framework net8.0 "${SOLUTION_PATH}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${OUTPUT_PATH_NET8}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Cleaning OrbToSlf"
    VERBATIM
)