cmake_minimum_required(VERSION 3.10)
project(WhisperNet)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
add_executable(WhisperNet src/main.cpp)

# Link Protobuf
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(WhisperNet ${Protobuf_LIBRARIES})
