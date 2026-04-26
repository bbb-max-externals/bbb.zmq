add_library(bbb_zmq_runtime SHARED
	src/packet_store.cpp
	src/dsl_lexer.cpp
	src/dsl_parser.cpp
	src/schema_compiler.cpp
	src/schema_registry.cpp
	src/parser_vm.cpp
	src/runtime.cpp
)

target_include_directories(bbb_zmq_runtime PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(bbb_zmq_runtime PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../extern/cppzmq
	${CMAKE_CURRENT_SOURCE_DIR}/../../extern/libzmq/include
)

target_link_libraries(bbb_zmq_runtime PUBLIC libzmq-static)

target_compile_features(bbb_zmq_runtime PUBLIC cxx_std_17)

if(WIN32)
	target_compile_definitions(bbb_zmq_runtime PUBLIC ZMQ_STATIC)
endif()

set_target_properties(bbb_zmq_runtime PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	OUTPUT_NAME "bbb_zmq_runtime"
)

if(APPLE)
	set_target_properties(bbb_zmq_runtime PROPERTIES
		MACOSX_RPATH ON
		INSTALL_RPATH "@loader_path"
	)
endif()

set(C74_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../externals")
set_target_properties(bbb_zmq_runtime PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${C74_LIBRARY_OUTPUT_DIRECTORY}"
	LIBRARY_OUTPUT_DIRECTORY_DEBUG "${C74_LIBRARY_OUTPUT_DIRECTORY}"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE "${C74_LIBRARY_OUTPUT_DIRECTORY}"
)
if(WIN32)
	set_target_properties(bbb_zmq_runtime PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${C74_LIBRARY_OUTPUT_DIRECTORY}"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${C74_LIBRARY_OUTPUT_DIRECTORY}"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${C74_LIBRARY_OUTPUT_DIRECTORY}"
		ARCHIVE_OUTPUT_DIRECTORY "${C74_LIBRARY_OUTPUT_DIRECTORY}"
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${C74_LIBRARY_OUTPUT_DIRECTORY}"
		ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${C74_LIBRARY_OUTPUT_DIRECTORY}"
	)
endif()
