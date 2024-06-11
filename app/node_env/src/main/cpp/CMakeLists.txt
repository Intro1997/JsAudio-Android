# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html. For
# more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${
# PROJECT_NAME}, Since this is the top level CMakeLists.txt, the project name is
# also accessible with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync
# within the top level build script scope).
project("node_env")

set(NODE_SHARED_LIBRARY_PATH ${CPP_DEPENDENCY_PATH}/node/library/libnode.so)
set(NODE_INCLUDE_PATH ${CPP_DEPENDENCY_PATH}/node/include)
set(TOOL_INCLUDE_PATH ${CPP_DEPENDENCY_PATH}/tool/include)

set(NODE_ENV_INCLUDE ${PROJECT_SOURCE_DIR}/node_env)
set(ARGV_INCLUDE ${PROJECT_SOURCE_DIR}/argv)
set(NODE_LOGGER_INCLUDE ${PROJECT_SOURCE_DIR}/node_logger)
set(AUDIO_INCLUDE ${CPP_DEPENDENCY_PATH}/../../audio/src/main/cpp/audio)

set(NODE_ENV_SRC ${PROJECT_SOURCE_DIR}/node_env/NodeEnv.cpp)
set(ARGV_SRC ${PROJECT_SOURCE_DIR}/argv/Argv.cpp)
set(NODE_LOGGER_SRC ${PROJECT_SOURCE_DIR}/node_logger/node_logger.cpp)
set(AUDIO_SRC ${CPP_DEPENDENCY_PATH}/../../audio/src/main/cpp/audio/audio.cpp)

include_directories(
        ${NODE_INCLUDE_PATH}/node ${NODE_INCLUDE_PATH}/node/cppgc
        )

# Creates and names a library, sets it as either STATIC or SHARED, and provides
# the relative paths to its source code. You can define multiple libraries, and
# CMake builds them for you. Gradle automatically packages shared libraries with
# your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define the
# target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME} is
# preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here; for
# GameActivity/NativeActivity derived applications, the same library name must
# be used in the AndroidManifest.xml file.
add_library(
        ${CMAKE_PROJECT_NAME} SHARED
        ${ARGV_SRC}
        ${NODE_ENV_SRC}
        ${NODE_LOGGER_SRC}
        ${AUDIO_SRC}
        node_env_bridge.cpp
        )

# Specifies libraries CMake should link to your target library. You can link
# libraries from various origins, such as libraries defined in this build
# script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        ${NODE_SHARED_LIBRARY_PATH}
        android
        log
        )

target_include_directories(
        ${CMAKE_PROJECT_NAME} SYSTEM PUBLIC ${NODE_INCLUDE_PATH}
        )

target_include_directories(
        ${CMAKE_PROJECT_NAME}
        PUBLIC ${ARGV_INCLUDE}
               ${NODE_ENV_INCLUDE}
               ${TOOL_INCLUDE_PATH}
               ${NODE_LOGGER_INCLUDE}
               ${AUDIO_INCLUDE}
        )
