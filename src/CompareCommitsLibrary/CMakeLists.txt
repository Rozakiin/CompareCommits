project(CompareCommitsLibrary VERSION 1.0)

#print_variables()

set(HEADERS
    CompareCommits.hpp
    SimilarityMatrix.hpp
)

set(SOURCES
    CompareCommits.cpp
    SimilarityMatrix.cpp
)

add_library(${PROJECT_NAME} STATIC 
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    git2
)
# enable compiler options for msvc, gccc, and clang
target_compile_options(${PROJECT_NAME} PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Werror
          -Wall
          -pedantic-errors
          -Wextra
          -Wconversion
          -Wsign-conversion
          >
     $<$<CXX_COMPILER_ID:MSVC>:
          /WX
          /W4
          >)