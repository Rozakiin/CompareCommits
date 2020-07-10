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