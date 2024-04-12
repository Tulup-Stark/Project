if (WIN32)
    cmake_minimum_required(VERSION 3.23)
else ()
    cmake_minimum_required(VERSION 3.22) 
endif (WIN32)

project(search_engine)

if (WIN32)
    configure_file(../config_win.json config.json COPYONLY)
else()
    configure_file(../config_lin.json config.json COPYONLY) 
endif(WIN32)

configure_file(../requests.json requests.json COPYONLY)
configure_file(../libstdc++-6.dll libstdc++-6.dll COPYONLY)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/src/database)

configure_file(../database/file001.txt database/file001.txt COPYONLY)
configure_file(../database/file002.txt database/file002.txt COPYONLY)
configure_file(../database/file003.txt database/file003.txt COPYONLY)
configure_file(../database/file004.txt database/file004.txt COPYONLY)

set(CMAKE_CXX_STANDARD 17)
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_executable(search_engine main.cpp
        ${INCLUDE_DIR}/converterjson.h converterjson.cpp
        ${INCLUDE_DIR}/invertedindex.h invertedindex.cpp
        ${INCLUDE_DIR}/entry.h
        ${INCLUDE_DIR}/searchserver.h searchserver.cpp
        ${INCLUDE_DIR}/relativeindex.h
    )

target_include_directories(search_engine PUBLIC ${INCLUDE_DIR})
