#!/bin/bash

# Check if build directory argument is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <build-directory>"
    echo "Example: $0 build-debug"
    exit 1
fi

BUILD_DIR="$1"

# Change to the bin directory under the specified build directory
cd "$BUILD_DIR/bin" || {
    echo "Error: Could not change to directory $BUILD_DIR/bin"
    exit 1
}

# Copy dependencies
# for dll in $(ldd OpenCLSolarSystem.exe | grep -E "ucrt64|clang64|glew" | awk '{print $3}'); do
#     echo "Copying $dll"
#     cp "$dll" .
# done

# echo "Dependencies copied successfully to $BUILD_DIR/bin"