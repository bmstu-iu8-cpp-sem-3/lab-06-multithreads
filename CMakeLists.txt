cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_COVERAGE "Build code coverage" OFF)

set(
  HUNTER_CACHE_SERVERS
  "https://github.com/bmstu-iu8-cpp-sem-3/hunter-cache"
  CACHE STRING "Default cache server"
)

include("cmake/HunterGate.cmake")

HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.23.278.tar.gz"
  SHA1 "35fa55bc1dfcf9f4d7264f8bb26cd1759ce1ae07"
)

project(sha256 VERSION 0.1.0)
string(APPEND CMAKE_CXX_FLAGS " -pedantic -Werror -Wall -Wextra")
string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-command-line-argument")
string(APPEND CMAKE_CXX_FLAGS " -Wshadow -Wnon-virtual-dtor")

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

find_package(Threads)

add_subdirectory("third-party/")

add_library(sha256 STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/source.cpp
)

add_executable(demo
  ${CMAKE_CURRENT_SOURCE_DIR}/demo/main.cpp
)

target_include_directories(sha256 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(demo PUBLIC sha256)

target_link_libraries(sha256
        PUBLIC Threads::Threads
        picosha)

add_executable(tests
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp
        )
target_include_directories(tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/sources
        )
target_link_libraries(tests
        PRIVATE GTest::gtest_main sha256
        )

enable_testing()
add_test(NAME unit_tests COMMAND tests)

if(BUILD_COVERAGE)
  function(ENABLE_PROJECT_COVERAGE PROJECT)
    target_compile_options(${PROJECT} PRIVATE --coverage)
    target_link_libraries(${PROJECT} PRIVATE --coverage)
    add_coverage(${PROJECT})
  endfunction()

  set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  find_package(codecov)
  enable_project_coverage(hashresearch)
  enable_project_coverage(tests)
  list(APPEND LCOV_REMOVE_PATTERNS "'${PROJECT_SOURCE_DIR}/tests/*'")
  coverage_evaluate()
endif()
