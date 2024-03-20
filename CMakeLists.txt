cmake_minimum_required( VERSION 3.8 )
project(
	euler_demo
	LANGUAGES C CXX
)

set( CMAKE_POLICY_DEFAULT_CMP0076 NEW )

# directories
set( BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>" )

# set the freeglut directories
set( FREEGLUT_INC_DIR "${CMAKE_SOURCE_DIR}/freeglut/include" )
set( FREEGLUT_LIB_DIR "${CMAKE_SOURCE_DIR}/freeglut/lib" )
set( IMGUI_DIR "${CMAKE_SOURCE_DIR}/imgui" )

# add executable target
add_executable( euler-demo )
set_target_properties(
	euler-demo PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
	# set the working directory to the binary directory
	VS_DEBUGGER_WORKING_DIRECTORY "${BIN_DIR}"
)

target_compile_options(
	euler-demo
	PRIVATE
		$<$<CXX_COMPILER_ID:GNU>: -Wall -Wextra -Wpedantic>	# set gcc compiler warnings
		$<$<CXX_COMPILER_ID:GNU>: -std=c11> # set c11 standard
		$<$<CXX_COMPILER_ID:GNU>: -Wno-unknown-pragmas> # disable warnings from external headers

		$<$<CXX_COMPILER_ID:MSVC>: /W4> # set MSVC compiler warnings
		$<$<CXX_COMPILER_ID:MSVC>: /std:c11> # set c11 standard
		$<$<CXX_COMPILER_ID:MSVC>: /external:anglebrackets /external:W0> # disable warnings from external headers
)

option( BUILD_GUI_EXT "Build the GUI extension" ON )
if( BUILD_GUI_EXT )
	target_compile_definitions(
		euler-demo
		PRIVATE BUILD_GUI_EXT
	)
	set_target_properties(
		euler-demo
		PROPERTIES
			OUTPUT_NAME "euler-demo-gui"
	)

	add_library(
		gui-extension
		STATIC
	)

	target_compile_definitions(
		gui-extension
		PRIVATE
			FREEGLUT
	)

	# add imgui library and link
	add_subdirectory( ${IMGUI_DIR} )
	target_link_libraries( gui-extension PRIVATE imgui )

	# add freeglut library and link
	target_link_directories( gui-extension PUBLIC ${FREEGLUT_LIB_DIR} )
	target_include_directories( gui-extension PUBLIC ${FREEGLUT_INC_DIR} )
	target_link_libraries( gui-extension PUBLIC freeglut.lib glu32.lib opengl32.lib )

	# add gimbal header and extension source
	target_include_directories( gui-extension PRIVATE ${CMAKE_SOURCE_DIR}/src )
	add_subdirectory( gui-src )

	# link the gui-extension library to the euler-demo executable
	target_link_libraries( euler-demo PRIVATE gui-extension )
else()
	# find and link the FreeGLUT library
	target_link_directories( euler-demo PRIVATE ${FREEGLUT_LIB_DIR} )
	target_include_directories( euler-demo PRIVATE ${FREEGLUT_INC_DIR} )
	target_link_libraries( euler-demo PRIVATE freeglut.lib glu32.lib opengl32.lib )
endif()

# copy dll to binary folder
add_custom_command(
	TARGET euler-demo
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${FREEGLUT_LIB_DIR}/freeglut.dll"
		"${BIN_DIR}/freeglut.dll"
)

# add project subdirectories
add_subdirectory( src )
