# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.21)

# We'll be using C++ 17
set(CMAKE_CXX_STANDARD 17)

project(composer
    VERSION 1.0.0
    LANGUAGES CXX
)

# Add includes path
include_directories(include/)

# Composer library
add_library(composer STATIC 
    src/composer/encrypt.cpp 
    src/composer/file.cpp
)

# Hash library
add_library(hash-library STATIC
    src/hash-library/md5.cpp
)

# Set linker stuff
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++ -s")
link_libraries(composer hash-library)

# Build tools
add_executable(composer-decrypt src/decrypt.cpp)
add_executable(composer-encrypt src/encrypt.cpp)
