cmake_minimum_required(VERSION 3.10)

project(mapreduce-cpp VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_executable(worker
    src/worker/helper.cpp
    src/worker/worker.cpp
    src/worker/wc.cpp
    src/worker/coordinator.cpp
    src/worker/main.cpp
)

include_directories(/path/to/json-library/include)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(worker PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(worker PRIVATE /W4)
endif()