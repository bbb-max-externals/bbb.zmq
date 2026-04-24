include(${CMAKE_CURRENT_LIST_DIR}/../../../extern/min-api/script/min-pretarget.cmake)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../runtime/include
	${CMAKE_CURRENT_SOURCE_DIR}/../../../extern/cppzmq
	${CMAKE_CURRENT_SOURCE_DIR}/../../../extern/libzmq/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE bbb_zmq_runtime)

set_target_properties(${PROJECT_NAME} PROPERTIES
	BUILD_RPATH "@loader_path/../../../.."
	INSTALL_RPATH "@loader_path/../../../.."
)

include(${CMAKE_CURRENT_LIST_DIR}/../../../extern/min-api/script/min-posttarget.cmake)
