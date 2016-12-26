cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME finder)
project(${PROJECT_NAME})

option(test "Build all tests." OFF)

execute_process(COMMAND llvm-config-3.8 --cxxflags
  OUTPUT_VARIABLE LLVM_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS "${LLVM_CXXFLAGS} -Wno-strict-aliasing")

execute_process(COMMAND llvm-config-3.8 --ldflags
  OUTPUT_VARIABLE LLVM_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND llvm-config-3.8 --libs
  OUTPUT_VARIABLE LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND llvm-config-3.8 --system-libs
  OUTPUT_VARIABLE LLVM_SYSTEM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CLANG_LIBS " -Wl,--start-group -lclangAST -lclangASTMatchers -lclangAnalysis -lclangBasic -lclangDriver -lclangEdit -lclangFrontend -lclangFrontendTool -lclangLex -lclangParse -lclangSema -lclangEdit -lclangRewrite -lclangRewriteFrontend -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangSerialization -lclangToolingCore -lclangTooling -lclangFormat -Wl,--end-group ")

set(CLANG_LINK_FLAGS
  "${CLANG_LIBS} ${LLVM_LDFLAGS} ${LLVM_LIBS} ${LLVM_SYSTEM_LIBS}")

set(CMAKE_CXX_LINK_EXECUTABLE
  "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES> ${CLANG_LINK_FLAGS}")

if (test)
  enable_testing()
  add_subdirectory(tests)
endif()

add_subdirectory(src)
