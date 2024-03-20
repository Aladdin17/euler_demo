cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(
	imgui
	DESCRIPTION "Imgui library"
	LANGUAGES CXX
)

add_library(imgui STATIC)
target_include_directories(
	imgui PUBLIC
		${CMAKE_CURRENT_LIST_DIR}
)

target_sources(
	imgui PRIVATE
	imconfig.h
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_internal.h
	imgui_tables.cpp
	imgui_widgets.cpp
	imgui.cpp
	imgui.h
	imstb_rectpack.h
	imstb_textedit.h
	imstb_truetype.h
)
