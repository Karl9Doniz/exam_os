cmake_minimum_required(VERSION 3.10)
project(MyMPI VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main library
add_library(mympi_lib
    src/mympi.cpp
)
target_include_directories(mympi_lib
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

add_executable(launcher
    src/launcher.cpp
)

add_executable(simple_test
    examples/simple_test.cpp
)
target_link_libraries(simple_test
    PRIVATE mympi_lib
)
