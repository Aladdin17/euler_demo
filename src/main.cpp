#include "gimbal.h"
#include <imgui.h>
#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl2.h>
#include <gl/freeglut.h>
#include <stdio.h>

static Camera camera;
static Gimbal gimbal;

void setCamera(int width, int height)
{
	GLdouble fov     = 38.0;    // degrees
	GLdouble aspect  = 1.0 * ((GLdouble) width / (GLdouble) height);     // aspect ratio aspect = height/width
	GLdouble nearVal = 0.5;
	GLdouble farVal  = 500.0;
	gluPerspective(fov, aspect, nearVal, farVal);
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// avoid division by zero
	h = (h == 0) ? 1 : h;
	w = (w == 0) ? 1 : w;

	// reset the camera
	setCamera(w, h);
	glMatrixMode(GL_MODELVIEW);
}

void setLight(void)
{
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_COLOR_MATERIAL );
	glShadeModel( GL_SMOOTH );
	glEnable( GL_NORMALIZE );

	GLfloat light_position[] = { 1.0f, 0.0f, 0.0f, 0.0f };
	GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat light_diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat light_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	GLfloat material_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat material_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat material_shininess[] = { 60.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
}

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(5.0);

	// configure depth test and culling
	glEnable (GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );

	// Setting the camera extrinsic parameter (position, lookat and up vector)
	camera.position[0] = 2.5f;
	camera.position[1] = 2.5f;
	camera.position[2] = 2.5f;

	camera.lookat[0] = 0.f;
	camera.lookat[1] = 0.f;
	camera.lookat[2] = 0.f;

	camera.up[0] = 0.f;
	camera.up[1] = 1.f;
	camera.up[2] = 0.f;

	// switch matrix mode to 'projection' and load an identity matrix as the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// setup the camera
	setCamera(
		glutGet(GLUT_WINDOW_WIDTH),
		glutGet(GLUT_WINDOW_HEIGHT)
	);

	// initialise gimbal
	gimbal.rotation[0] = 0.0f;
	gimbal.rotation[1] = 0.0f;
	gimbal.rotation[2] = 0.0f;
	gimbal.drawAxes = false;
	gimbal.drawRotations = true;
	gimbal.eulerMode = XYZ;
}

void display(void)
{
	// call reshape every frame to ensure the window is always the correct size
	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Euler Rotation Demo");
	ImGui::Checkbox("Toggle Axes", &gimbal.drawAxes);
	ImGui::Checkbox("Toggle Rotations", &gimbal.drawRotations);
	const char* items[] = { "XYZ", "XZY", "YXZ", "YZX", "ZXY", "ZYX" };
	static int currentItem = 0;
	static float degPerSecond = 10.0f;
	enum Axis activeAxis = AXIS_NONE;
	if (ImGui::Combo("Mode", &currentItem, items, IM_ARRAYSIZE(items)))
	{
		gimbal.eulerMode = (enum GimbalEulerMode) currentItem;
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
		ImGui::InputFloat(label, &gimbal.rotation[i], 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Positive") || ImGui::IsItemActive())
		{
			activeAxis = (enum Axis) i;
			gimbal.rotation[i] += degPerSecond * ImGui::GetIO().DeltaTime;
			if (gimbal.rotation[i] >= 360.0f)
			{
				gimbal.rotation[i] -= 360.0f;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Negative") || ImGui::IsItemActive())
		{
			activeAxis = (enum Axis) i;
			gimbal.rotation[i] -= degPerSecond * ImGui::GetIO().DeltaTime;
			if (gimbal.rotation[i] < -360.0f)
			{
				gimbal.rotation[i] += 360.0f;
			}
		}
		ImGui::PopID();
	}
	if (ImGui::Button("Reset Rotations"))
	{
		gimbal.rotation[0] = 0.0f;
		gimbal.rotation[1] = 0.0f;
		gimbal.rotation[2] = 0.0f;
	}
	if (ImGui::Button("Quit"))
	{
		glutLeaveMainLoop();
	}
	ImGui::End();
	gimbal.activeAxis = activeAxis;

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set the lighting to be fixed in world-space
	setLight();

	// now setting the camera
	gluLookAt(
		camera.position[0], camera.position[1], camera.position[2],  // eye location
		camera.lookat[0], camera.lookat[1], camera.lookat[2],        // looking at
		camera.up[0], camera.up[1], camera.up[2]                     // up vector
	);

	// draw gimbal and flush
	drawGimbal(&gimbal);

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	glFlush();
}

void keys(unsigned char key, int x, int y)
{
	// 'use' the parameters to avoid warnings
	(void)x, (void)y;

	switch (key)
	{
	case 'a': // fall through
	case 'A':
		gimbal.drawAxes = !gimbal.drawAxes;
		break;
	case 'g': // fall through
	case 'G':
		gimbal.drawRotations = !gimbal.drawRotations;
		break;
	case 'x':
		gimbal.rotation[0] += 5.0f;
		if (gimbal.rotation[0] >= 360.0f)
		{
			gimbal.rotation[0] -= 360.0f;
		}
		break;
	case 'X':
		gimbal.rotation[0] -= 5.0f;
		if (gimbal.rotation[0] <= 360.0f)
		{
			gimbal.rotation[0] += 360.0f;
		}
		break;
	case 'y':
		gimbal.rotation[1] += 5.0f;
		if (gimbal.rotation[1] >= 360.0f)
		{
			gimbal.rotation[1] -= 360.0f;
		}
		break;
	case 'Y':
		gimbal.rotation[1] -= 5.0f;
		if (gimbal.rotation[1] <= 360.0f)
		{
			gimbal.rotation[1] += 360.0f;
		}
		break;
	case 'z':
		gimbal.rotation[2] += 5.0f;
		if (gimbal.rotation[2] >= 360.0f)
		{
			gimbal.rotation[2] -= 360.0f;
		}
		break;
	case 'Z':
		gimbal.rotation[2] -= 5.0f;
		if (gimbal.rotation[2] <= 360.0f)
		{
			gimbal.rotation[2] += 360.0f;
		}
		break;
	case 'q': // fall through
	case 'Q':
		exit(0);
		break;
	}

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	// GLUT initialization
	glutInit(&argc,argv);
#ifdef __FREEGLUT_EXT_H__
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
#endif

	// window size and position
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB  | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Euler Rotation Demo");

	// GLUT callbacks
	glutDisplayFunc(display);
	glutIdleFunc(display);

	// ImGui initialization
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// can't remove this otherwise the program crashes at launch
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.DisplaySize = ImVec2(500, 500);
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplGLUT_Init();
	ImGui_ImplOpenGL2_Init();

	// install the callback funcs
	ImGui_ImplGLUT_InstallFuncs();

	// world initialization and loop
	init();
	glutMainLoop();

	// ImGui shutdown
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();
}
