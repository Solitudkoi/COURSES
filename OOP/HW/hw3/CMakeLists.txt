cmake_minimum_required(VERSION 3.10)
project(sketch)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add problem executable
add_executable(main
    src/main.cpp
    src/arc.cpp
    src/circle.cpp
    src/edge.cpp
    src/line.cpp
    src/sketch.cpp
    src/vertex.cpp
)

# Add include directories
target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Set output directory
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)