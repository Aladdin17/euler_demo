#include "gui.h"
#include <gl/freeglut.h>
#include <imgui.h>
#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl2.h>
#include <stdio.h>
#include <cmath>
#define ANGLE_EPSILON 5e-2f

void helpMarker( const char* desc )
{
	// from ImGui::Demo
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

bool rotateAxis(Gimbal* gimbal, Axis axis, char* label, float speed)
{
	bool active = false;
	ImGui::PushID(label);
	ImGui::Text(label);
	if (ImGui::IsItemHovered())
	{
		if (ImGui::BeginTooltip())
		{
			const char* desc = "[0 , 360]";
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::Text(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(100.0f);
	char id[4];
	snprintf(id, sizeof(id), "##%c", *label);
	float angle = gimbal->rotation[axis];
	if (ImGui::DragFloat(id, &angle, 1.0f, 0.0f, 0.0f) || ImGui::IsItemActive())
	{
		active = true;
		if (angle >= 180.0f)
		{
			angle -= 360.0f;
		}
		else if (angle < -180.0f)
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
		if (gimbal->rotation[axis] >= 180.0f)
		{
			gimbal->rotation[axis] -= 360.0f;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("-") || ImGui::IsItemActive())
	{
		active = true;
		gimbal->rotation[axis] -= speed * ImGui::GetIO().DeltaTime;
		if (gimbal->rotation[axis] < -180.0f)
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
	io.DisplaySize = ImVec2(1200, 1200);

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
	ImGui::SetNextWindowPos(ImVec2(50, 50));
	ImGui::Begin("Euler Rotation Demo");
		ImGui::SeparatorText("Display Options");
		ImGui::Checkbox("Axes", &gimbal->drawAxes);
		ImGui::SameLine(0.0f, 10.0f);
		ImGui::Checkbox("Gizmo", &gimbal->drawRotations);
		ImGui::Spacing();

		// euler mode - first row
		ImGui::SeparatorText("Euler Mode");
		if (ImGui::IsItemHovered())
		{
			if (ImGui::BeginTooltip())
			{
				const char* desc = " \
Determines the order that the rotations are applied.\n \
E.g., XYZ will apply X rotation then Y, then Z.\n \
Each rotation is modified by the preceeding rotations\n \
so the order of the rotations can change the final outcome.";
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::Text(desc);
				ImGui::PopTextWrapPos();
			}
			ImGui::EndTooltip();
		}

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
			if (ImGui::IsItemHovered())
		{
			if (ImGui::BeginTooltip())
			{
				const char* desc = " \
The following boxes can be interacted with by clicking and dragging left/right; \
they can also be Ctrl + Clicked to input a value directly.\n \
The + and - buttons will rotate the selected axis by the chosen speed; \
the direction of rotation based on a right-hand coordinate system.";
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::Text(desc);
				ImGui::PopTextWrapPos();
			}
			ImGui::EndTooltip();
		}
		ImGui::Text(" ");
		ImGui::SameLine();
		ImGui::PushItemWidth(100.0f);
		if (ImGui::DragFloat("##rotSpeed", &rotationDegPerSecond, 1.0f, 60.0f))
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
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("deg/s");
		if (ImGui::IsItemHovered())
		{
			if (ImGui::BeginTooltip())
			{
				const char* desc = "[0 , 60]";
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::Text(desc);
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
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
		static bool animate = false;
		static float target[] = { 0.0f, 0.0f, 0.0f };
		if (ImGui::Button("Reset Orientation"))
		{
			animate = true;
			// gimbal->rotation[0] = 0.0f;
			// gimbal->rotation[1] = 0.0f;
			// gimbal->rotation[2] = 0.0f;
		}

		if ( animate )
		{
			// calculate the difference between the current rotation and the target
			float diff[3] = { target[0] - gimbal->rotation[0], target[1] - gimbal->rotation[1], target[2] - gimbal->rotation[2] };

			// if the magnitude of the difference is zero then we are done
			float mag = sqrtf(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
			if ( mag <= ANGLE_EPSILON || (rotationDegPerSecond <= ANGLE_EPSILON ))
			{
				// stop animation and reset the target
				animate = false;
				gimbal->rotation[0] = target[0];
				gimbal->rotation[1] = target[1];
				gimbal->rotation[2] = target[2];
			}
			else
			{
				float dir[] = {
					diff[0] >= 180.0f ? -1.0f : (diff[0] < 0.0f ? -1.0f : 1.0f),
					diff[1] >= 180.0f ? -1.0f : (diff[1] < 0.0f ? -1.0f : 1.0f),
					diff[2] >= 180.0f ? -1.0f : (diff[2] < 0.0f ? -1.0f : 1.0f)
				};

				// rotate the gimbal towards the target
				gimbal->rotation[0] += dir[0] * rotationDegPerSecond * ImGui::GetIO().DeltaTime;
				gimbal->rotation[1] += dir[1] * rotationDegPerSecond * ImGui::GetIO().DeltaTime;
				gimbal->rotation[2] += dir[2] * rotationDegPerSecond * ImGui::GetIO().DeltaTime;
			}
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
