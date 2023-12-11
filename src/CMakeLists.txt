cmake_minimum_required(VERSION 3.10)

add_subdirectory(config)
add_subdirectory(Server)
add_subdirectory(Client)

BUILD_EXEC(APP LOG CONFIG PEVENTS SERVER CLIENT)
