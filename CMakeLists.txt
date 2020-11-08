cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_COVERAGE "Build coverage" OFF)

set(
        HUNTER_CACHE_SERVERS
        "https://github.com/bmstu-iu8-cpp-sem-3/hunter-cache"
        CACHE STRING "Default cache server"
)

include("tools/gate/cmake/HunterGate.cmake")

HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.23.278.tar.gz"
        SHA1 "35fa55bc1dfcf9f4d7264f8bb26cd1759ce1ae07"
)

project(hash_finder_lib)
project(hash_finder_app)
string(APPEND CMAKE_CXX_FLAGS " -pedantic -Werror -Wall -Wextra")
string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-command-line-argument")
string(APPEND CMAKE_CXX_FLAGS " -Wshadow -Wnon-virtual-dtor")

hunter_add_package(Boost COMPONENTS log filesystem thread)
find_package(Boost CONFIG REQUIRED log filesystem thread)
hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/PicoSHA2/)

FILE(GLOB_RECURSE HASH_FINDER_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sources/lib/*.cpp")
add_library(hash_finder_lib STATIC ${HASH_FINDER_LIB_SOURCES})
FILE(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
add_executable(tests ${TEST_SOURCES})
FILE(GLOB_RECURSE HASH_FINDER_APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sources/app/*.cpp")
add_executable(hash_finder_app ${HASH_FINDER_APP_SOURCES})

target_include_directories(hash_finder_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(hash_finder_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(tests PRIVATE GTest::gmock_main hash_finder_lib)
target_link_libraries(hash_finder_app PRIVATE hash_finder_lib Boost::log Boost::filesystem Boost::thread)

enable_testing()
add_test(NAME unit_tests COMMAND tests)

if (BUILD_COVERAGE)
    function(ENABLE_PROJECT_COVERAGE PROJECT)
        target_compile_options(${PROJECT} INTERFACE -O0 -g --coverage)
        if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
            target_link_options(${PROJECT} INTERFACE --coverage)
        else ()
            target_link_libraries(${PROJECT} INTERFACE --coverage)
        endif ()
    endfunction()

    set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/CMake-codecov/cmake")
    find_package(codecov)
    enable_project_coverage(hash_finder_lib)
    enable_project_coverage(tests)
    list(APPEND LCOV_REMOVE_PATTERNS "'${PROJECT_SOURCE_DIR}/tests/*'")
    coverage_evaluate()
endif ()
