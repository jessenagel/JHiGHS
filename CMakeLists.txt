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

target_link_libraries(jhighs
    highs
    ${JNI_LIBRARIES}
)

# Set output directory based on platform
if(WIN32)
    set_target_properties(jhighs PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src/main/resources/natives/windows-x86_64
    )
elseif(APPLE)
    set_target_properties(jhighs PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src/main/resources/natives/darwin-x86_64
    )
else()
    set_target_properties(jhighs PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src/main/resources/natives/linux-x86_64
    )
endif()