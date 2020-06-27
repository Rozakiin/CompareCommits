project(CompareCommitsTests VERSION 1.0)

#print_variables()

set(SOURCES
    CompareCommitsTests.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    gtest_main
    CompareCommitsLibrary
)

add_test(
    NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
)

