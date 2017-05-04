/*GLSLProgram Class: A modified version of the functions from Angel files*/

#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H

#pragma once
#include"Angel.h"
#include<iostream>
#include<fstream>
#include<string>

using namespace std;

class GLSLProgram
{
private:
	string filename;
	GLuint program;
	char* readShaderSource(const char* shaderFile);
	GLenum shaderType;

public:
	GLSLProgram();
	~GLSLProgram();

	void CompileShader(const char* shaderFileName, GLenum shaderType);

	void Use();

};

#endif // !GLSLPROGRAM_H
