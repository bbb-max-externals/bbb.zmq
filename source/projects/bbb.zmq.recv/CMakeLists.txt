bbb_add_external(
	DEPS bbb_zmq_runtime
	INCLUDES
		${CMAKE_CURRENT_SOURCE_DIR}/../../runtime/include
		${CMAKE_CURRENT_SOURCE_DIR}/../../../extern/cppzmq
		${CMAKE_CURRENT_SOURCE_DIR}/../../../extern/libzmq/include
	RPATH "@loader_path/../../../.."
)
