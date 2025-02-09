cmake_minimum_required(VERSION 3.22)

project(foundation-lua)

if(NOT TARGET foundation)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/foundation/)
endif()

if(NOT LIBRARY_BUILD_TYPE)
	set(LIBRARY_BUILD_TYPE SHARED)
endif()

add_library(foundation-lua ${LIBRARY_BUILD_TYPE}
	src/exceptions/compiler-exception.cpp
	src/exceptions/exhausted-stack-exception.cpp
	src/exceptions/not-supported-exception.cpp
	src/exceptions/runtime-exception.cpp
	src/exceptions/unexpected-type-exception.cpp
	src/global.cpp
	src/stack-trace-entry.cpp
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
	target_link_libraries(foundation-lua lua_shared)
else()
	target_link_libraries(foundation-lua lua_static)
endif()

target_compile_features(foundation-lua PRIVATE cxx_std_20)

set(ARCH_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/${CMAKE_SYSTEM_PROCESSOR}")

set_target_properties(foundation-lua
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${ARCH_OUTPUT_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${ARCH_OUTPUT_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${ARCH_OUTPUT_DIR}/bin")

target_precompile_headers(foundation-lua PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/foundation/src/foundation.hpp)

target_link_libraries(foundation-lua foundation)
