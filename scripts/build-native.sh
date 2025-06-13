#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

# Determine platform
OS=$(uname -s)
ARCH=$(uname -m)
case "$OS" in
    Linux*)
        PLATFORM="linux-x86_64"
        LIB_EXT="so"
        ;;
    Darwin*)
        PLATFORM="darwin-x86_64"
        LIB_EXT="dylib"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        PLATFORM="windows-x86_64"
        LIB_EXT="dll"
        ;;
    *)
        echo "Unsupported operating system: $OS"
        exit 1
        ;;
esac

echo "Building for platform: $PLATFORM"

# Create and clean build directory
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure and build with CMake
cmake -DCMAKE_BUILD_TYPE=Release "$PROJECT_ROOT"
make -j$(nproc || sysctl -n hw.ncpu || echo 4)

echo "Building native library..."
echo "$PWD"
# Generate JNI headers
javac -h . -cp "$PROJECT_ROOT/src/main/java" \
  "$PROJECT_ROOT/src/main/java/nl/jessenagel/jhighs/HiGHS.java" \
  "$PROJECT_ROOT/src/main/java/nl/jessenagel/jhighs/VarType.java" \
  "$PROJECT_ROOT/src/main/java/nl/jessenagel/jhighs/HighsStatus.java" \
  "$PROJECT_ROOT/src/main/java/nl/jessenagel/jhighs/ModelStatus.java" \
  "$PROJECT_ROOT/src/main/java/nl/jessenagel/jhighs/Solution.java"
echo "JNI headers generated successfully."

# Create output directory
OUTPUT_DIR="$PROJECT_ROOT/src/main/resources/dnatives/$PLATFORM"
mkdir -p "$OUTPUT_DIR"
echo "Output directory created: $OUTPUT_DIR"
# Platform-specific compilation and copying
case "$PLATFORM" in
    linux-*)
        echo "Compiling for Linux..."
        g++ -shared -fPIC -static-libgcc -static-libstdc++ \
            -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux \
            -I"$PROJECT_ROOT/src/main/native/third-party/HiGHS/highs" \
            -I"$PROJECT_ROOT/build/src/main/native/third-party/HiGHS" \
            -L"$PROJECT_ROOT/build/lib64" \
            -L"$PROJECT_ROOT/build/lib" \
            -Wl,-Bstatic -lhighs -Wl,-Bdynamic \
            "$PROJECT_ROOT/src/main/native/cpp/highs_jni.cpp" \
            -o libhighs_jni.so
        echo "Compilation complete."
        # Copy JNI library
        cp libhighs_jni.so "$OUTPUT_DIR/"


        # Copy HiGHS libraries
        if [[ -f "$PROJECT_ROOT/build/lib64/libhighs.so" ]]; then
            cp "$PROJECT_ROOT/build/lib64/libhighs.so" "$OUTPUT_DIR/"
        elif [[ -f "$PROJECT_ROOT/build/lib/libhighs.so" ]]; then
            cp "$PROJECT_ROOT/build/lib/libhighs.so" "$OUTPUT_DIR/"
        fi

        if [[ -f "$PROJECT_ROOT/build/lib64/libhighs.so.1" ]]; then
            cp "$PROJECT_ROOT/build/lib64/libhighs.so.1" "$OUTPUT_DIR/"
        elif [[ -f "$PROJECT_ROOT/build/lib/libhighs.so.1" ]]; then
            cp "$PROJECT_ROOT/build/lib/libhighs.so.1" "$OUTPUT_DIR/"
        fi

        if [[ -f "$PROJECT_ROOT/build/lib64/libhighs.so.1.11.0" ]]; then
            cp "$PROJECT_ROOT/build/lib64/libhighs.so.1.11.0" "$OUTPUT_DIR/"
        elif [[ -f "$PROJECT_ROOT/build/lib/libhighs.so.1.11.0" ]]; then
            cp "$PROJECT_ROOT/build/lib/libhighs.so.1.11.0" "$OUTPUT_DIR/"
        fi

        MAIN_LIB="libhighs_jni.so"
        ;;

    darwin-*)
        echo "Compiling for macOS..."
        g++ -shared -fPIC \
            -I${JAVA_HOME}/include -I${JAVA_HOME}/include/darwin \
            -I"$PROJECT_ROOT/src/main/native/third-party/HiGHS/highs" \
            -I"$PROJECT_ROOT/build/src/main/native/third-party/HiGHS" \
            -L"$PROJECT_ROOT/build/lib" \
            -lhighs "$PROJECT_ROOT/src/main/native/cpp/highs_jni.cpp" \
            -o libjhighs.dylib

        cp libjhighs.dylib "$OUTPUT_DIR/"
        if [[ -f "$PROJECT_ROOT/build/lib/libhighs.dylib" ]]; then
            cp "$PROJECT_ROOT/build/lib/libhighs.dylib" "$OUTPUT_DIR/"
        fi

        MAIN_LIB="libjhighs.dylib"
        ;;

    windows-*)
        echo "Compiling for Windows..."
        g++ -shared \
            -I"${JAVA_HOME}/include" -I"${JAVA_HOME}/include/win32" \
            -I"$PROJECT_ROOT/src/main/native/third-party/HiGHS/highs" \
            -I"$PROJECT_ROOT/build/src/main/native/third-party/HiGHS" \
            -L"$PROJECT_ROOT/build/lib" \
            -lhighs "$PROJECT_ROOT/src/main/native/cpp/highs_jni.cpp" \
            -o jhighs.dll

        cp jhighs.dll "$OUTPUT_DIR/"
        if [[ -f "$PROJECT_ROOT/build/lib/highs.dll" ]]; then
            cp "$PROJECT_ROOT/build/lib/highs.dll" "$OUTPUT_DIR/"
        fi

        MAIN_LIB="jhighs.dll"
        ;;
esac

# Verify the library was created
if [[ ! -f "$BUILD_DIR/$MAIN_LIB" ]]; then
    echo "Error: $MAIN_LIB not found in $BUILD_DIR"
    exit 1
fi

echo "Native library built successfully: $BUILD_DIR/$MAIN_LIB"
echo "Libraries copied to: $OUTPUT_DIR"

# List all files in output directory
echo "Files in output directory:"
ls -la "$OUTPUT_DIR"