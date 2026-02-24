cmake_minimum_required(VERSION 3.20)
project(LSMEngine CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(include)
add_executable(lsm_engine 
    src/main.cpp 
    src/core/skiplist.cpp
    src/storage/io_engine.cpp
)

# io_uring 
find_package(PkgConfig REQUIRED)
pkg_check_modules(URING REQUIRED liburing)
target_link_libraries(lsm_engine PRIVATE ${URING_LIBRARIES})