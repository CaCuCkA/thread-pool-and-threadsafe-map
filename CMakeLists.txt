cmake_minimum_required(VERSION 3.17)

set(THREAD_POOL thread_pool)
set(PROJECT_NAME thread_pools)
set(THREAD_POOL_USING_WIN_API thread_pool_using_win_api)
set(THREAD_POOL_USING_POSIX_API thread_pool_using_posix_api)
set(THREAD_POOL_WITH_LOCAL_QUEUE thread_pool_with_local_queue)
set(THREAD_POOL_WITH_WORK_STEALING thread_pool_with_work_stealing)

project(${PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set(WARNINGS_AS_ERRORS OFF)

set(ENABLE_PVS_STUDIO OFF)

set(ENABLE_UBSan OFF)
set(ENABLE_ASAN OFF)
set(ENABLE_TSan OFF)
set(ENABLE_MSAN OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

include(cmake/CompilerWarnings.cmake)

##########################################################
set(SRC ${CMAKE_SOURCE_DIR}/src)
set(INC ${CMAKE_SOURCE_DIR}/include)
##########################################################

set(THREAD_POOL_BASE ${INC}/FunctionWrapper.h ${INC}/JoinThreads.h
                     ${INC}/Utils.h           ${INC}/CrossType.h)

add_executable(${THREAD_POOL}            ${INC}/StaticThreadPool.h
               ${INC}/ThreadSafeQueue.h  ${SRC}/Main.cpp
               ${THREAD_POOL_BASE})

add_executable(${THREAD_POOL_WITH_LOCAL_QUEUE}  ${INC}/StaticThreadPoolWithLocalQueue.h
               ${INC}/ThreadSafeQueue.h         ${SRC}/Main.cpp
               ${THREAD_POOL_BASE})

add_executable(${THREAD_POOL_WITH_WORK_STEALING}  ${INC}/StaticThreadPoolWithWorkStealing.h
               ${INC}/WorkStealingQueue.h         ${SRC}/Main.cpp
               ${THREAD_POOL_BASE})

add_executable(${THREAD_POOL_USING_WIN_API}  ${INC}/StaticThreadPoolUsingWinApi.h
               ${SRC}/Main.cpp)

add_executable(${THREAD_POOL_USING_POSIX_API}  ${INC}/StaticThreadPoolUsingPosixApi.h
               ${SRC}/Main.cpp)


target_compile_definitions(${THREAD_POOL} PRIVATE THREAD_POOL)
target_compile_definitions(${THREAD_POOL_WITH_LOCAL_QUEUE} PRIVATE QUEUE_THREAD_POOL)
target_compile_definitions(${THREAD_POOL_WITH_WORK_STEALING} PRIVATE STEALING_THREAD_POOL)

target_include_directories(${THREAD_POOL} PRIVATE ${INC} ${SRC})
target_include_directories(${THREAD_POOL_USING_WIN_API} PRIVATE ${INC} ${SRC})
target_include_directories(${THREAD_POOL_USING_POSIX_API} PRIVATE ${INC} ${SRC})
target_include_directories(${THREAD_POOL_WITH_LOCAL_QUEUE} PRIVATE ${INC} ${SRC})
target_include_directories(${THREAD_POOL_WITH_WORK_STEALING} PRIVATE ${INC} ${SRC})

set_target_properties(${THREAD_POOL_WITH_LOCAL_QUEUE} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        )

set_target_properties(${THREAD_POOL} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        )

set_target_properties(${THREAD_POOL_WITH_WORK_STEALING} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        )

set_target_properties(${THREAD_POOL_USING_WIN_API} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        )

set_target_properties(${THREAD_POOL_USING_POSIX_API} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        )

include(cmake/main-config.cmake)