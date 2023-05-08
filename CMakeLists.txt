cmake_minimum_required(VERSION 3.14)
project(rome_delegates VERSION 0.1 LANGUAGES CXX)

option(ROME_DELEGATES_BUILD_TESTS "enable to also build the tests as 'test_delegates'" OFF)


add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE
    include/rome/delegate.hpp
    include/rome/detail/bad_delegate_call.hpp
    include/rome/detail/delegate_base.hpp
)
add_library(rome::delegates ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_14)


if(ROME_DELEGATES_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
