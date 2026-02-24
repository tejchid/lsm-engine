cmake_minimum_required(VERSION 3.22)
project(LSMEngine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -Wall -Wextra")

find_package(PkgConfig REQUIRED)
pkg_check_modules(URING REQUIRED liburing)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(lsm_engine ${SOURCES})
target_link_libraries(lsm_engine PRIVATE ${URING_LIBRARIES})