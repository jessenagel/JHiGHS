cmake_minimum_required(VERSION 3.15)
project(jhighs)

set(CMAKE_CXX_STANDARD 14)

# Find JNI
find_package(JNI REQUIRED)

# Add HiGHS as subdirectory
add_subdirectory(src/main/native/third-party/HiGHS)

# Create JNI library
add_library(jhighs SHARED
    src/main/native/cpp/highs_jni.cpp
    src/main/native/cpp/jni_utils.cpp
)

target_include_directories(jhighs PRIVATE
    ${JNI_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/main/native/headers
    ${CMAKE_SOURCE_DIR}/src/main/native/third-party/HiGHS/highs
)

# Platform-specific linking
if(APPLE)
    # On modern macOS, don't link JavaVM framework - JNI symbols are resolved at runtime
    target_link_libraries(jhighs
        highs
    )
    # Set proper linking flags for macOS
    set_target_properties(jhighs PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
else()
    # On other platforms, use regular JNI libraries
    target_link_libraries(jhighs
        highs
        ${JNI_LIBRARIES}
    )
endif()

message(STATUS "JNI_LIBRARIES: ${JNI_LIBRARIES}")

# Determine architecture
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
    set(ARCH_NAME "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64)$")
    set(ARCH_NAME "x86_64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(armv7l)$")
    set(ARCH_NAME "armv7")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i386|i686)$")
    set(ARCH_NAME "x86")
else()
    set(ARCH_NAME ${CMAKE_SYSTEM_PROCESSOR})
endif()

# Set output directory based on platform and architecture
if(WIN32)
    set_target_properties(jhighs PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src/main/resources/natives/windows-${ARCH_NAME}
    )
elseif(APPLE)
    set_target_properties(jhighs PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src/main/resources/natives/darwin-${ARCH_NAME}
    )
else()
    set_target_properties(jhighs PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src/main/resources/natives/linux-${ARCH_NAME}
    )
endif()
