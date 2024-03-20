#include "gui.h"
#include <gl/freeglut.h>
#include <imgui.h>
#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl2.h>
#include <stdio.h>

bool rotateAxis(Gimbal* gimbal, Axis axis, char* label, float speed)
{
	bool active = false;
	ImGui::PushID(label);
	ImGui::Text(label);
	ImGui::SameLine();
	ImGui::PushItemWidth(100.0f);
	char id[4];
	snprintf(id, sizeof(id), "##%c", *label);
	float angle = gimbal->rotation[axis];
	if (ImGui::DragFloat(id, &angle, 1.0f, 0.0f, 0.0f) || ImGui::IsItemActive())
	{
		active = true;
		if (angle >= 360.0f)
		{
			angle -= 360.0f;
		}
		else if (angle < -360.0f)
		{
			angle += 360.0f;

		}

		gimbal->rotation[axis] = angle;
	}

	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("+") || ImGui::IsItemActive())
	{
		active = true;
		gimbal->rotation[axis] += speed * ImGui::GetIO().DeltaTime;
		if (gimbal->rotation[axis] >= 360.0f)
		{
			gimbal->rotation[axis] -= 360.0f;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("-") || ImGui::IsItemActive())
	{
		active = true;
		gimbal->rotation[axis] -= speed * ImGui::GetIO().DeltaTime;
		if (gimbal->rotation[axis] < -360.0f)
		{
			gimbal->rotation[axis] += 360.0f;
		}
	}
	ImGui::PopID();
	return active;
}

void gui_init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// can't remove this otherwise the program crashes at launch
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.DisplaySize = ImVec2(500, 500);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 4.0f;
	style.FrameRounding = 4.0f;
	style.GrabRounding = 2.0f;
	style.Alpha = 1.0f;
	style.WindowMenuButtonPosition = ImGuiDir_None;

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

	ImGui::SetNextWindowSize(ImVec2(200, 300));
	ImGui::Begin("Euler Rotation Demo");
		ImGui::SeparatorText("Display Options");
		ImGui::Checkbox("Axes", &gimbal->drawAxes);
		ImGui::SameLine(0.0f, 10.0f);
		ImGui::Checkbox("Gizmo", &gimbal->drawRotations);
		ImGui::Spacing();

		// euler mode - first row
		ImGui::SeparatorText("Euler Mode");
		ImGui::RadioButton("XYZ", (int*) &gimbal->eulerMode, EULER_MODE_XYZ);
		ImGui::SameLine(0.0f, 10.0f);
		ImGui::RadioButton("YXZ", (int*) &gimbal->eulerMode, EULER_MODE_YXZ);
		ImGui::SameLine(0.0f, 10.0f);
		ImGui::RadioButton("ZXY", (int*) &gimbal->eulerMode, EULER_MODE_ZXY);
		// euler mode - second row
		ImGui::RadioButton("XZY", (int*) &gimbal->eulerMode, EULER_MODE_XZY);
		ImGui::SameLine(0.0f, 10.0f);
		ImGui::RadioButton("YZX", (int*) &gimbal->eulerMode, EULER_MODE_YZX);
		ImGui::SameLine(0.0f, 10.0f);
		ImGui::RadioButton("ZYX", (int*) &gimbal->eulerMode, EULER_MODE_ZYX);
		ImGui::Spacing();

		static float rotationDegPerSecond = 10.0f;
		enum Axis activeAxis = AXIS_NONE;
		ImGui::SeparatorText("Rotations");
		ImGui::Text(" ");
		ImGui::SameLine();
		ImGui::PushItemWidth(100.0f);
		if (ImGui::DragFloat(" deg/s", &rotationDegPerSecond, 1.0f, 60.0f))
		{
			if (rotationDegPerSecond < 0.0f)
			{
				rotationDegPerSecond = 0.0f;
			}
			else if (rotationDegPerSecond > 360.0f)
			{
				rotationDegPerSecond = 360.0f;
			}
		}
		if (rotateAxis(gimbal, AXIS_X, "X", rotationDegPerSecond))
		{
			activeAxis = AXIS_X;
		}
		if (rotateAxis(gimbal, AXIS_Y, "Y", rotationDegPerSecond))
		{
			activeAxis = AXIS_Y;
		}
		if (rotateAxis(gimbal, AXIS_Z, "Z", rotationDegPerSecond))
		{
			activeAxis = AXIS_Z;
		}
		gimbal->activeAxis = activeAxis;
		ImGui::Spacing();

		ImGui::Separator();
		if (ImGui::Button("Reset Orientation"))
		{
			gimbal->rotation[0] = 0.0f;
			gimbal->rotation[1] = 0.0f;
			gimbal->rotation[2] = 0.0f;
		}
	ImGui::End();
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
