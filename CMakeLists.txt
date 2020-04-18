# cmake version to be used
cmake_minimum_required(VERSION 3.0)

set (CMAKE_CXX_STANDARD 11)

# project name
project(hmap)

# flags

# files

# include
include_directories(./include ./src) # ./libs/Logger/include ./libs/Randomize/include

# target
add_executable(test ./tests/main.cpp ./src/local_communicator.cpp)
add_executable(mpi-test ./tests/mpi_main.cpp)

# external libs
# DYNAMIC LIBRARY
#add_subdirectory(./libs/Logger)
#target_link_libraries(binary logger)

# STATIC LIBRARY
#add_library(randomize STATIC IMPORTED)
#set_property(TARGET randomize PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libs/Randomize/librandomize.a)
#target_link_libraries(binary randomize)
