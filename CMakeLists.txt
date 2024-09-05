cmake_minimum_required(VERSION 3.20)
project(rome_delegates VERSION 1.0 LANGUAGES CXX)

option(ROME_DELEGATES_BUILD_TESTS "Enable to also configure the test targets." OFF)
option(ROME_DELEGATES_INSTRUMENT "Instrument unit tests for sanitizers and code coverage." OFF)


add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE
    include/rome/delegate.hpp
)
add_library(rome::delegates ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_14)


if(ROME_DELEGATES_BUILD_TESTS)
    add_subdirectory(test)
endif()
