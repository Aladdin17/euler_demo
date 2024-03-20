#include "gui.h"
#include <gl/freeglut.h>
#include <imgui.h>
#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl2.h>
#include <stdio.h>

void gui_init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// can't remove this otherwise the program crashes at launch
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.DisplaySize = ImVec2(500, 500);

	ImGui_ImplGLUT_Init();
	ImGui_ImplOpenGL2_Init();

	// install the callback funcs
	ImGui_ImplGLUT_InstallFuncs();
}

void gui_update(Gimbal* gimbal)
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Euler Rotation Demo");
	ImGui::Checkbox("Toggle Axes", &gimbal->drawAxes);
	ImGui::Checkbox("Toggle Rotations", &gimbal->drawRotations);
	const char* items[] = { "XYZ", "XZY", "YXZ", "YZX", "ZXY", "ZYX" };
	static int currentItem = 0;
	static float degPerSecond = 10.0f;
	enum Axis activeAxis = AXIS_NONE;
	if (ImGui::Combo("Mode", &currentItem, items, IM_ARRAYSIZE(items)))
	{
		gimbal->eulerMode = (enum EulerMode) currentItem;
	}
	ImGui::SliderFloat("Speed", &degPerSecond, 1.0f, 60.0f);
	for (int i = 0; i < 3; i++)
	{
		static char category[10];
		snprintf(category, 20, "rot_cat_%c", "XYZ"[i]);
		ImGui::PushID(category);
		static char label[3];
		snprintf(label, 3, "##%c", "XYZ"[i]);
		ImGui::Text("%c", "XYZ"[i]);
		ImGui::SameLine();
		ImGui::PushItemWidth(50.0f);
		ImGui::InputFloat(label, &gimbal->rotation[i], 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Positive") || ImGui::IsItemActive())
		{
			activeAxis = (enum Axis) i;
			gimbal->rotation[i] += degPerSecond * ImGui::GetIO().DeltaTime;
			if (gimbal->rotation[i] >= 360.0f)
			{
				gimbal->rotation[i] -= 360.0f;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Negative") || ImGui::IsItemActive())
		{
			activeAxis = (enum Axis) i;
			gimbal->rotation[i] -= degPerSecond * ImGui::GetIO().DeltaTime;
			if (gimbal->rotation[i] < -360.0f)
			{
				gimbal->rotation[i] += 360.0f;
			}
		}
		ImGui::PopID();
	}
	if (ImGui::Button("Reset Rotations"))
	{
		gimbal->rotation[0] = 0.0f;
		gimbal->rotation[1] = 0.0f;
		gimbal->rotation[2] = 0.0f;
	}
	if (ImGui::Button("Quit"))
	{
		glutLeaveMainLoop();
	}
	ImGui::End();
	gimbal->activeAxis = activeAxis;
}

void gui_render()
{
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void gui_shutdown()
{
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();
}
