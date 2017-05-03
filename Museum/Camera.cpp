#include "Camera.h"

Camera::Camera(vec3 pos, vec3 at)
{
	direction = at - pos;
	direction = normalize(direction);
	up_v = vec3(0, 1, 0); //+ve y direction
	side_v = cross(direction, up_v);
	side_v = normalize(side_v);
	yaw = pitch = 0.0;
	speed = 0.3;

	this->pos = pos;
	this->view_pos = at;
}

Camera::~Camera()
{

}

void Camera::rotate(vec3 angles)
{
	//direction = view_pos - pos; //get current direction
	//GLfloat cosAng = dot(normalize(direction), vec4(1.0, 0.0, 0.0, 0.0));

	//angles.z = cosAng*angles.x;
	////angles.x = (1 - cosAng)*angles.x;
	////std::cout << cosAng << " " << angles.x << " " << angles.y << " " << angles.z << std::endl;
	//direction = (RotateX(angles.x)*RotateY(angles.y)*RotateZ(angles.z))*direction; //rotate direction
	//view_pos = pos + direction; //get new view position from new direction
	
}

void Camera::Pitch(float p)
{
	pitch = p;
}

void Camera::Yaw(float y)
{
	yaw = y;
}

void Camera::SetSpeed(float s)
{
	speed = s;
}

void Camera::Step()
{
	vec3 vdir = direction, vup = up_v, vside = side_v;

	//perform yaw operation
	vside = cross(vup, vdir);
	vside = normalize(vside);
	vdir += vside*yaw;
	vdir = normalize(vdir);

	//perform pitch operation
	vup = cross(vdir, vside);
	vup = normalize(vup);
	vdir += vup*pitch;
	vdir = normalize(vdir);

	up_v = vup;
	side_v = vside;
	direction = vdir;

	view_pos = pos + direction;
	
	yaw = pitch = 0; //reset yaw and pitch values
}

//void Camera::mouseRotate(GLfloat x, GLfloat y)
//{
//	rotate(vec3((y - mousePos.y)*delta, (x - mousePos.x)*delta, 0));
//
//	updateMouse(x, y);
//}

void Camera::move(MOVE_DIR dir)
{
	//This section of code is needed so the camera doesn't move along the y axis. i.e up and down
	//So the camera has a fixed height throughout
	//vec3 vdir = direction;
	//vdir.y = 0.0; //zero y portion of direction vector
	//vdir = normalize(vdir);
	//vec3 vside = normalize(cross(vec3(0.0, 1.0, 0.0), vdir));

	switch (dir)
	{
	case FORWARD:
		pos += direction*speed;
		break;
	case RIGHT:
		pos += -side_v*speed;
		break;
	case BACKWARD:
		pos += -direction*speed;
		break;
	case LEFT:
		pos += side_v*speed;
		break;
	case UP:
		pos.y += 1;
		break;
	case DOWN:
		pos.y -= 1;
	}
	view_pos = pos + direction;
}

vec3 Camera::position()
{
	return pos;
}

vec3 Camera::viewPosition()
{
	return view_pos;
}

//void Camera::updateMouse(GLfloat x, GLfloat y)
//{
//	mousePos.x = x;
//	mousePos.y = y;
//}

vec3 Camera::Up()
{
	return vec3(0.0, 1.0, 0.0);

	//return -normalize(cross(side_v, direction)); Use for flying cameras
}

void Camera::Balance()
{
	pos.y = view_pos.y;
	direction = normalize(view_pos - pos);
	side_v = normalize(cross(vec3(0, 1.0, 0), direction));
}
