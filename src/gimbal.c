#include "gimbal.h"
#include <math.h>
#include <gl/freeglut.h>

//--------------------------------------------------------------------------------------------------
// defines
//--------------------------------------------------------------------------------------------------
#define PI 3.14159265358979323846f

//--------------------------------------------------------------------------------------------------
// prototypes
//--------------------------------------------------------------------------------------------------

static void drawArrow( float );
static void drawAxes( GLfloat, GLfloat, float );
static void drawCube( GLfloat[3] );
static void drawCircle( float, int, float[3], bool, float );
static void drawCube( GLfloat[3] );
static void rotateEntity( Gimbal*, enum Axis );

//--------------------------------------------------------------------------------------------------
// functions
//--------------------------------------------------------------------------------------------------

void drawArrow(float alpha)
{
	GLUquadricObj *quadric;

	// Create a new quadric
	quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);

	// arrow shaft
	glPushMatrix();
	glColor4f( 0.5f, 0.5f, 0.5f, alpha );
	glTranslatef( 0.0f, 0.0f, 0.3f );
	drawCube( (GLfloat[]){ 0.15f, 0.15f, 0.6f } );
	glPopMatrix();

	// arrow head
	glPushMatrix();
	glColor4f(0.0f, 1.0f, 0.0f, alpha);  // Green color
	glTranslatef(0.0f, 0.0f, 0.6f);  // Move to the end of the shaft
	gluCylinder(quadric, 0.2, 0.0, 0.4, 32, 32);  // Cone shape
	glPopMatrix();

	// draw arrow head bottom
	glPushMatrix();
	glColor4f( 0.0, 1.0f, 0.0f , alpha);
	glTranslatef(0.0f, 0.0f, 0.6f);
	glRotatef( 180.0f, 1.0f, 0.0f, 0.0f );
	gluDisk(quadric, 0.0, 0.2, 32, 32);
	glPopMatrix();

	// clean-up
	gluDeleteQuadric(quadric);
}

void drawAxes(GLfloat lineWidth, GLfloat length, float alpha)
{
	// disable lighting
	GLboolean isLightingEnabled = glIsEnabled(GL_LIGHTING);
	glDisable(GL_LIGHTING);

	// set new line width
	GLfloat currentWidth;
	glGetFloatv(GL_LINE_WIDTH, &currentWidth);
	glLineWidth(lineWidth);

	// x-axis
	glBegin(GL_LINES);
		glColor4f(1,0,0, alpha);
		glVertex3f(0,0,0);
		glVertex3f(length,0,0);
	glEnd();

	// y-axis
	glBegin(GL_LINES);
		glColor4f(0,1,0, alpha);
		glVertex3f(0,0,0);
		glVertex3f(0,length,0);
	glEnd();

	// z-axis
	glBegin(GL_LINES);
		glColor4f(0,0,1, alpha);
		glVertex3f(0,0,0);
		glVertex3f(0,0,length);
	glEnd();

	// reset line width
	glLineWidth(currentWidth);

	// reset lighting
	if (isLightingEnabled)
	{
		glEnable(GL_LIGHTING);
	}
}

void drawCircle( float radius, int segments, float color[3], bool active, float alpha )
{
	// disable lighting
	GLboolean isLightingEnabled = glIsEnabled(GL_LIGHTING);
	glDisable(GL_LIGHTING);

	float width;
	glGetFloatv( GL_LINE_WIDTH, &width );
	glLineWidth( active ? 8.0f : 4.0f );
	glBegin( GL_LINE_LOOP );
	for (int i = 0; i < segments; i++)
	{
		float theta = 2.0f * PI * (float)i / (float)segments;
		float x = radius * cosf(theta);
		float y = radius * sinf(theta);
		glColor4f(color[0], color[1], color[2], alpha);
		glVertex2f(x, y);
	}
	glEnd();
	glLineWidth( width );

	// reset lighting
	if (isLightingEnabled)
	{
		glEnable(GL_LIGHTING);
	}
}

