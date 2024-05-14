cmake_minimum_required(VERSION 3.9)
project(SpComm3d)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

set (CMAKE_CXX_COMPILER mpic++)

# set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -march=native")
# set(CMAKE_CXX_FLAGS_DEBUG "-g")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(src)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(benchSddmm "miniapp/bench_sddmm.cpp" ${SOURCES})

add_executable(benchSpMM "miniapp/bench_spmm.cpp" ${SOURCES})

# add_executable(benchBoth "miniapp/main_combined.cpp" ${SOURCES})

# add_executable(serialSim "miniapp/serialSim.cpp" ${SOURCES})

add_executable(numCheckSeq "miniapp/numCheckSeq.cpp" ${SOURCES})

add_executable(conv2bin "tools/read_convert_mm.cpp")

add_executable(test "miniapp/test.cpp")

