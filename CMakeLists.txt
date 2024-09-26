cmake_minimum_required(VERSION 3.10)
project(WhisperNet)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Find Protobuf
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

# Paths for Abseil (manually add Abseil headers and library paths)
set(ABSEIL_INCLUDE_DIR "/opt/homebrew/opt/abseil/include")
set(ABSEIL_LIBRARY_DIR "/opt/homebrew/opt/abseil/lib")

# Include the Abseil headers
include_directories(${ABSEIL_INCLUDE_DIR})

# Paths for the .proto files and generated files
set(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(GENERATED_PROTO_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${GENERATED_PROTO_DIR}")

# Specify the .proto file
set(PROTO_FILES "${PROTO_DIR}/whispernet.proto")

# Generate the .pb.cc and .pb.h files
add_custom_command(
    OUTPUT "${GENERATED_PROTO_DIR}/whispernet.pb.cc" "${GENERATED_PROTO_DIR}/whispernet.pb.h"
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
            --cpp_out="${GENERATED_PROTO_DIR}"
            --proto_path="${PROTO_DIR}"
            "${PROTO_FILES}"
    DEPENDS "${PROTO_FILES}"
    COMMENT "Running protoc on ${PROTO_FILES}"
)

# Create a custom target for the protobuf files
add_custom_target(proto_files
    DEPENDS "${GENERATED_PROTO_DIR}/whispernet.pb.cc" "${GENERATED_PROTO_DIR}/whispernet.pb.h"
)

# Include the directory with generated files
include_directories(${GENERATED_PROTO_DIR})

# Add source files
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")

# Add the generated protobuf source file to the sources
set(SOURCES ${SOURCES} "${GENERATED_PROTO_DIR}/whispernet.pb.cc")

# Create the executable
add_executable(whispernet ${SOURCES})

# Make the executable depend on the proto_files target
add_dependencies(whispernet proto_files)

# Link libraries to the executable
target_link_libraries(whispernet
    OpenSSL::SSL
    OpenSSL::Crypto
    ${Protobuf_LIBRARIES}
    pthread
    absl
)

# Manually specify where to find the Abseil libraries
link_directories(${ABSEIL_LIBRARY_DIR})
