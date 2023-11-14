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
	src/types/table.cpp)

if (NOT DEFINED FART_SRC)
	message(FATAL_ERROR "Missing fart source directory (FART_SRC).")
endif()

if (NOT DEFINED LUA_VERSION)
	set(LUA_VERSION "5.4.6")
endif()

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/build)
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
endif()

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/build/lua-${LUA_VERSION}.tar.gz)
	execute_process(
		COMMAND curl -R -O http://www.lua.org/ftp/lua-${LUA_VERSION}.tar.gz
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
	)
	execute_process(
		COMMAND tar -zxf lua-${LUA_VERSION}.tar.gz
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
	)
endif()

execute_process(
	COMMAND ${LUA_MAKE_PREFIX} make all test
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lua-${LUA_VERSION})

target_include_directories(fart-lua SYSTEM
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/build/lua-${LUA_VERSION}/src
	PUBLIC ${FART_SRC})

message(STATUS ${FART_SRC})

target_link_libraries(fart-lua ${CMAKE_CURRENT_SOURCE_DIR}/build/lua-${LUA_VERSION}/src/liblua.a)

add_definitions(-DFART_NO_IO -DFART_NO_SOCKET)

target_compile_features(fart-lua PRIVATE cxx_std_17)

set_target_properties(fart-lua
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

target_precompile_headers(fart-lua PRIVATE
	${FART_SRC}/fart.hpp)
