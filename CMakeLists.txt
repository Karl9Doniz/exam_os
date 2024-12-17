cmake_minimum_required(VERSION 3.10)
project(MyMPIFramework)

set(CMAKE_CXX_STANDARD 17)

add_library(mympi_lib
    src/mympi.cpp
    src/mympi.h
)

target_include_directories(mympi_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(mympi_lib PUBLIC pthread)

function(create_mpi_executable name)
    add_executable(${name} src/${name}.cpp)
    target_link_libraries(${name} PRIVATE mympi_lib)
endfunction()

create_mpi_executable(producer_consumer)
create_mpi_executable(circle)

option(BUILD_PRODUCER_CONSUMER "Build producer-consumer example" ON)
option(BUILD_CIRCLE "Build circle communication example" ON)

if(NOT BUILD_PRODUCER_CONSUMER)
    set_target_properties(producer_consumer PROPERTIES EXCLUDE_FROM_ALL 1)
endif()

if(NOT BUILD_CIRCLE)
    set_target_properties(circle PROPERTIES EXCLUDE_FROM_ALL 1)
endif()
