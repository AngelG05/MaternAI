cmake_minimum_required(VERSION 3.15)
project(MaternAI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /O2 /Oi /Ot /GL /arch:AVX2)
    link_libraries(ws2_32 crypt32)
else()
    add_compile_options(-O3 -march=native -flto -Wall -Wextra -pthread)
    add_link_options(-flto)
    link_libraries(pthread)
endif()

include_directories(include)

add_executable(MaternAI src/main.cpp)

add_executable(test_engine src/test_engine.cpp)
target_include_directories(test_engine PRIVATE include)

add_executable(benchmarks src/benchmarks.cpp)
target_include_directories(benchmarks PRIVATE include)

if(MSVC)
    target_link_libraries(test_engine PRIVATE ws2_32 crypt32)
    target_link_libraries(benchmarks PRIVATE ws2_32 crypt32)
else()
    target_link_libraries(test_engine PRIVATE pthread)
    target_link_libraries(benchmarks PRIVATE pthread)
endif()
