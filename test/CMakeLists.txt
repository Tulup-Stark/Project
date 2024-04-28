project(Google_tests)

if (WIN32)
    configure_file(../config_win.json config.json COPYONLY)
else()
    configure_file(../config_lin.json config.json COPYONLY) 
endif(WIN32)
configure_file(../requests.json requests.json COPYONLY)
configure_file(../libstdc++-6.dll libstdc++-6.dll COPYONLY)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/database)

set(JSON_BuildTests OFF CACHE INTERNAL "")

include(FetchContent)

FetchContent_Declare(
        googletest
        URL
        https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
        GIT_TAG v3.11.2
        )

include_directories(
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ${INCLUDE_DIR}
)

set(gtest_disable_pthreads on)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest json)
enable_testing()



add_executable(Google_Tests_run
        ${INCLUDE_DIR}/converterjson.h ${SOURCE_DIR}/converterjson.cpp
        ${INCLUDE_DIR}/invertedindex.h ${SOURCE_DIR}/invertedindex.cpp
        ${INCLUDE_DIR}/entry.h
        ${INCLUDE_DIR}/searchserver.h ${SOURCE_DIR}/searchserver.cpp
        ${INCLUDE_DIR}/relativeindex.h
        invertedindextests.cpp searchservertests.cpp
        )

include(GoogleTest)
gtest_discover_tests(Google_Tests_run)

target_link_libraries(Google_Tests_run PUBLIC gtest gtest_main nlohmann_json::nlohmann_json)
