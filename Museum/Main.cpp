// Main.cpp : Defines the entry point for the console application.

#include "Angel.h"
#include "Object.h"
#include <string>
#include <vector>
#include"Museum.h"
#include"Camera.h"
#include"SkyBox.h"
#include<cstdlib>

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"SOIL.lib")
#pragma comment(lib, "assimp.lib")

GLfloat speed = 5;// angle we increase the rotation angle on mouse click
int Oldy = 0, Oldx = 256;//for mouse controls
					   //Global projection variables
GLfloat border;//range of ortho projection
GLuint ModelViewLoc, ProjectionLoc;
bool LeftButtonDown = false, MiddleButtonDown = false;
mat4 projection, model_view;
GLfloat projectionAngle = 45.0;
bool ortho = false;//select the perspective matrix to start with
string  projName = "Perspective";//used in the display window title
GLfloat aspect = 1.0, pfar, pnear;//use in perspective projection
GLfloat bmax;// The maximum width of the three directions of the bounding box
vec4 cb;// Center of bounding box obtained from object

//Global directional Light
vec4 light_position(0.0, 0.0, 0.0, 1.0);//infinite light source ie vector
vec3 light_ambient(0.3, .3, .3);
vec3 light_diffuse(1., 1., 1.);
vec3 light_specular(0.4, .4, .4);

// Note that loading all these models takes time
// We could use multiple threads to load each one
// There is also a paper on using CUDA to handle the loading
// Also note that the loaded models better not overfile the capacity of the GPU card

Museum museum;
Camera camera;
SkyBox skybox;
GLuint program;

//Variables to switch between directional and point lights
GLuint UseDirLightLoc;
bool UseDirLight = false;

bool UseFrustumCull = true; //Initially set to frustum cull


//OpenGL initialization
void
init()
{
	skybox.Init("hw_lagoon");

	// Load shaders and use the resulting shader program
	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	museum.Load(program);
	
	//Currently hacking to get the max width and center of museum
	bmax = museum.models[7].MaxWidth();
	cb = museum.models[7].BoxCenter();

	//Initialize camera
	camera = Camera(vec3(0.0, 2, cb.z + bmax), vec3(0.0,2,cb.z));
	//frustum's near and far planes
	pnear = .01;
	pfar = pnear + 10 * -bmax;

	//Move light information over to shader. These are the same for every model
	GLuint LightLoc = glGetUniformLocation(program, "LightPosition");
	glUniform4fv(LightLoc, 1, light_position);
	GLuint LAmbientLoc = glGetUniformLocation(program, "LAmbient");
	glUniform3fv(LAmbientLoc, 1, light_ambient);
	GLuint LDiffuseLoc = glGetUniformLocation(program, "LDiffuse");
	glUniform3fv(LDiffuseLoc, 1, light_diffuse);
	GLuint LSpecularLoc = glGetUniformLocation(program, "LSpecular");
	glUniform3fv(LSpecularLoc, 1, light_specular);

	UseDirLightLoc = glGetUniformLocation(program, "UseDirectional");

	// store the locations for the modelview and projection matrices.  This won't change
	ModelViewLoc = glGetUniformLocation(program, "ModelView");
	ProjectionLoc = glGetUniformLocation(program, "Projection");
	
	// Makes sure objects that are closer to the eye are viewed over objects farther from it
	//Turn it off and see what happens
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);// Set clear color to white
}

