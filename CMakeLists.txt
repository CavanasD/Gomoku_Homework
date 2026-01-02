cmake_minimum_required(VERSION 3.16)

project(Gomoku_Homework VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build Type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler Options
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -O3)
endif()

# Source Files
set(SRC_FILES
    src/main.cpp
    src/widget/gomokuLogic.cpp
    src/widget/gomokuLogic.h
    src/widget/aibrain.cpp
    src/widget/aibrain.h
        src/widget/main_console.cpp

)

# Executable
add_executable(gomoku_core ${SRC_FILES})

# Include Directories
target_include_directories(gomoku_core PRIVATE src)

