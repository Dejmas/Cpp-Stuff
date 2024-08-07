cmake_minimum_required(VERSION 3.10)
project(fifos)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRCS "src/*.cpp")
file(GLOB_RECURSE HDRS "src/*.h")


# Add AddressSanitizer flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=return")

# Link against ASan runtime library
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")


add_executable(fifos ${SRCS} ${HDRS})
