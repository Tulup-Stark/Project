cmake_minimum_required (VERSION 3.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(SrcFiles
	${MY_SRC_DIR}/InvertedIndex.cpp
	${MY_SRC_DIR}/SearchEngine.cpp
	${MY_SRC_DIR}/ConverterJSON.cpp
)

message(STATUS "MY_SRC_DIR = " ${MY_SRC_DIR})
message(STATUS "MY_INCLUDE_DIR = " ${MY_INCLUDE_DIR})

set(TestsFiles
	InvertedIndex_test.cpp
	SearchEngine_test.cpp
	ConverterJSON_test.cpp
)

add_executable(GtestTests ${TestsFiles} ${SrcFiles})
target_link_libraries(GtestTests nlohmann_json::nlohmann_json GTest::gtest_main)
include(GoogleTest)
target_include_directories(GtestTests PUBLIC ${MY_INCLUDE_DIR})
gtest_discover_tests(GtestTests)
