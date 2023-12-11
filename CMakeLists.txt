cmake_minimum_required(VERSION 3.10)

project(ASIO)

list(APPEND CMAKE_MODULE_PATH 
${CMAKE_SOURCE_DIR}/cmake_modules
)

find_package(Threads)

include(enviroment)

LIBS(pthread)

add_subdirectory(src)
add_subdirectory(lib)
add_subdirectory(tst)
