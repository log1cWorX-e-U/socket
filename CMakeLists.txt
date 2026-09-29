cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)

project(socket LANGUAGES C)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(socket STATIC)
add_subdirectory(../string ${CMAKE_BINARY_DIR}/libs_build/string_from_socket)
add_subdirectory(../threading ${CMAKE_BINARY_DIR}/libs_build/threading_from_socket)

target_sources(socket PRIVATE
	socket.c
)

target_include_directories(socket PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(socket
	PUBLIC
		string
		threading
)
