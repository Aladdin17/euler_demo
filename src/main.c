#include "gimbal.h"
#include <gl/freeglut.h>

#ifdef BUILD_GUI_EXT
	#include <gui.h>
#endif

static Camera camera;
static Gimbal primary;
static Gimbal target;

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
	primary.rotation[0] = 0.0f;
	primary.rotation[1] = 0.0f;
	primary.rotation[2] = 0.0f;
	primary.drawAxes = false;
	primary.drawRotations = true;
	primary.eulerMode = EULER_MODE_XYZ;
	primary.activeAxis = AXIS_NONE;
	primary.alpha = 1.0f;

	// initialise target gimbal
	target.rotation[0] = 0.0f;
	target.rotation[1] = 0.0f;
	target.rotation[2] = 0.0f;
	target.drawAxes = false;
	target.drawRotations = false;
	target.eulerMode = EULER_MODE_XYZ;
	target.activeAxis = AXIS_NONE;
	target.alpha = 0.3f;
}

void display(void)
{
#ifdef BUILD_GUI_EXT
	// call reshape every frame to ensure the window is always the correct size
	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	gui_update(&primary, &target);
#endif


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
	drawGimbal(&primary);

	// clear the depth buffer to render the target gimbal
	glClear(GL_DEPTH_BUFFER_BIT);
	// enable blending to render the target gimbal transparently
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// draw the target gimbal
	drawGimbal(&target);
	glDisable(GL_BLEND);

#ifdef BUILD_GUI_EXT
	gui_render();
#endif

	glFlush();
}

#ifndef BUILD_GUI_EXT
void keys(unsigned char key, int x, int y)
{
	// 'use' the parameters to avoid warnings
	(void)x, (void)y;

	switch (key)
	{
	case 'a': // fall through
	case 'A':
		primary.drawAxes = !primary.drawAxes;
		break;
	case 'g': // fall through
	case 'G':
		primary.drawRotations = !primary.drawRotations;
		break;
	case 'x':
		primary.activeAxis = AXIS_X;
		primary.rotation[0] += 5.0f;
		if (primary.rotation[0] >= 360.0f)
		{
			primary.rotation[0] -= 360.0f;
		}
		break;
	case 'X':
		primary.activeAxis = AXIS_X;
		primary.rotation[0] -= 5.0f;
		if (primary.rotation[0] <= 360.0f)
		{
			primary.rotation[0] += 360.0f;
		}
		break;
	case 'y':
		primary.activeAxis = AXIS_Y;
		primary.rotation[1] += 5.0f;
		if (primary.rotation[1] >= 360.0f)
		{
			primary.rotation[1] -= 360.0f;
		}
		break;
	case 'Y':
		primary.activeAxis = AXIS_Y;
		primary.rotation[1] -= 5.0f;
		if (primary.rotation[1] <= 360.0f)
		{
			primary.rotation[1] += 360.0f;
		}
		break;
	case 'z':
		primary.activeAxis = AXIS_Z;
		primary.rotation[2] += 5.0f;
		if (primary.rotation[2] >= 360.0f)
		{
			primary.rotation[2] -= 360.0f;
		}
		break;
	case 'Z':
		primary.activeAxis = AXIS_Z;
		primary.rotation[2] -= 5.0f;
		if (primary.rotation[2] <= 360.0f)
		{
			primary.rotation[2] += 360.0f;
		}
		break;
	case '1':
		primary.eulerMode = EULER_MODE_XYZ;
		break;
	case '2':
		primary.eulerMode = EULER_MODE_XZY;
		break;
	case '3':
		primary.eulerMode = EULER_MODE_YXZ;
		break;
	case '4':
		primary.eulerMode = EULER_MODE_YZX;
		break;
	case '5':
		primary.eulerMode = EULER_MODE_ZXY;
		break;
	case '6':
		primary.eulerMode = EULER_MODE_ZYX;
		break;
	case 'r': // fall through
	case 'R':
		primary.rotation[0] = 0.0f;
		primary.rotation[1] = 0.0f;
		primary.rotation[2] = 0.0f;
		break;
	case 'q': // fall through
	case 'Q':
		exit(0);
		break;
	}

	glutPostRedisplay();
}

void keysUp(unsigned char key, int x, int y)
{
	// 'use' the parameters to avoid warnings
	(void)x, (void)y;

	switch (key)
	{
	case 'x': // fall through
	case 'X':
	case 'y': // fall through
	case 'Y':
	case 'z': // fall through
	case 'Z':
		primary.activeAxis = AXIS_NONE;
		break;
	}

	glutPostRedisplay();
}
#endif

void idleFunc( void )
{
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
	glutInitWindowSize(1200, 1200);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Euler Rotation Demo");

	// GLUT callbacks
	glutDisplayFunc(display);
	glutIdleFunc(idleFunc);
#ifndef BUILD_GUI_EXT
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keys);
	glutKeyboardUpFunc(keysUp);
#endif

#ifdef BUILD_GUI_EXT
	gui_init();
#endif

	// world initialization and loop
	init();
	glutMainLoop();

#ifdef BUILD_GUI_EXT
	gui_shutdown();
#endif
}
