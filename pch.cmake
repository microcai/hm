cmake_minimum_required(VERSION 2.8)

# 创建预编译头
# Target是用来生成预编译头的项目Target；PrecompiledHeader和PrecompiledSource分别是头文件的路径

# add_pch( target_name stable.h stable.c )
# where: - stable.h is a header that should be precompiled.
#        - stable.c is a dummy module that contains only directive to include stable.h (it is
#          required for proper dependency resolution to trigger recompilation of PCH).
# Notes: - Only works for GCC and compatible compilers (e.g. LLVM/Clang).
#        - Compiler flags are retrieved from CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_BUILDTYPE.
#          Include directories added via include_directories and defines added via add_definitions
#          are also added to compiler flags.
macro( add_pch_gcc _targetName _inputHeader )
  # Extract CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_XXX for the current configuration XXX.
  string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _build_type_flags_var )
  set( _flags "${CMAKE_CXX_FLAGS} ${${_build_type_flags_var}}" )

  # Convert string of space separated flags into a list.
  separate_arguments( _flags )

  # Extract include directories set by include_directories command.
  get_directory_property( _includes INCLUDE_DIRECTORIES )
  foreach( _include ${_includes} )
    list( APPEND _flags "-I${_include}" )
  endforeach()

  # Extract definitions set by add_definitions command.
  get_directory_property( _defines COMPILE_DEFINITIONS )
  foreach( _define ${_defines} )
    list( APPEND _flags "-D${_define}" )
  endforeach()

  # Build precompiled header and copy original header to the build folder - GCC wants it there.
  add_custom_command( OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}.gch"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}"
    COMMAND "${CMAKE_CXX_COMPILER}" ${_flags} -x c++-header "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}"
	WORKING_DIRECTORY	"${CMAKE_CURRENT_SOURCE_DIR}"
    )
  add_custom_target( ${_targetName} DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}.gch" )
endmacro()

macro( add_pch_clang _targetName _inputHeader )
  # Extract CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_XXX for the current configuration XXX.
  string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _build_type_flags_var )
  set( _flags "${CMAKE_CXX_FLAGS} ${${_build_type_flags_var}}" )

  # Convert string of space separated flags into a list.
  separate_arguments( _flags )

  # Extract include directories set by include_directories command.
  get_directory_property( _includes INCLUDE_DIRECTORIES )
  foreach( _include ${_includes} )
    list( APPEND _flags "-I${_include}" )
  endforeach()

  # Extract definitions set by add_definitions command.
  get_directory_property( _defines COMPILE_DEFINITIONS )
  foreach( _define ${_defines} )
    list( APPEND _flags "-D${_define}" )
  endforeach()

  # Build precompiled header and copy original header to the build folder - GCC wants it there.
  add_custom_command( OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}.pch"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}"
    COMMAND "${CMAKE_CXX_COMPILER}" ${_flags} -x c++-header "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}"
	WORKING_DIRECTORY	"${CMAKE_CURRENT_SOURCE_DIR}"
    )
  add_custom_target( ${_targetName} DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}.gch" )
endmacro()

macro( add_pch _targetName _inputHeader )
	if(CMAKE_COMPILER_IS_GNUCXX)
	add_pch_gcc( _targetName _inputHeader)
	#elseif()
	endif()
endmacro()