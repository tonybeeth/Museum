#include "GLSLProgram.h"

GLSLProgram::GLSLProgram()
{
}


GLSLProgram::~GLSLProgram()
{
}

char * GLSLProgram::readShaderSource(const char * shaderFile)
{
	ifstream t(shaderFile);
	if (!t)
		return NULL;

	string str;

	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	char* buf = new char[str.size()+1];
	strcpy(buf, str.c_str());
	buf[str.size()] = '\0';
	return buf;
}

void GLSLProgram::CompileShader(const char * shaderFileName, GLenum shaderType)
{
	filename = string(shaderFileName);
	this->shaderType = shaderType;

	GLuint program = glCreateProgram();

	char* source = readShaderSource(filename.c_str());

	if (source == NULL) {
		std::cerr << "Failed to read " << filename << std::endl;
		exit(EXIT_FAILURE);
	}

	GLuint shader = glCreateShader(this->shaderType);
	glShaderSource(shader, 1, (const GLchar**)&source, NULL);
	glCompileShader(shader);

	GLint  compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		std::cerr << filename << " failed to compile:" << std::endl;
		GLint  logSize;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetShaderInfoLog(shader, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;

		exit(EXIT_FAILURE);
	}

	delete[] source;

	glAttachShader(program, shader);


	/* link  and error check */
	glLinkProgram(program);

	GLint  linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		std::cerr << filename << ": Shader program failed to link" << std::endl;
		GLint  logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;

		exit(EXIT_FAILURE);
	}

	/* use program object */
	glUseProgram(program);

	//set program
	this->program = program;
}

void GLSLProgram::Use()
{
	glUseProgram(program);
}
