add_subdirectory(googletest-release-1.8.0)

include_directories(${gtest_SOURCE_DIR}/inlcude ${gtest_SOURCE_DIR})

add_executable(runUnitTests test.cpp)
target_link_libraries(runUnitTests gtest gtest_main)
add_test(NAME helloTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/runUnitTests)
