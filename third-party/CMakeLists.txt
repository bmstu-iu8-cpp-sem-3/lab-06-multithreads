add_library(picosha INTERFACE)

target_include_directories(picosha
        INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/PicoSHA2
        )