#pragma once
#include <stdbool.h>

typedef float Vec3[3];
typedef struct Camera
{
	Vec3 position;
	Vec3 lookat;
	Vec3 up;
} Camera;

enum Axis
{
	AXIS_X = 0,
	AXIS_Y = 1,
	AXIS_Z = 2,
	AXIS_NONE = 3
};

enum GimbalEulerMode
{
	XYZ,
	XZY,
	YXZ,
	YZX,
	ZXY,
	ZYX
};

typedef struct Gimbal
{
	Vec3 rotation;
	bool drawRotations;
	bool drawAxes;
	enum GimbalEulerMode eulerMode;
	enum Axis activeAxis;
} Gimbal;


void drawGimbal(Gimbal* gimbal);
