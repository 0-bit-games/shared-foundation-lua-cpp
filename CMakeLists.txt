cmake_minimum_required(VERSION 3.27)

project(fart-lua)

add_library(fart-lua
	src/state.cpp
	src/global.cpp
	src/exceptions/compiler-exception.cpp
	src/exceptions/runtime-exception.cpp
	src/exceptions/not-supported-exception.cpp
	src/exceptions/unexpected-type-exception.cpp
	src/types/type.cpp
	src/types/boolean.cpp
	src/types/number.cpp
	src/types/string.cpp
	src/types/function.cpp
	src/types/caller.cpp
	src/types/table.cpp
	src/types/light-user-data.cpp
	src/types/user-function.cpp
	src/types/value.cpp)

if (NOT DEFINED LUA_VERSION)
	set(LUA_VERSION "5.4.6")
endif()

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/build)
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/lua)

target_link_libraries(fart-lua lua_static)

add_definitions(-DFART_NO_IO -DFART_NO_SOCKET)

target_compile_features(fart-lua PRIVATE cxx_std_17)

set_target_properties(fart-lua
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

target_precompile_headers(fart-lua PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/fart/fart.hpp)
