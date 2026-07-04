cmake_minimum_required(VERSION 3.19)
project(bbb.zmq)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(BBB_ZMQ_BUILD_TESTS "Build bbb.zmq runtime tests" OFF)

# --- load bbb_add_external() ---
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/bbb_external.cmake)

# --- min-api ---
set(C74_MIN_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/min-api)
include(${C74_MIN_API_DIR}/script/min-package.cmake)

# --- output directory ---
set(C74_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/externals)

# --- universal binary (macOS) ---
if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
	set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS architecture" FORCE)
endif()

# --- ZeroMQ (static) ---
set(ZMQ_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ENABLE_CPACK OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC ON CACHE BOOL "" FORCE)
set(BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(ENABLE_WS OFF CACHE BOOL "" FORCE)
set(WITH_DOCS OFF CACHE BOOL "" FORCE)
set(ENABLE_DRAFTS OFF CACHE BOOL "" FORCE)
if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
endif()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/libzmq)

# --- shared runtime library ---
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/runtime)

# --- auto-discover external projects ---
MACRO(SUBDIRLIST result curdir)
	FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
	SET(dirlist "")
	FOREACH(child ${children})
		IF(IS_DIRECTORY ${curdir}/${child})
			LIST(APPEND dirlist ${child})
		ENDIF()
	ENDFOREACH()
	SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(PROJECT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/source/projects)
foreach(project_dir ${PROJECT_DIRS})
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/projects/${project_dir}/CMakeLists.txt")
		message("Generating: ${project_dir}")
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/projects/${project_dir})
	endif()
endforeach()

if(BBB_ZMQ_BUILD_TESTS)
	enable_testing()
	set(CMAKE_EXE_LINKER_FLAGS "")
	foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
		set(CMAKE_EXE_LINKER_FLAGS_${config} "")
	endforeach()
	add_executable(bbb_zmq_runtime_tests
		tests/runtime_tests.cpp
		source/runtime/src/packet_store.cpp
		source/runtime/src/dsl_lexer.cpp
		source/runtime/src/dsl_parser.cpp
		source/runtime/src/schema_compiler.cpp
		source/runtime/src/schema_registry.cpp
		source/runtime/src/parser_vm.cpp
		source/runtime/src/runtime.cpp
	)
	target_include_directories(bbb_zmq_runtime_tests PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/source/runtime/include
	)
	target_compile_features(bbb_zmq_runtime_tests PRIVATE cxx_std_17)
	add_test(NAME bbb_zmq_runtime_tests COMMAND bbb_zmq_runtime_tests)
endif()

set(CMAKE_SUPPRESS_REGENERATION true)
