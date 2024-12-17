cmake_minimum_required(VERSION 3.10)
project(MyMPIFramework)

set(CMAKE_CXX_STANDARD 17)

# Add source files
add_executable(my_mpi_program
    src/main.cpp
    src/mympi.cpp
    src/mympi.h
)

# Include directories
target_include_directories(my_mpi_program PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Link libraries if needed
target_link_libraries(my_mpi_program pthread)
