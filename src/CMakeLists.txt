cmake_minimum_required( VERSION 3.8 )
target_sources(
	${PROJECT_NAME}
	PRIVATE
		main.cpp
		gimbal.cpp
		gimbal.h
		imgui_build.cpp
)
