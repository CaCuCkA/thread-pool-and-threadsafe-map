cmake_minimum_required( VERSION 3.15 )

set( PROJECT_NAME PARALLEL_FEATURES )
project( ${PROJECT_NAME} C CXX )

set( THREADSAFE_MAP "threadsafe_map" )
set( THREAD_POOL "thread_pool" )

set( CMAKE_CXX_STANDARD 17 )

set( WARNINGS_AS_ERRORS OFF )

set( ENABLE_PVS_STUDIO OFF )


set( ENABLE_UBSan OFF )
set( ENABLE_ASAN OFF )
set( ENABLE_TSan OFF )
set( ENABLE_MSAN OFF )

if ( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release )
endif ()

include( cmake/CompilerWarnings.cmake )

##########################################################
include_directories( src include )

set( SRC ${CMAKE_SOURCE_DIR}/src )
set( INC ${CMAKE_SOURCE_DIR}/include )
##########################################################


add_executable( ${THREADSAFE_MAP} 	${SRC}/TestThreadSafeMap.cpp
                ${INC}/ThreadSafeMap.h )

add_executable( ${THREAD_POOL} 	${SRC}/TestThreadPool.cpp
        ${INC}/FunctionWrapper.h ${INC}/JoinThreads.h
        ${INC}/ThreadSafeQueue.h ${INC}/StaticThreadPool.h )

SET_TARGET_PROPERTIES( ${THREADSAFE_MAP} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )

SET_TARGET_PROPERTIES( ${THREAD_POOL} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )

# Include CMake setup
include( cmake/main-config.cmake )