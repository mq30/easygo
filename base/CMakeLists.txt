cmake_minimum_required(VERSION 2.8)

project(base)
set(TARGET_NAME base)

# support c++11
add_definitions(-std=c++11)

# export dynamic library
add_definitions(-DEXPORT_BASE)

# project root derectory
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories(
	${PROJECT_ROOT}/include)

set(SRC_LIST 
	${PROJECT_ROOT}/include/base/CBase.h
	${PROJECT_ROOT}/include/base/CCounter.h
	${PROJECT_ROOT}/include/base/CUuid.h
	${PROJECT_ROOT}/include/base/CSystem.h
	${PROJECT_ROOT}/include/declexport.h
	CCounter.cpp
	CUuid.cpp
	CSystem.cpp
	)

if (UNIX)
	# set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif (UNIX)

# default static library
if (NOT EASYGO_LIBRARY_TYPE)
	set(EASYGO_LIBRARY_TYPE "STATIC")
endif (NOT EASYGO_LIBRARY_TYPE)

# if this is an shared library, add EASYGO_DYNAMIC_LINK definition.
string(TOUPPER ${EASYGO_LIBRARY_TYPE} EASYGO_LIBRARY_TYPE)
if (${EASYGO_LIBRARY_TYPE} STREQUAL "SHARED")
	add_definitions(-DEASYGO_DYNAMIC_LINK)
endif (${EASYGO_LIBRARY_TYPE} STREQUAL "SHARED")

# generate library
add_library(${TARGET_NAME} ${EASYGO_LIBRARY_TYPE} ${SRC_LIST})

# configure link libraries
target_link_libraries(${TARGET_NAME}
	)

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
