cmake_minimum_required(VERSION 3.6)
project(wbm)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

include_directories(
    ./include
    ./include/WBMatrix
    ./include/OptWBMatrix
)

# Gather all source files
file(GLOB WBMatrix_SRC ./src/WBMatrix/*.c)
file(GLOB OptWBMatrix_SRC ./src/OptWBMatrix/*.c)

# Main executable with all C source files directly
add_executable(${PROJECT_NAME} 
    ./main.cpp
    ${WBMatrix_SRC}
    ${OptWBMatrix_SRC}
)