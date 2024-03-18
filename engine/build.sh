#!/bin/bash
# Build script for engine
set echo on

mkdir -p ../build_linux

# Get a list of all the .c files.
cFilenames=$(find . -type f -name "*.c")

# echo "Files:" $cFilenames

assembly="engine"
compilerFlags="-g -shared -fdeclspec -fPIC"
# -fms-extensions 
# -Wall -Werror
includeFlags="-Isrc -I$VULKAN_SDK/include"
linkerFlags="-lvulkan -lxcb -lxkbcommon -L$VULKAN_SDK/lib"
defines="-D_DEBUG -DFEXPORT"

echo "Building $assembly..."
clang $cFilenames $compilerFlags -o ../build_linux/lib$assembly.so $defines $includeFlags $linkerFlags
