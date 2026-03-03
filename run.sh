#!/bin/bash
set -e

BUILD_DIR="build"

cmake -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" --config Release

echo ""
echo "==== Running Tests ===="
cd "$BUILD_DIR"
ctest --build-config Release --verbose
cd ..

echo ""
echo "==== Running Benchmark ===="
"./$BUILD_DIR/fixed64_benchmark"
