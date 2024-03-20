// #include "geometry.h"
// #include "drawing_objects.h"
#include "gimbal.h"
#include <gl/freeglut.h>
// #include <imgui.h>
// #include <backends/imgui_impl_opengl2.h>
// #include <backends/imgui_impl_glut.h>

static Camera camera;
static Gimbal gimbal;

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

void setCamera(int width, int height)
{
	GLdouble fov     = 38.0;    // degrees
	GLdouble aspect  = 1.0 * ((GLdouble) width / (GLdouble) height);     // aspect ratio aspect = height/width
	GLdouble nearVal = 0.5;
	GLdouble farVal  = 500.0;
	gluPerspective(fov, aspect, nearVal, farVal);
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

int main(int argc, char** argv)
{
	// GLUT initialization
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB  | GLUT_DEPTH);

	// window size and position
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Euler Rotation Demo");

	// GLUT callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keys);

	// world initialization and loop
	init();
	glutMainLoop();
}
