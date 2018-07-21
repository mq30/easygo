cmake_minimum_required(VERSION 2.8)

project(network)
set(TARGET_NAME network)

# support c++11
add_definitions(-std=c++11)

# export dynamic library
add_definitions(-DEXPORT_NETWORK)

# project root derectory
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories(
	${PROJECT_ROOT}/include)

set(SRC_LIST 
	${PROJECT_ROOT}/include/network/CNetBase.h
	${PROJECT_ROOT}/include/network/CListener.h
	${PROJECT_ROOT}/include/network/CConnector.h
	${PROJECT_ROOT}/include/network/NetType.h
	${PROJECT_ROOT}/include/network/CSocket.h
	${PROJECT_ROOT}/include/network/CNetReadTask.h
	${PROJECT_ROOT}/include/network/CNetWriteTask.h
	${PROJECT_ROOT}/include/network/CNetCloseTask.h
	${PROJECT_ROOT}/include/network/CNetConnectTask.h
	${PROJECT_ROOT}/include/network/CAcceptor.h
	${PROJECT_ROOT}/include/network/CActor.h
	${PROJECT_ROOT}/include/network/CNetBuffer.h
	${PROJECT_ROOT}/include/network/CConnection.h
	${PROJECT_ROOT}/include/network/CNetClock.h
	${PROJECT_ROOT}/include/network/CTimer.h
	${PROJECT_ROOT}/include/declexport.h
	CNetBase.cpp 
	CListener.cpp
	CConnector.cpp
	CSocket.cpp
	CNetReadTask.cpp
	CNetWriteTask.cpp
	CNetCloseTask.cpp
	CNetConnectTask.cpp
	CAcceptor.cpp
	CActor.cpp
	CNetBuffer.cpp
	CConnection.cpp
	CNetClock.cpp
	CTimer.cpp
	)

if (UNIX)
	# set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif (UNIX)

# default static library
if (NOT EASYGO_LIBRARY_TYPE)
	set(EASYGO_LIBRARY_TYPE "STATIC")
endif (NOT EASYGO_LIBRARY_TYPE)

# add EASYGO_DYNAMIC_LINK definition to support dynamic library
string(TOUPPER ${EASYGO_LIBRARY_TYPE} EASYGO_LIBRARY_TYPE)
if (${EASYGO_LIBRARY_TYPE} STREQUAL "SHARED")
	add_definitions(-DEASYGO_DYNAMIC_LINK)
endif (${EASYGO_LIBRARY_TYPE} STREQUAL "SHARED")

# generate library
add_library(${TARGET_NAME} ${EASYGO_LIBRARY_TYPE} ${SRC_LIST})

# configure link libraries
if (WIN32)
	target_link_libraries(${TARGET_NAME}
		ws2_32.lib
		debug ${PROJECT_ROOT}/debug/lib/base.lib
		debug ${PROJECT_ROOT}/debug/lib/thread.lib
		optimized ${PROJECT_ROOT}/release/lib/base.lib
		optimized ${PROJECT_ROOT}/release/lib/thread.lib
)
else ()
	target_link_libraries(${TARGET_NAME}
		debug ${PROJECT_ROOT}/debug/lib/libthread.a
		debug ${PROJECT_ROOT}/debug/lib/libbase.a
		optimized ${PROJECT_ROOT}/debug/lib/libthread.a
		optimized ${PROJECT_ROOT}/debug/lib/libbase.a
)
endif ()

if (WIN32)
	# Don't generate manifests
	set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/MANIFEST:NO" )
	add_definitions(-DWIN32)
endif (WIN32)

# default release
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif (NOT CMAKE_BUILD_TYPE)
message( "   ${TARGET_NAME}: ${CMAKE_BUILD_TYPE} ${EASYGO_LIBRARY_TYPE}" )

# configure output directory
# OUTPUTCONFIG: DEBUG, RELEASE, MINSIZEREL, RELWITHDEBINFO
set(CMAKE_CONFIGURATION_TYPES DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
	message( "** Configuring ${OUTPUTCONFIG} ...")
	string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
	string( TOLOWER ${OUTPUTCONFIG} DIR_NAME )
	set_target_properties( ${TARGET_NAME} PROPERTIES 
		RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_ROOT}/${DIR_NAME}/lib )
	set_target_properties( ${TARGET_NAME} PROPERTIES 
		LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_ROOT}/${DIR_NAME}/lib )
	set_target_properties( ${TARGET_NAME} PROPERTIES 
		ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_ROOT}/${DIR_NAME}/lib )
endforeach( OUTPUTCONFIG )

message( "" )
