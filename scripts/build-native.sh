#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
LIB_NAME="libhighs_jni.so"

# Create and clean build directory
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure and build
cmake -DCMAKE_BUILD_TYPE=Release "$PROJECT_ROOT"
make -j$(nproc)

echo "Building native library..."
javac -h . $PROJECT_ROOT/src/main/java/nl/jessenagel/jhighs/HiGHS.java  # Generate JNI headers
echo "JNI headers generated successfully."
echo "Compiling native code..."

g++ -shared -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -I$PROJECT_ROOT/src/main/native/third-party/HiGHS/highs \
    -I$PROJECT_ROOT/build/src/main/native/third-party/HiGHS \
    -L$PROJECT_ROOT/build/lib64 \
    -lhighs $PROJECT_ROOT/src/main/native/cpp/highs_jni.cpp -o libhighs_jni.so

cp libhighs_jni.so $PROJECT_ROOT/src/main/resources/natives/linux-x86_64/
cp $PROJECT_ROOT/build/lib64/libhighs.so $PROJECT_ROOT/src/main/resources/natives/linux-x86_64/
cp $PROJECT_ROOT/build/lib64/libhighs.so.1 $PROJECT_ROOT/src/main/resources/natives/linux-x86_64/
cp $PROJECT_ROOT/build/lib64/libhighs.so.1.11.0 $PROJECT_ROOT/src/main/resources/natives/linux-x86_64/

# Verify the library was created
if [[ ! -f "$BUILD_DIR/$LIB_NAME" ]]; then
    echo "Error: $LIB_NAME not found in $BUILD_DIR"
    exit 1
fi

echo "Native library built successfully: $BUILD_DIR/$LIB_NAME"