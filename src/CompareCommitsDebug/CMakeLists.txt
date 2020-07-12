project(CompareCommitsDebug VERSION 1.0)

set(HEADERS
    
)

set(SOURCES
    main.cpp
)

add_executable(${PROJECT_NAME} 
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    CompareCommitsLibrary
)
