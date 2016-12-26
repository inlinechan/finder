add_subdirectory(googletest-release-1.8.0)
add_subdirectory(basic)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/src)
add_definitions(-DTEST_BASEDIR=${CMAKE_CURRENT_SOURCE_DIR})

add_executable(runUnitTests test.cpp IndexerTest.cpp)
target_link_libraries(runUnitTests gtest gtest_main gmock Indexer)
add_test(NAME helloTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/runUnitTests)
