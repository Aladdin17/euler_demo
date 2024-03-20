cmake_minimum_required( VERSION 3.8 )
project(
	euler_demo
	LANGUAGES C
)

set( CMAKE_POLICY_DEFAULT_CMP0076 NEW )

# directories
set( BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>" )

# set the freeglut directories
if( NOT DEFINED FREEGLUT_INC_DIR )
	set( FREEGLUT_INC_DIR "${CMAKE_SOURCE_DIR}/freeglut/include" CACHE STRING "Path to FreeGLUT headers." )
endif()
if( NOT DEFINED FREEGLUT_LIB_DIR )
	set( FREEGLUT_LIB_DIR "${CMAKE_SOURCE_DIR}/freeglut/lib" CACHE STRING "Path to FreeGLUT precompiled libs." )
endif()

# add executable target
add_executable( ${PROJECT_NAME} )
set_target_properties(
	${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
	# set the working directory to the binary directory
	VS_DEBUGGER_WORKING_DIRECTORY "${BIN_DIR}"
)

target_compile_options(
	${PROJECT_NAME}
	PRIVATE
		$<$<CXX_COMPILER_ID:GNU>: -Wall -Wextra -Wpedantic>	# set gcc compiler warnings
		$<$<CXX_COMPILER_ID:GNU>: -std=c11> # set c11 standard
		$<$<CXX_COMPILER_ID:GNU>: -Wno-unknown-pragmas> # disable warnings from external headers

		$<$<CXX_COMPILER_ID:MSVC>: /W4> # set MSVC compiler warnings
		$<$<CXX_COMPILER_ID:MSVC>: /std:c11> # set c11 standard
		$<$<CXX_COMPILER_ID:MSVC>: /external:anglebrackets /external:W0> # disable warnings from external headers
)

# find and link the FreeGLUT library
set( OpenGL_GL_PREFERENCE "LEGACY" )
find_package( FreeGLUT QUIET )
if( FreeGLUT_FOUND )
	message( STATUS "Linking FreeGLUT." )
	target_link_libraries( ${PROJECT_NAME} glut GLU GL )
else()
	# termintate the build if FreeGLUT is not found and we are not on Windows
	# the libraries have not been provided for other platforms
	if( NOT WIN32 )
		message ( FATAL_ERROR "FreeGLUT not found: Please install from your package manager. " )
	endif()

	# check that freeglut exists locally
	if( NOT IS_DIRECTORY ${FREEGLUT_LIB_DIR} )
		message( FATAL_ERROR "FreeGLUT library directory not found: Please set the FREEGLUT_LIB_DIR variable to the directory containing precompiled freeglut libs." )
	endif()

	if( NOT IS_DIRECTORY ${FREEGLUT_INC_DIR} )
		message( FATAL_ERROR "FreeGLUT include directory not found: Please set the FREEGLUT_INC_DIR variable to the directory containing freeglut header files." )
	endif()

	# sets a relative link path to this cmake file
	message( STATUS "Linking FreeGLUT." )
	target_link_directories( ${PROJECT_NAME} PRIVATE ${FREEGLUT_LIB_DIR} )
	target_include_directories( ${PROJECT_NAME} PRIVATE ${FREEGLUT_INC_DIR} )
	target_link_libraries( ${PROJECT_NAME} PRIVATE freeglut.lib glu32.lib opengl32.lib )

	# copy dll to binary folder
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${FREEGLUT_LIB_DIR}/freeglut.dll"
		"${BIN_DIR}/freeglut.dll"
	)
endif()

# add project subdirectories
add_subdirectory( src )
