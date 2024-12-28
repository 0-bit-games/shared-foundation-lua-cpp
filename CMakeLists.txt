cmake_minimum_required(VERSION 3.22)

project(fart-lua)

if(NOT LIBRARY_BUILD_TYPE)
	set(LIBRARY_BUILD_TYPE SHARED)
endif()

add_library(fart-lua ${LIBRARY_BUILD_TYPE}
	src/exceptions/compiler-exception.cpp
	src/exceptions/not-supported-exception.cpp
	src/exceptions/runtime-exception.cpp
	src/exceptions/unexpected-type-exception.cpp
	src/global.cpp
	src/state.cpp
	src/types/boolean.cpp
	src/types/caller.cpp
	src/types/function.cpp
	src/types/light-user-data.cpp
	src/types/number.cpp
	src/types/string.cpp
	src/types/table.cpp
	src/types/type.cpp
	src/types/user-function.cpp
	src/types/value.cpp)

if(NOT DEFINED LUA_VERSION)
	set(LUA_VERSION "5.4.6")
endif()

if(${LIBRARY_BUILD_TYPE} STREQUAL "SHARED")
	set(LUA_ENABLE_SHARED ON)
else()
	set(LUA_ENABLE_SHARED OFF)
endif()

set(LUA_ENABLE_TESTING OFF)
set(LUA_BUILD_COMPILER OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/lua)

if(${LIBRARY_BUILD_TYPE} STREQUAL "SHARED")
	target_link_libraries(fart-lua lua_shared)
else()
	target_link_libraries(fart-lua lua_static)
endif()

add_definitions(-DFART_NO_IO -DFART_NO_SOCKET)

target_compile_features(fart-lua PRIVATE cxx_std_17)

set(ARCH_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/${CMAKE_SYSTEM_PROCESSOR}")

set_target_properties(fart-lua
	PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${ARCH_OUTPUT_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${ARCH_OUTPUT_DIR}/bin")

target_precompile_headers(fart-lua PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/fart/fart.hpp)
