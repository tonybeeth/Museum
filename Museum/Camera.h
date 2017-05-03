//Camera.h: Header file for Camera class

#ifndef CAMERA_H
#define CAMERA_H

#include"Angel.h"

#pragma once
class Camera
{
private:
	//camera position
	vec3 pos;
	//up and side vectors
	vec3 up_v, side_v;
	//position being looked at
	vec3 view_pos;
	//viewing direction vector
	vec3 direction;

	GLfloat delta = 360/1500.0;
	GLfloat pitch, yaw, speed;

public:
	static enum MOVE_DIR {FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN};

	Camera(vec3 = vec3(0, 0, 0), vec3 = vec3(0, 0, -10));

	//void mouseRotate(GLfloat x, GLfloat y);
	void move(MOVE_DIR);
	vec3 position();
	vec3 viewPosition();
	//void updateMouse(GLfloat x, GLfloat y);
	void rotate(vec3);
	void Pitch(float p);
	void Yaw(float r);
	void Step();
	void SetSpeed(float s);
	vec3 Up();
	void Balance(); //sets the camera's y position to the same as the view position's

	~Camera();
};

#endif // !CAMERA_H
