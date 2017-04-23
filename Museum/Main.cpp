// ObjViewer.cpp : Defines the entry point for the console application.
//

#include "Angel.h"
#include "Object.h"
#include <string>
#include <vector>
#include "soil.h"
#include <set>
#include"Museum.h"
#include"Camera.h"
#include"SkyBox.h"
#include<ctime>
#include<cstdlib>
#include"GLSLProgram.h"


#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"SOIL.lib")
#pragma comment(lib, "assimp.lib")

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat speed = 5;// angle we increase the rotation angle on mouse click
int Oldy = 0, Oldx = 256;//for mouse controls
					   //Global projection variables
GLfloat border;//range of ortho projection
GLuint ModelViewLoc, TextureLoc, BumpLoc, ProjectionLoc;
GLint bumpval;
bool LeftButtonDown = false, MiddleButtonDown = false;
mat4 projection, model_view;
GLfloat projectionAngle = 45.0;
bool ortho = false;//select the perspective matrix to start with
string  projName = "Perspective";//used in the display window title
GLfloat aspect = 1.0, pfar, pnear;//use in perspective projection
GLfloat bmax;// The maximum width of the three directions of the bounding box
vec4 eye, //eye position for perspective projections
cb;// Center of bounding box obtained from object

GLuint program;

// Global lighting initializations
vec4 light_position(0.0, 0.0, 0.0, 1.0);//infinite light source ie vector
vec3 light_ambient(0.3, .3, .3);
vec3 light_diffuse(1., 1., 1.);
vec3 light_specular(0.4, .4, .4);

// Note that loading all these models takes time
// We could use multiple threads to load each one
// There is also a paper on using CUDA to handle the loading
// Also note that the loaded models better not overfile the capacity of the GPU card

Museum museum;
bool SWITCH = true;
Camera camera;
SkyBox skybox;
Object sphere;
#define NUM_PARTICLES 1024*1024  //number of particles to move
#define WORK_GROUP_SIZE 128		//number of work items per work group
GLuint particlesVao;
GLuint posSSbo, velSSbo, colSSbo;
GLSLProgram csProgram; //compute shader program

void SetupParticles()
{
	//generate and bind vao for particles
	glGenVertexArrays(1, &particlesVao);
	glBindVertexArray(particlesVao);

	//generate and bind buffers for particles
	glGenBuffers(1, &posSSbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posSSbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(vec4), NULL, GL_STATIC_DRAW);

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT; //the invalidate makes a big difference when rewriting???
	vec4* points = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, NUM_PARTICLES * sizeof(vec4), bufMask);

	float FMIN = -1.0, FMAX = 1.0;
	srand(time(NULL));//set seed

	for (int i = 0; i < NUM_PARTICLES; ++i) {
		points[i].x = FMIN + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (FMAX - FMIN)));
		points[i].y = FMIN + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (FMAX - FMIN)));
		points[i].z = FMIN + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (FMAX - FMIN)));
		points[i].w = i;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	FMIN = 0.0, FMAX = 1.0;
	//generate and bind buffer for particles' velocities
	glGenBuffers(1, &velSSbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velSSbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(vec4), NULL, GL_STATIC_DRAW);

	vec4* vels = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, NUM_PARTICLES * sizeof(vec4), bufMask);

	srand(time(NULL));//set seed
	for (int i = 0; i < NUM_PARTICLES; ++i) {
		vels[i].x = FMIN + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (FMAX - FMIN)));
		vels[i].y = FMIN + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (FMAX - FMIN)));
		vels[i].z = FMIN + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (FMAX - FMIN)));
		vels[i].w = 1;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//generate and bind buffer for particles' color
	glGenBuffers(1, &colSSbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, colSSbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(vec4), NULL, GL_STATIC_DRAW);

	vec4* cols = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, NUM_PARTICLES * sizeof(vec4), bufMask);

	srand(time(NULL));//set seed
	for (int i = 0; i < NUM_PARTICLES; ++i) {
		cols[i].x = FMIN + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (FMAX - FMIN)));
		cols[i].y = FMIN + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (FMAX - FMIN)));
		cols[i].z = FMIN + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (FMAX - FMIN)));
		cols[i].w = 1;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//Initialize compute shader program
	csProgram.CompileShader("cshader.glsl", GL_COMPUTE_SHADER);

	glBindVertexArray(0);
}

//OpenGL initialization
void
init()
{
	skybox.Init("hw_lagoon");

	// Load shaders and use the resulting shader program
	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	museum.Load(program);
	//sphere.LoadFile("sphere.obj");
	//sphere.Load(program);
	
	bmax = museum.models[6].MaxWidth();
	cb = museum.models[6].BoxCenter();
	//bmax = sphere.MaxWidth();
	//cb = sphere.BoxCenter();

	camera = Camera(vec3(0.0, 2, cb.z+bmax/2), vec3(0.0, 2, cb.z-bmax));
	//camera = Camera(vec3(0.0, 2, cb.z + bmax * 2), vec3(cb.x,cb.y,cb.z));
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

	// store the locations for the modelview and projection matrices.  This won't change
	ModelViewLoc = glGetUniformLocation(program, "ModelView");
	ProjectionLoc = glGetUniformLocation(program, "Projection");
	
	//SetupParticles();

	glEnable(GL_DEPTH_TEST);// Turn this off and see what happens. 

	//Face culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	glClearColor(1.0, 1.0, 1.0, 1.0);// Set clear color to white
}

