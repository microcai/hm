cmake_minimum_required(VERSION 2.8)

#=============================================================
# macros
#####################################################
# 创建预编译头
# Target是用来生成预编译头的项目Target；PrecompiledHeader和PrecompiledSource分别是头文件的路径1�7
MACRO(ADD_PRECOMPILED_HEADER  PrecompiledHeader)

  IF(CMAKE_COMPILER_IS_GNUCXX)
	ADD_GCC_PRECOMPILED_HEADER(${PrecompiledHeader})
  ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDMACRO(ADD_PRECOMPILED_HEADER)

# 改函数用于判断gcc是否支持预编译头
IF(CMAKE_COMPILER_IS_GNUCXX)
  EXEC_PROGRAM(
	${CMAKE_CXX_COMPILER}
	ARGS					--version
	OUTPUT_VARIABLE _compiler_output)
  STRING(REGEX REPLACE ".* ([0-9]\\.[0-9]\\.[0-9]) .*" "\\1"
	gcc_compiler_version ${_compiler_output})
  #MESSAGE("GCC Version: ${gcc_compiler_version}")
  IF(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
	SET(PCHSupport_FOUND TRUE)
  ELSE(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
	IF(gcc_compiler_version MATCHES "3\\.4\\.[0-9]")
	  SET(PCHSupport_FOUND TRUE)
	ENDIF(gcc_compiler_version MATCHES "3\\.4\\.[0-9]")
  ENDIF(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

MACRO(ADD_GCC_PRECOMPILED_HEADER PrecompileHeader)
  SET(_compile_FLAGS ${CMAKE_CXX_FLAGS})

  SET(_input "${CMAKE_CURRENT_SOURCE_DIR}/${PrecompileHeader}")

  GET_FILENAME_COMPONENT(_name ${_input} NAME)
  GET_FILENAME_COMPONENT(_path ${_input} PATH)

  MESSAGE("creating pch: ${_input}")

  SET(_output "${CMAKE_CURRENT_BINARY_DIR}/${_name}.gch")

  GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
  GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)

  #LIST(APPEND _compile_FLAGS "-fPIC")
  LIST(APPEND _compile_FLAGS ${_directory_flags})

  SEPARATE_ARGUMENTS(_compile_FLAGS)
  SET(INSOURCE 0)

  if(CMAKE_CURRENT_SOURCE_DIR STREQUAL  CMAKE_CURRENT_BINARY_DIR)
  SET(INSOURCE 1)
  endif()

  if(NOT  INSOURCE)
  # 拷贝头文件到binary目录
  ADD_CUSTOM_COMMAND(
	OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${_name}
	COMMAND ${CMAKE_COMMAND} -E copy  ${_input}
	${CMAKE_CURRENT_BINARY_DIR}/${_name}
	)
 endif()

 SET(PCH_PATH ${CMAKE_CURRENT_BINARY_DIR}/${_name} CACHE INTERNAL
	"the path of the precompiled header")

  #MESSAGE("command : ${CMAKE_COMMAND} -E copy  ${_input}
  #  ${CMAKE_CURRENT_BINARY_DIR}/${_name}")

  # 添加用于生成预编译头的命令
 ADD_CUSTOM_COMMAND(
	OUTPUT ${_output}
	COMMAND ${CMAKE_CXX_COMPILER}
	${_compile_FLAGS}
	-x c++-header
	-o ${_output}
	${_input}
	DEPENDS /${_input}
	)
 ADD_CUSTOM_TARGET(${PrecompileHeader}_gch
	DEPENDS ${_output}
	)
ENDMACRO(ADD_GCC_PRECOMPILED_HEADER)


#=============================================================
# start here
#####################################################
INCLUDE (CheckFunctionExists)
project(hotelmgr CXX)

SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=gnu++11 ")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    ON)

find_package( Boost 1.48 COMPONENTS  signals date_time program_options filesystem regex chrono locale system thread)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_libraries(${Boost_LIBRARIES})
    # fix static build
	if(UNIX) # windows 下就算了
		link_libraries(-pthread -lrt)
	endif()
endif()


SET(SOURCES
	book.cpp
	client.cpp
	common.cpp
	gc.cpp
	httpd.cpp
	httpfile.cpp
	init.cpp
	main.cpp
	roomid.cpp
	status.cpp
	shell.cpp
)

add_executable(hm ${SOURCES})

add_precompiled_header(pch.hpp)

ADD_DEPENDENCIES(hm pch.hpp_gch)
