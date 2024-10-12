cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(wave_producer_valid)

set(WAVE_PRODUCER_DIR ../../../app/audio/src/main/cpp/cpp_layer/WaveProducer)

add_executable(${CMAKE_PROJECT_NAME} wave_producer_valid.cpp)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${WAVE_PRODUCER_DIR})
