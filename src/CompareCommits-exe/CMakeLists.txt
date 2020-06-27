project(CompareCommitsProgram VERSION 1.0)

#print_variables()

add_executable(CompareCommitsMain main.cpp)

target_link_libraries(CompareCommitsMain PRIVATE CompareCommits)