/*-------------------------------------------------------------
Frustum Culling using AABB intersection method
-------------------------------------------------------------*/
bool frustumCull(const mat4& PMV, const vector<vec3>& boundBox)
{
	vec4 frustumPt;
	vec3 minBox(FLT_MAX), maxBox(-FLT_MAX), maxIntersect, minIntersect;

	//create new vector with bounding box end points in frustum coordinates
	for (const vec3 pt : boundBox)
	{
		frustumPt = PMV*pt;

		frustumPt /= abs(frustumPt.w);

		minBox.x = min(minBox.x, frustumPt.x);
		minBox.y = min(minBox.y, frustumPt.y);
		minBox.z = min(minBox.z, frustumPt.z);

		maxBox.x = max(maxBox.x, frustumPt.x);
		maxBox.y = max(maxBox.y, frustumPt.y);
		maxBox.z = max(maxBox.z, frustumPt.z);
	}
	minIntersect.x = max(GLfloat(-1), minBox.x);
	minIntersect.y = max(GLfloat(-1), minBox.y);
	minIntersect.z = max(GLfloat(-1), minBox.z);

	maxIntersect.x = min(GLfloat(1), maxBox.x);
	maxIntersect.y = min(GLfloat(1), maxBox.y);
	maxIntersect.z = min(GLfloat(1), maxBox.z);

	if (maxIntersect.x < minIntersect.x || maxIntersect.y < minIntersect.y || maxIntersect.z < minIntersect.z) {
		return false;
	}

	return true;
}

void displayParticles()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, posSSbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, velSSbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, colSSbo);

	//Invoke compute shader
	csProgram.Use();
	glDispatchCompute(NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	//render updated values
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "particle"), true);

	glBindVertexArray(particlesVao);
	glBindBuffer(GL_ARRAY_BUFFER, posSSbo);
	glVertexPointer(4, GL_FLOAT, 0, (void*)0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUniform1i(glGetUniformLocation(program, "particle"), false);
}

//----------------------------------------------------------------------------
long long frame = 0, curTime, timebase, fps;
void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera.Step();
	model_view = LookAt(camera.position(), camera.viewPosition(), camera.Up());
	glUseProgram(program);
	glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view);
	skybox.Draw();

	//glUseProgram(program);
	museum.Draw(GL_TRIANGLES, ModelViewLoc, &frustumCull);
	//sphere.Draw(GL_TRIANGLES);

	//displayParticles();

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
		// reset the position to 0,0,0
	case 'r':
		Theta[0] = Theta[1] = Theta[2] = 0.0;
		break;

		// change the eye position along the z axis
	case 'Z':
		camera.move(Camera::FORWARD);
		break;
	case 'z':
		camera.move(Camera::BACKWARD);
		break;

		//Toggle back and forth from wireframe to shading
	case 'w':
		if (wirestate)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		wirestate = (wirestate) ? false : true;
		break;

	case 'b'://toggle bump mapping
		glGetUniformiv(program, BumpLoc, &bumpval);
		bumpval = (bumpval + 1) % 2;
		glUniform1i(BumpLoc, bumpval);
		break;
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'p':
		camera.move(Camera::UP);
		break;
	case 'P':
		camera.move(Camera::DOWN);
		break;
	}
}

//----------------------------------------------------------------------------
void
mouse(int button, int state, int x, int y)
{
	camera.updateMouse(x, y);
	Oldx = x; Oldy = y;
	switch (button) {
	case GLUT_LEFT_BUTTON:   LeftButtonDown = (state == GLUT_DOWN) ? TRUE : FALSE; Oldy = y;   break;
	case GLUT_MIDDLE_BUTTON:  MiddleButtonDown = (state == GLUT_DOWN) ? TRUE : FALSE; Oldy = y;  break;
	case GLUT_RIGHT_BUTTON:   Axis = Zaxis;   break;
	}
}
void MouseMotion(int x, int y)
{
	// If button1 pressed
	if (LeftButtonDown)
	{	
		Theta[0] += (y - Oldy)*.01;		
		Theta[1] -= (Oldx - x)*.01;

		camera.Pitch((y - Oldy) / 1000.0);
		camera.Yaw((x - Oldx) / 1000.0);
		//camera.mouseRotate(x, y);
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
	//	Theta[Axis] += speed;
	if (Theta[Axis] > 360.0) {
		Theta[Axis] -= 360.0;
	}
	glutPostRedisplay();
}

//this call back is called on initialization of the window and every time it is resized etc
void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	aspect = GLfloat(width) / height;
	/*if (ortho) {
	border = models[selectedModel].GetBBmax();
	if (aspect>1) {
	projection = Ortho(-border*aspect, border*aspect, -border, border, -border, border);
	}
	else {
	projection = Ortho(-border, border, -border*aspect, border*aspect, -border, border);
	}
	}
	else {*/

	projection = Perspective(projectionAngle, aspect, pnear, pfar);
	glUniformMatrix4fv(ProjectionLoc, 1, GL_TRUE, projection);
}
//----------------------------------------------------------------------------

void specialKeyFunction(int key, int x, int y)
{
	SWITCH = true;
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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 1);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Obj loader");

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
