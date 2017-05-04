/*Texture.h: Header file for texture Object*/

#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once

#include<Angel.h>
#include<map>
#include<string>
#include <vector>
#include"soil.h"
#include<string>
using namespace std;

class Texture
{
private:
	GLuint texId;
	//string texFileName;
	unsigned char* texImage = NULL;
	int texWidth, texHeight;
	GLuint texUnit;

public:

	//Default constructor using default texture unit 0
	Texture(GLuint texUnit = GL_TEXTURE0);

	//Parametereized constructor
	Texture(const char* textureFileName, GLuint texUnit = GL_TEXTURE0, int force_channels = SOIL_LOAD_RGB);

	//Init method to be called for allocating tex object and loading texture file
	bool Init(const char* str, GLuint texUnit = GL_TEXTURE0, int force_channels = SOIL_LOAD_RGB);

	//Function to set texture for use in shaders before drawing
	void Use(GLuint program, const char *shaderSampler);

	~Texture();

};


#endif // !TEXTURE_H