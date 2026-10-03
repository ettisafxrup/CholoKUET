cmake_minimum_required(VERSION 3.10)
project(CholoKUET LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Static runtime on MinGW so the exe doesn't load another toolchain's
# libstdc++ from PATH, and runs on PCs without MinGW.
if(MINGW)
    add_link_options(-static)
endif()

file(GLOB CORE_SOURCES dsa/*/*.cpp src/*.cpp)

add_library(cholokuet_core STATIC ${CORE_SOURCES})
# main.cpp can also pull in every source itself for a one-line g++ build;
# here each file is compiled separately, so switch that off.
target_compile_definitions(cholokuet_core PUBLIC CHOLOKUET_SEPARATE_FILES)

add_executable(CholoKUET main.cpp)
if(WIN32)
    target_sources(CholoKUET PRIVATE assets/cholokuet.rc)   # app icon
endif()
target_link_libraries(CholoKUET PRIVATE cholokuet_core)

enable_testing()
file(GLOB TEST_SOURCES tests/test_*.cpp)
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE cholokuet_core)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
