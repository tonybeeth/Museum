
#ifndef CAMERA_H
#define CAMERA_H

#include"Angel.h"

#pragma once
class Camera
{
private:
	vec3 pos;
	vec3 up_v, side_v;
	vec3 view_pos;
	vec3 direction;
	GLfloat angle;
	vec2 mousePos;
	GLfloat delta = 360/1500.0;
	GLfloat pitch, yaw, speed;

public:
	static enum MOVE_DIR {FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN};

	Camera(vec3 = vec3(0, 0, 0), vec3 = vec3(0, 0, -10));

	void mouseRotate(GLfloat x, GLfloat y);
	void move(MOVE_DIR);
	vec3 position();
	vec3 viewPosition();
	void updateMouse(GLfloat x, GLfloat y);
	void rotate(vec3);
	void Pitch(float p);
	void Yaw(float r);
	void Step();
	void SetSpeed(float s);
	vec3 Up();

	~Camera();
};

#endif // !CAMERA_H