/*-------------------------------------------------------------
Frustum Culling using AABB intersection method:
	Requires the projection*modelview matrix, and a vector
	with 8 points defining the bounding box of the object

	Returns true if there is an intersection between the bounding 
	box and the viewing frustum, else false;
-------------------------------------------------------------*/
bool FrustumIntersect(const mat4& PMV, const vector<vec3>& boundBox)
{
	//If frustum culling is disabled, then return true for all objects
	if (UseFrustumCull == false) {
		return true;
	}

	vec4 frustumPt;
	vec3 minBox(FLT_MAX), maxBox(-FLT_MAX), maxIntersect, minIntersect;

	//create new vector with bounding box end points in frustum coordinates
	for (const vec3 pt : boundBox)
	{
		//transform bounding box point
		frustumPt = PMV*pt;

		//normalize transformed point
		frustumPt /= abs(frustumPt.w);

		//get minx and maxs
		minBox.x = min(minBox.x, frustumPt.x);
		minBox.y = min(minBox.y, frustumPt.y);
		minBox.z = min(minBox.z, frustumPt.z);

		maxBox.x = max(maxBox.x, frustumPt.x);
		maxBox.y = max(maxBox.y, frustumPt.y);
		maxBox.z = max(maxBox.z, frustumPt.z);
	}
	//get maximum and minimum points for intersection
	minIntersect.x = max(GLfloat(-1), minBox.x);
	minIntersect.y = max(GLfloat(-1), minBox.y);
	minIntersect.z = max(GLfloat(-1), minBox.z);

	maxIntersect.x = min(GLfloat(1), maxBox.x);
	maxIntersect.y = min(GLfloat(1), maxBox.y);
	maxIntersect.z = min(GLfloat(1), maxBox.z);

	//Check if intersection is a valid box
	if (maxIntersect.x < minIntersect.x || maxIntersect.y < minIntersect.y || maxIntersect.z < minIntersect.z) {
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------
long long frame = 0, curTime, timebase, fps;
void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Step the camera to apply any changes
	camera.Step();

	//Get model view matrix
	model_view = LookAt(camera.position(), camera.viewPosition(), camera.Up());

	//draw skybox first
	skybox.Draw();
	//Draw the museum (and all models in it)
	museum.Draw(GL_TRIANGLES, ModelViewLoc, &FrustumIntersect);

	//Build the window header with time and projection info
	frame++;
	curTime = glutGet(GLUT_ELAPSED_TIME);
	string display_string;
	if (curTime - timebase>1000) {
		fps = frame*1000.0 / (curTime - timebase);
		display_string = "Anthony’s Museum Project: PROJECTION= " + projName + " : FPS= " + to_string(fps);
		glutSetWindowTitle(display_string.c_str());
		timebase = curTime;
		frame = 0;
	}
	glutSwapBuffers();
}


//----------------------------------------------------------------------------
bool wirestate = 1;
void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		// reset the position to the front of the museum
	case 'r':
		camera = Camera(vec3(0.0, 2, cb.z + bmax), vec3(0.0, 2, cb.z - bmax));
		break;

		//Toggle back and forth from wireframe to shading
	case 'L':
		if (wirestate)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		wirestate = (wirestate) ? false : true;
		break;

	//Toggle back and forth from using frustum culling
	case 'f':case 'F':
		UseFrustumCull = !UseFrustumCull;
		if (UseFrustumCull)
			cout << "Frustum culling Enabled" << endl;
		else
			cout << "Frustum culling Disabled" << endl;
		break;

	//Balance the camera.
		//This helps if you rotate the camera too much and need to operate
		//on the same Y level as the position you are looking at
	case 'b': case'B':
		camera.Balance();
		break;

	//Quit the program
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;

	//Toggle back and forth from using directional light to point lights
	case '0':
		UseDirLight = !UseDirLight;
		glUniform1i(UseDirLightLoc, UseDirLight);
		break;
	}

	//Send to museum to handle any extra functions
	museum.keyboardFunction(key, x, y);
}

//----------------------------------------------------------------------------
void
mouse(int button, int state, int x, int y)
{
	Oldx = x; Oldy = y;
	switch (button) {
	case GLUT_LEFT_BUTTON:   LeftButtonDown = (state == GLUT_DOWN) ? TRUE : FALSE; Oldy = y;   break;
	case GLUT_MIDDLE_BUTTON:  MiddleButtonDown = (state == GLUT_DOWN) ? TRUE : FALSE; Oldy = y;  break;
	}
}

void MouseMotion(int x, int y)
{
	// If button1 pressed
	if (LeftButtonDown)
	{
		//Perform some rotations
		camera.Pitch((y - Oldy) / 2000.0);
		camera.Yaw((x - Oldx) / 2000.0);
	}
	if (MiddleButtonDown)
	{
		if (y > Oldy) {
			camera.move(Camera::FORWARD);
		}
		else {
			camera.move(Camera::BACKWARD);
		}
	}
	Oldx = x;
	Oldy = y;
}

//----------------------------------------------------------------------------
void
idle(void)
{
	glutPostRedisplay();
}

//this call back is called on initialization of the window and every time it is resized etc
void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	aspect = GLfloat(width) / height;

	//Get projection matrix and send it over
	projection = Perspective(projectionAngle, aspect, pnear, pfar);
	glUniformMatrix4fv(ProjectionLoc, 1, GL_TRUE, projection);
}


/*----------------------------------------------------------------------------
Left, Right, Forward, Backward, Up, and Down movements in the museum
-----------------------------------------------------------------------------*/
void specialKeyFunction(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		camera.move(Camera::RIGHT);
		break;
	case GLUT_KEY_LEFT:
		camera.move(Camera::LEFT);
		break;
	case GLUT_KEY_UP:
		camera.move(Camera::FORWARD);
		break;
	case GLUT_KEY_DOWN:
		camera.move(Camera::BACKWARD);
		break;
	case GLUT_KEY_PAGE_UP:
		camera.move(Camera::UP);
		break;
	case GLUT_KEY_PAGE_DOWN:
		camera.move(Camera::DOWN);
		break;
	}

}

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	//activate multisampling
	glutSetOption(GLUT_MULTISAMPLE, 4);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 1);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Virtual Museum");
	//More multisampling and smoothing
	glEnable(GLUT_MULTISAMPLE);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);

	glewInit();
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	glutSpecialFunc(specialKeyFunction);
	glutMainLoop();
	return 0;
}
