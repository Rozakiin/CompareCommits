project(CompareCommitsProgram VERSION 1.0)

#print_variables()

set(HEADERS
    
)

set(SOURCES
    main.cpp
)

#add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
#find_package(Boost 1.73.0 REQUIRED COMPONENTS program_options)
message("Boost include directory is ${Boost_INCLUDE_DIR}")
message("Boost library directory is ${Boost_LIBRARY_DIRS}")
message("Boost libs are ${Boost_LIBRARIES}")

if (Boost_FOUND)

    #add_definitions(-DBOOST_ALL_NO_LIB)  # tell the compiler to undefine this boost macro
    include_directories(SYSTEM ${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIRS})

    add_executable(${PROJECT_NAME} 
        ${HEADERS}
        ${SOURCES}
    )

    target_link_libraries(${PROJECT_NAME} PUBLIC
        CompareCommitsLibrary
        ${Boost_LIBRARIES}
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
            >
    )
endif()