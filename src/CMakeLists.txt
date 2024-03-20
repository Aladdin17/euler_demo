cmake_minimum_required( VERSION 3.8 )
target_sources(
	${PROJECT_NAME}
	PRIVATE
		main.c
		gimbal.c
		gimbal.h
)

if ( BUILD_GUI_EXT )
	add_subdirectory( gui )
endif()