void drawCube( GLfloat scale[3] )
{
	enum CubeFaces {
		CUBE_FRONT,
		CUBE_BACK,
		CUBE_LEFT,
		CUBE_RIGHT,
		CUBE_TOP,
		CUBE_BOTTOM
	};

	// looking from in front of the cube down the negative z-axis
	// the vertices are defined in a counter-clockwise order starting at
	// the bottom-left-front corner
	Vec3 vertices[8]= {
		{ -0.5f, -0.5f,  0.5f },  // front bottom left
		{  0.5f, -0.5f,  0.5f }, // front bottom right
		{  0.5f,  0.5f,  0.5f }, // front top right
		{ -0.5f,  0.5f,  0.5f }, // front top left

		{ -0.5f, -0.5f, -0.5f }, // back bottom left
		{  0.5f, -0.5f, -0.5f }, // back bottom right
		{  0.5f,  0.5f, -0.5f }, // back top right
		{ -0.5f,  0.5f, -0.5f }  // back top left
	};

	Vec3 normals[6] = {
		{  0,  0,  1 }, // front
		{  0,  0, -1 }, // back
		{ -1,  0,  0 }, // left
		{  1,  0,  0 }, // right
		{  0,  1,  0 }, // top
		{  0, -1,  0 }  // bottom
	};

	// we need to enable normalisation because we are scaling the cube
	bool isNormalsEnabled = glIsEnabled( GL_NORMALIZE );
	glEnable( GL_NORMALIZE );

	glPushMatrix();
	glScalef( scale[0], scale[1], scale[2] );
	glBegin( GL_QUADS ); // front
		glNormal3fv(normals[CUBE_FRONT]);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[2]);
		glVertex3fv(vertices[3]);
	glEnd();

	glBegin( GL_QUADS ); // back
		glNormal3fv(normals[CUBE_BACK]);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[4]);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[6]);
	glEnd();

	glBegin( GL_QUADS ); // left
		glNormal3fv(normals[CUBE_LEFT]);
		glVertex3fv(vertices[4]);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[7]);
	glEnd();

	glBegin( GL_QUADS ); // right
		glNormal3fv(normals[CUBE_RIGHT]);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[6]);
		glVertex3fv(vertices[2]);
	glEnd();

	glBegin( GL_QUADS ); // top
		glNormal3fv(normals[CUBE_TOP]);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[2]);
		glVertex3fv(vertices[6]);
		glVertex3fv(vertices[7]);
	glEnd();

	glBegin( GL_QUADS ); // bottom
		glNormal3fv(normals[CUBE_BOTTOM]);
		glVertex3fv(vertices[4]);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[0]);
	glEnd();

	glPopMatrix();

	// reset normalisation
	if ( !isNormalsEnabled )
	{
		glDisable( GL_NORMALIZE );
	}
}

void drawGimbal(Gimbal* gimbal)
{
	glPushMatrix();

	switch (gimbal->eulerMode)
	{
	case EULER_MODE_XYZ:
		rotateEntity( gimbal, AXIS_Z );
		rotateEntity( gimbal, AXIS_Y );
		rotateEntity( gimbal, AXIS_X );
		break;
	case EULER_MODE_XZY:
		rotateEntity( gimbal, AXIS_Y );
		rotateEntity( gimbal, AXIS_Z );
		rotateEntity( gimbal, AXIS_X );
		break;
	case EULER_MODE_YXZ:
		rotateEntity( gimbal, AXIS_Z );
		rotateEntity( gimbal, AXIS_X );
		rotateEntity( gimbal, AXIS_Y );
		break;
	case EULER_MODE_YZX:
		rotateEntity( gimbal, AXIS_X );
		rotateEntity( gimbal, AXIS_Z );
		rotateEntity( gimbal, AXIS_Y );
		break;
	case EULER_MODE_ZXY:
		rotateEntity( gimbal, AXIS_Y );
		rotateEntity( gimbal, AXIS_X );
		rotateEntity( gimbal, AXIS_Z );
		break;
	case EULER_MODE_ZYX:
		rotateEntity( gimbal, AXIS_X );
		rotateEntity( gimbal, AXIS_Y );
		rotateEntity( gimbal, AXIS_Z );
		break;
	}

	if (gimbal->drawAxes)
	{
		drawAxes(0.5f, 2.0f, gimbal->alpha);
	}

	drawArrow(gimbal->alpha);
	glPopMatrix();
}

void rotateEntity( Gimbal* gimbal, enum Axis axis )
{
	float rotaxis[3] = {0.0f, 0.0f, 0.0f};
	rotaxis[axis] = 1.0f;

	if ( axis == AXIS_X )
	{
		glRotatef(gimbal->rotation[0], 1.0f, 0.0f, 0.0f);
	}
	else if ( axis == AXIS_Y )
	{
		glRotatef(gimbal->rotation[1], 0.0f, 1.0f, 0.0f);
	}
	else
	{
		glRotatef(gimbal->rotation[2], 0.0f, 0.0f, 1.0f);
	}

	if ( gimbal->drawRotations )
	{
		glPushMatrix();
		if ( axis == AXIS_X )
		{
			glRotatef ( 90.0f, 0, 1, 0 );
		}
		else if ( axis == AXIS_Y )
		{
			glRotatef ( 90.0f, 1, 0, 0 );
		}
		else
		{
			glRotatef ( 90.0f, 0, 0, 1 );
		}

		if ( gimbal->activeAxis == axis )
		{
			drawCircle( 1.0f, 30, rotaxis, true, gimbal->alpha );
		}
		else
		{
			drawCircle( 1.0f, 30, rotaxis, false, gimbal->alpha );
		}

		glPopMatrix();
	}
}
