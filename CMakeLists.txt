# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.21)

# We'll be using C++ 17
set(CMAKE_CXX_STANDARD 17)

project(composer
    VERSION 1.0.0
    LANGUAGES CXX ASM
)

# Add includes path
include_directories(include/)

# Composer library
add_library(composer STATIC 
src/composer/collection.cpp 
    src/composer/compiler.cpp 
    src/composer/encrypt.cpp 
    src/composer/file.cpp
)

# Hash library
add_library(hash-library STATIC
    src/hash-library/md5.cpp
)

# Set linker stuff
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++ -s")
link_libraries(composer hash-library d3dx9 d3d9)

# Build tools
add_executable(composer-assemble src/assemble.cpp)
add_executable(composer-compile src/compile.cpp)
add_executable(composer-disassemble src/disassemble.cpp)
add_executable(composer-explode src/explode.cpp)
add_executable(composer-merge src/merge.cpp)
add_executable(composer-decrypt src/decrypt.cpp)
add_executable(composer-encrypt src/encrypt.cpp)
