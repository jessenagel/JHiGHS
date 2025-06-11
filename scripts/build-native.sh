#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Create build directory
mkdir -p "$PROJECT_ROOT/build"
cd "$PROJECT_ROOT/build"

# Configure and build
cmake -DCMAKE_BUILD_TYPE=Release "$PROJECT_ROOT"
make -j$(nproc)

echo "Native libraries built successfully"