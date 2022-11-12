#!/bin/bash

# Generate project inside ./build folder.
cmake \
    -S . \
    -B build \
    -DCMAKE_TOOLCHAIN_FILE="./cmake/arm-none-eabi.cmake" \
    -DNRF5_SDK_PATH="$SDK_ROOT" \
    -DNRF5_BOARD="pca10056" \
    -DNRF5_SOFTDEVICE_VARIANT="s140"