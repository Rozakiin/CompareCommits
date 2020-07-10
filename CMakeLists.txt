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
project(CompareCommits VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_IMDEPENDENT_CODE ON)
set(CMAKE_VERBOSE_MAKEFILE TRUE)

include(cmake/PrintVariables.cmake)

print_variables()

enable_testing()

# Build googletest
add_subdirectory(lib/googletest)

# Set libgit2 options (e.g. disable shared lib) TODO
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Build libgit2
add_subdirectory(lib/libgit2)

# add includes
include_directories(lib/libgit2/include)
include_directories(src)

# Build CompareCommits Library
add_subdirectory(src/CompareCommitsLibrary)

# Set Boost options
#set(BOOST_LIBRARYDIR ${CMAKE_SOURCE_DIR}/lib/boost/stage/lib)
set(BOOST_ROOT lib/boost)
set(BOOST_LIBRARYDIR lib/boost/stage/lib)
#set(Boost_DEBUG ON)  #<---------- Real life saver

# linking of Boost
set(Boost_USE_STATIC_LIBS        ON) #on works?
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(BOOST_ALL_DYN_LINK           OFF)   # force dynamic linking for all libraries

# Find Boost
find_package(Boost 1.73 REQUIRED COMPONENTS program_options)
message(status "** Boost Include: ${Boost_INCLUDE_DIR}")
message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
message(status "** Boost Libraries: ${Boost_LIBRARIES}")

# Build CompareCommits Program
add_subdirectory(src/CompareCommitsProgram)

# Build tests
add_subdirectory(tests)
