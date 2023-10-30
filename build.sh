#!/bin/bash

# Remove the 'build' directory if it exists
if [ -d "build" ]; then
    rm -r build
fi

# Create the 'build' directory
mkdir build

# Change to the 'build' directory
cd build

# Run CMake in the 'build' directory
cmake ..

# Build the project using 'make'
make

# Print command to execute the program
echo "To execute the program, change your current directory to /build/bin then run: ./Exercice0x -h"
