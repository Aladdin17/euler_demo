cmake_minimum_required( VERSION 3.8 )
target_sources(
	gui-extension
	PRIVATE
		gui.cpp
		gui.h
		build.cpp
)

target_include_directories(
	gui-extension
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/gui
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
)
