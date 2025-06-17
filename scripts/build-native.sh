#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

# Determine platform and architecture
OS=$(uname -s)
ARCH=$(uname -m)

# Normalize architecture names
case "$ARCH" in
    x86_64|amd64)
        NORM_ARCH="x86_64"
        ;;
    arm64|aarch64)
        NORM_ARCH="arm64"
        ;;
    armv7l)
        NORM_ARCH="armv7"
        ;;
    i386|i686)
        NORM_ARCH="x86"
        ;;
    *)
        echo "Unsupported architecture: $ARCH"
        exit 1
        ;;
esac

case "$OS" in
    Linux*)
        PLATFORM="linux-$NORM_ARCH"
        LIB_EXT="so"
        ;;
    Darwin*)
        PLATFORM="darwin-$NORM_ARCH"
        LIB_EXT="dylib"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        PLATFORM="windows-$NORM_ARCH"
        LIB_EXT="dll"
        ;;
    *)
        echo "Unsupported operating system: $OS"
        exit 1
        ;;
esac

echo "Building for platform: $PLATFORM (OS: $OS, Arch: $ARCH -> $NORM_ARCH)"

# Create and clean build directory
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Generate JNI headers first
echo "Generating JNI headers..."
javac -h "$BUILD_DIR" -cp "$PROJECT_ROOT/src/main/java" \
  "$PROJECT_ROOT/src/main/java/nl/jessenagel/jhighs/HiGHS.java" \
  "$PROJECT_ROOT/src/main/java/nl/jessenagel/jhighs/VarType.java" \
  "$PROJECT_ROOT/src/main/java/nl/jessenagel/jhighs/HighsStatus.java" \
  "$PROJECT_ROOT/src/main/java/nl/jessenagel/jhighs/ModelStatus.java" \
  "$PROJECT_ROOT/src/main/java/nl/jessenagel/jhighs/Solution.java"
echo "JNI headers generated successfully."

# Configure and build with CMake
case "$OS" in
    Linux*)
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-fPIC" "$PROJECT_ROOT"
        ;;
    Darwin*)
        # For macOS, ensure JAVA_HOME is set correctly
        if [[ -z "$JAVA_HOME" ]]; then
            export JAVA_HOME=$(/usr/libexec/java_home)
            echo "JAVA_HOME set to: $JAVA_HOME"
        fi

        # Set architecture-specific flags for macOS
        if [[ "$NORM_ARCH" == "arm64" ]]; then
            CMAKE_ARCH_FLAGS="-DCMAKE_OSX_ARCHITECTURES=arm64"
        elif [[ "$NORM_ARCH" == "x86_64" ]]; then
            CMAKE_ARCH_FLAGS="-DCMAKE_OSX_ARCHITECTURES=x86_64"
        else
            CMAKE_ARCH_FLAGS=""
        fi

        cmake -DCMAKE_BUILD_TYPE=Release \
              -DCMAKE_CXX_FLAGS="-fPIC" \
              -DJAVA_HOME="$JAVA_HOME" \
              $CMAKE_ARCH_FLAGS \
              "$PROJECT_ROOT"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        # Windows architecture handling
        if [[ "$NORM_ARCH" == "x86_64" ]]; then
            cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release "$PROJECT_ROOT"
        else
            cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_GENERATOR_PLATFORM="$NORM_ARCH" "$PROJECT_ROOT"
        fi
        ;;
    *)
        echo "Unsupported platform: $OS"
        exit 1
        ;;
esac

# Build the project
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "Build completed successfully!"

# Create output directory if it doesn't exist
OUTPUT_DIR="$PROJECT_ROOT/src/main/resources/natives/$PLATFORM"
mkdir -p "$OUTPUT_DIR"

# Copy HiGHS libraries to output directory
echo "Copying HiGHS libraries..."

# Function to copy library files
copy_libs() {
    local lib_dir="$1"
    local lib_ext="$2"

    if [[ -d "$lib_dir" ]]; then
        echo "Checking for libraries in: $lib_dir"

        case "$OS" in
            Darwin*)
                # macOS - copy .dylib files
                if [[ -f "$lib_dir/libhighs.$lib_ext" ]]; then
                    cp "$lib_dir/libhighs.$lib_ext" "$OUTPUT_DIR/"
                    echo "Copied libhighs.$lib_ext"
                fi
                if [[ -f "$lib_dir/libhighs.1.$lib_ext" ]]; then
                    cp "$lib_dir/libhighs.1.$lib_ext" "$OUTPUT_DIR/"
                    echo "Copied libhighs.1.$lib_ext"
                fi
                if [[ -f "$lib_dir/libhighs.1.11.$lib_ext" ]]; then
                    cp "$lib_dir/libhighs.1.11.$lib_ext" "$OUTPUT_DIR/"
                    echo "Copied libhighs.1.11.$lib_ext"
                fi
                ;;
            Linux*)
                # Linux - copy .so files
                if [[ -f "$lib_dir/libhighs.$lib_ext" ]]; then
                    cp "$lib_dir/libhighs.$lib_ext" "$OUTPUT_DIR/"
                    echo "Copied libhighs.$lib_ext"
                fi
                if [[ -f "$lib_dir/libhighs.$lib_ext.1" ]]; then
                    cp "$lib_dir/libhighs.$lib_ext.1" "$OUTPUT_DIR/"
                    echo "Copied libhighs.$lib_ext.1"
                fi
                if [[ -f "$lib_dir/libhighs.$lib_ext.1.11.0" ]]; then
                    cp "$lib_dir/libhighs.$lib_ext.1.11.0" "$OUTPUT_DIR/"
                    echo "Copied libhighs.$lib_ext.1.11.0"
                fi
                ;;
            MINGW*|MSYS*|CYGWIN*)
                # Windows - copy .dll files
                if [[ -f "$lib_dir/highs.$lib_ext" ]]; then
                    cp "$lib_dir/highs.$lib_ext" "$OUTPUT_DIR/"
                    echo "Copied highs.$lib_ext"
                fi
                if [[ -f "$lib_dir/libhighs.$lib_ext" ]]; then
                    cp "$lib_dir/libhighs.$lib_ext" "$OUTPUT_DIR/"
                    echo "Copied libhighs.$lib_ext"
                fi
                ;;
        esac
    fi
}

# Try different library directories based on architecture
case "$NORM_ARCH" in
    x86_64|amd64)
        # Try lib64 first, then lib
        copy_libs "$PROJECT_ROOT/build/lib64" "$LIB_EXT"
        copy_libs "$PROJECT_ROOT/build/lib" "$LIB_EXT"
        ;;
    *)
        # For other architectures, try lib first, then lib64
        copy_libs "$PROJECT_ROOT/build/lib" "$LIB_EXT"
        copy_libs "$PROJECT_ROOT/build/lib64" "$LIB_EXT"
        ;;
esac

# Verify the library was created
case "$OS" in
    Linux*)
        MAIN_LIB="libjhighs.so"
        ;;
    Darwin*)
        MAIN_LIB="libjhighs.dylib"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        MAIN_LIB="jhighs.dll"
        ;;
esac

if [[ ! -f "$OUTPUT_DIR/$MAIN_LIB" ]]; then
    echo "Error: $MAIN_LIB not found in $OUTPUT_DIR"
    echo "Contents of $OUTPUT_DIR:"
    ls -la "$OUTPUT_DIR" 2>/dev/null || echo "Directory does not exist"
    exit 1
fi

echo "Native library built successfully: $OUTPUT_DIR/$MAIN_LIB"

# List all files in output directory
echo "Files in output directory:"
ls -la "$OUTPUT_DIR"
