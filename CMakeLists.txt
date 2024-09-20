cmake_minimum_required(VERSION 3.10)
project(WhisperNet)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Add source files
file(GLOB SOURCES "src/*.cpp" "src/auth/*.cpp")

# Create the executable
add_executable(whispernet ${SOURCES})

# Link OpenSSL libraries to the executable
target_link_libraries(whispernet OpenSSL::SSL OpenSSL::Crypto)
