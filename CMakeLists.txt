# stop if cmake version below 3.5
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(APP_NAME prf_test_apps)

# project name and supported languages
project(${APP_NAME} C CXX)

# set default build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# detect operating system and host processor
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

# set c++ compiler flags
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif ()

# project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

set(INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/common
        )
include_directories(
        ${INCLUDE_DIRS}
)

set(COMMON_SRCS
        ${CMAKE_SOURCE_DIR}/common/crc32.c
        pws_callback/pws_callback_sample.cpp)


add_executable(pws_status_spi
        pws_status/pws_status_spi.cpp
        ${COMMON_SRCS}
        )

add_executable(pws_callback_sample
        pws_callback/pws_callback_sample.cpp
        ${COMMON_SRCS}
        )
target_link_libraries(pws_callback_sample
        ${CMAKE_SOURCE_DIR}/static/libpws.a
        )
