cmake_minimum_required(VERSION 2.6) 

project(Vana)

#### Finding libraries
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.37.0 REQUIRED COMPONENTS filesystem regex system thread)

find_package(MySQL REQUIRED)
include_directories(${MYSQL_INCLUDE_DIR})
set(LIBS ${LIBS} ${MYSQL_LIBRARIES})

find_package(MySQL++ REQUIRED)
include_directories(${MYSQL++_INCLUDE_DIR})
set(LIBS ${LIBS} ${MYSQL++_LIBRARIES})

find_package(Lua REQUIRED)
include_directories(${LUA_INCLUDE_DIR})
set(LIBS ${LIBS} ${LUA_LIBRARIES})

include_directories(${CMAKE_SOURCE_DIR}/src/Common)

add_subdirectory(src)