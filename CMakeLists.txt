# CMake build script for the CompareCommits project TODO
#
# Building (out of source build):
# > mkdir build && cd build
# > cmake .. [-DSETTINGS=VALUE]
# > cmake --build .
#
# Testing:
# > ctest -V
#
# Install:
# > cmake --build . --target install
cmake_minimum_required(VERSION 3.16)
project(CompareCommitsTopProject VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_IMDEPENDENT_CODE ON)

include(PrintVariables.cmake)

#print_variables()

enable_testing()

# Build googletest
add_subdirectory(lib/googletest)

# Set libgit2 options (e.g. disable shared lib)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Build libgit2
add_subdirectory(lib/libgit2)

add_subdirectory(src/CompareCommits)

add_subdirectory(tests)
