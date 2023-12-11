cmake_minimum_required(VERSION 3.10)



function(BUILD_EXEC)
	set(list_args "${ARGN}")
	set(target_link_exec "${ARGN}")
	build_exec("${target_link_exec}")
endfunction()

function(BUILD_EXEC)
	set(COMPILER_SETTINGS " ${CMAKE_CXX_FLAGS} -Wall ")
	set(list_args "${ARGN}")
	list(GET list_args 0 exec_name)
	file(GLOB exec_sources  true *.h *.cpp *.hpp *.c)
	add_executable(${exec_name} ${exec_sources})
	target_include_directories(${exec_name} PUBLIC ./)
	target_link_libraries(${list_args})
	set_target_properties(${exec_name} PROPERTIES COMPILE_FLAGS "${COMPILER_SETTINGS}")
endfunction()


function(BUILD_LIB)
	set(list_args "${ARGN}")
	set(target_link_libs "${ARGN}")
	build_lib("${target_link_libs}")
endfunction()


function(BUILD_LIB)
	set(COMPILER_SETTINGS " ${CMAKE_CXX_FLAGS} -Wall ")
	set(list_args "${ARGN}")
	list(GET list_args 0 lib_name)
	file(GLOB lib_sources  true *.h *.cpp *.hpp *.c)
	add_library(${lib_name} SHARED ${lib_sources})
	target_include_directories(${lib_name} PUBLIC ./)
	target_link_libraries(${list_args})
	set_target_properties(${lib_name} PROPERTIES COMPILE_FLAGS "${COMPILER_SETTINGS}")
endfunction()


function(BUILD_INTERFACE)
	set(list_args "${ARGN}")
	add_library(${list_args} INTERFACE)
	target_include_directories(${list_args} INTERFACE ./)
endfunction()



