cmake_minimum_required(VERSION 2.8)
project(hotelmgr CXX)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

include(pch.cmake)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_CXX_COMPILER "g++")
SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=gnu++11 ")

#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(Boost_USE_MULTITHREADED      OFF)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_STATIC_RUNTIME    ON)

add_definitions(-DBOOST_SYSTEM_NO_DEPRECATED)

find_package( Boost 1.51 COMPONENTS date_time filesystem regex system)


find_package( CryptoPP )

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_libraries(${Boost_LIBRARIES})
    if(UNIX) # windows 下就算了
		link_libraries("-static-libgcc -static-libstdc++")
	endif()
endif()

if(CRYPTOPP_FOUND)
	include_directories(${CRYPTOPP_INCLUDE_DIRS})
	link_libraries(${CRYPTOPP_LIBRARIES})
endif()

SET(SOURCES
	book.cpp
	client.cpp
	cgi.cpp
	gc.cpp
	httpd.cpp
	httpfile.cpp
	init.cpp
	main.cpp
	roomid.cpp
	status.cpp
	shell.cpp
)

include_directories("lib")
add_subdirectory(lib)

add_executable(hm ${SOURCES}  $<TARGET_OBJECTS:libhm>)

add_pch(hm_pch pch.hpp)

ADD_DEPENDENCIES(hm hm_pch)

ADD_DEPENDENCIES(libhm hm_pch)


add_subdirectory(php)