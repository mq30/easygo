cmake_minimum_required(VERSION 2.8)

project(server)
set(TARGET_NAME server)

# support c++11
add_definitions(-std=c++11)

# project root derectory
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories(
	${PROJECT_ROOT}/include)

set(SRC_LIST 
	main.cpp 
	CAppConnection.h
	CAppConnection.cpp
	CAppListener.h
	CAppListener.cpp
	CAppConnector.h
	CAppConnector.cpp
	CAppTimer.h
	CAppTimer.cpp
	)

if (UNIX)
	# set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif (UNIX)

# generate executable file
add_executable(${TARGET_NAME} ${SRC_LIST})

# configure link libraries
if (WIN32)
	target_link_libraries(${TARGET_NAME}
	ws2_32.lib
	debug ${PROJECT_ROOT}/debug/lib/base.lib
	debug ${PROJECT_ROOT}/debug/lib/thread.lib
	debug ${PROJECT_ROOT}/debug/lib/network.lib
	optimized ${PROJECT_ROOT}/release/lib/base.lib
	optimized ${PROJECT_ROOT}/release/lib/thread.lib
	optimized ${PROJECT_ROOT}/release/lib/network.lib
)
else ()
	target_link_libraries(${TARGET_NAME}
	pthread
	debug ${PROJECT_ROOT}/debug/lib/libnetwork.a
	debug ${PROJECT_ROOT}/debug/lib/libthread.a
	debug ${PROJECT_ROOT}/debug/lib/libbase.a
	optimized ${PROJECT_ROOT}/release/lib/libnetwork.a
	optimized ${PROJECT_ROOT}/release/lib/libthread.a
	optimized ${PROJECT_ROOT}/release/lib/libbase.a
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
