if (WIN32)
    cmake_minimum_required(VERSION 3.23)
else ()
    cmake_minimum_required(VERSION 3.22) #unix
endif (WIN32)

project(search_engine VERSION 0.2)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
        GIT_TAG v3.11.2
        )

FetchContent_MakeAvailable(json)

set(JSON_BuildTests OFF CACHE INTERNAL "")

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)
add_subdirectory(tests)

target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)
include(GoogleTest)
gtest_discover_tests(Google_Tests_run)

target_link_libraries(Google_Tests_run PUBLIC gtest gtest_main nlohmann_json::nlohmann_json)
