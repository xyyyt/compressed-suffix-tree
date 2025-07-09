cmake_minimum_required(VERSION 3.0)
project(compressed_suffix_tree)

IF (${CMAKE_BUILD_TYPE} MATCHES "debug")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug)
  add_definitions(-g -W -Wall -Wextra -std=c++17)
ELSE ()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/release)
  add_definitions(-W -Wall -Wextra -std=c++17)
ENDIF()


# google-test
find_package(GTest REQUIRED)

set(RUNTIME_OUTPUT_DIRECTORY_TEST ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test)
include(CTest)


add_executable(CompressedSuffixTreeTest
  ${CMAKE_CURRENT_SOURCE_DIR}/test/CompressedSuffixTreeTest.cpp)

target_link_libraries(CompressedSuffixTreeTest PRIVATE
  GTest::gtest
  GTest::gmock)

target_include_directories(CompressedSuffixTreeTest PRIVATE
  ${GTEST_INCLUDE_DIRS}
  ${GMOCK_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(CompressedSuffixTreeTest
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIRECTORY_TEST})

add_test(NAME CompressedSuffixTreeTest
  COMMAND CompressedSuffixTreeTest
  WORKING_DIRECTORY ${RUNTIME_OUTPUT_DIRECTORY_TEST})
