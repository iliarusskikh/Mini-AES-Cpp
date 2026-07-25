cmake_minimum_required(VERSION 3.16)
project(MiniAES VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(mini_aes source/main.cpp)
target_include_directories(mini_aes PRIVATE include)

if(MSVC)
    target_compile_options(mini_aes PRIVATE /W4 /permissive-)
else()
    target_compile_options(mini_aes PRIVATE -Wall -Wextra -Wpedantic)
endif()
