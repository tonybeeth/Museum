#include "SkyBox.h"

extern mat4 projection;
extern mat4 model_view;

SkyBox::SkyBox()
{
}

void SkyBox::Init(const char* cubeDir)
{
	//create shader program
	program = InitShader("sky_vshader.glsl", "sky_fshader.glsl");
	glUseProgram(program);
	//generate and bind vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//generate and bind buffer
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	//move vertex data to buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//plumbing
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//generate a texture ID
	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &textureID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	//store texture faces names
	vector<string> textureFaceNames = { "right.png", "left.png", "up.png", "down.png", "back.png", "front.png" };

	//load all faces textures
	int width, height;
	unsigned char* image;
	GLuint target;
	for (int i = 0; i < textureFaceNames.size(); i++) {
		image = SOIL_load_image((".\\textures\\" +string(cubeDir) + "\\" + textureFaceNames[i]).c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		
		glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	modelViewLoc = glGetUniformLocation(program, "ModelView");
	projectionLoc = glGetUniformLocation(program, "Projection");

	glBindVertexArray(0);
}

void SkyBox::Draw()
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// First, disable writing to the depth buffer. This means everything in the world will
	//be drawn in front of our skybox
	glDepthMask(GL_FALSE);

	glUseProgram(program); //use the skybox shaders

	//send over model view and projection matrices
	mat4 mv = model_view;

	mv[0][3] = mv[1][3] = mv[2][3] = 0;//remove translation component

	glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection);
	glUniformMatrix4fv(modelViewLoc, 1, GL_TRUE, mv);

	glBindVertexArray(vao); //bind vao
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); //bind texture ID
	glDrawArrays(GL_TRIANGLES, 0, 36); //draw 36 triangles
	glBindVertexArray(0); //unbind vao
	glDepthMask(GL_TRUE); //enable writing to depth buffer
}

SkyBox::~SkyBox()
{

}
