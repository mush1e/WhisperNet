cmake_minimum_required(VERSION 3.10)
project(WhisperNet)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Create the executable
add_executable(whispernet ${SOURCES})

# Link libraries if needed
# target_link_libraries(whispernet <libraries>)
