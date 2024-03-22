#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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

enum EulerMode
{
	EULER_MODE_XYZ,
	EULER_MODE_XZY,
	EULER_MODE_YXZ,
	EULER_MODE_YZX,
	EULER_MODE_ZXY,
	EULER_MODE_ZYX
};

typedef struct Gimbal
{
	Vec3 rotation;
	float alpha;
	bool drawRotations;
	bool drawAxes;
	enum EulerMode eulerMode;
	enum Axis activeAxis;
} Gimbal;

void drawGimbal(Gimbal* gimbal);

#ifdef __cplusplus
}
#endif
