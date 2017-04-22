#pragma once
#include<Angel.h>
#include<soil.h>
#include<vector>
#include<string>
using namespace std;

#define bsize 50.0

class SkyBox
{
private:
	GLuint program;
	GLuint vao;
	GLuint textureID;
	GLuint buffer;
	GLuint modelViewLoc, projectionLoc;

	float vertices[36*3] = {
		-bsize, bsize, -bsize,
		-bsize, -bsize, -bsize,
		bsize, -bsize, -bsize,
		bsize, -bsize, -bsize,
		bsize, bsize, -bsize,
		-bsize, bsize, -bsize,
		-bsize, -bsize, bsize,
		-bsize, -bsize, -bsize,
		-bsize, bsize, -bsize,
		-bsize, bsize, -bsize,
		-bsize, bsize, bsize,
		-bsize, -bsize, bsize,
		bsize, -bsize, -bsize,
		bsize, -bsize, bsize,
		bsize, bsize, bsize,
		bsize, bsize, bsize,
		bsize, bsize, -bsize,
		bsize, -bsize, -bsize,
		-bsize, -bsize, bsize,
		-bsize, bsize, bsize,
		bsize, bsize, bsize,
		bsize, bsize, bsize,
		bsize, -bsize, bsize,
		-bsize, -bsize, bsize,
		-bsize, bsize, -bsize,
		bsize, bsize, -bsize,
		bsize, bsize, bsize,
		bsize, bsize, bsize,
		-bsize, bsize, bsize,
		-bsize, bsize, -bsize,
		-bsize, -bsize, -bsize,
		-bsize, -bsize, bsize,
		bsize, -bsize, -bsize,
		bsize, -bsize, -bsize,
		-bsize, -bsize, bsize,
		bsize, -bsize, bsize
	};

public:
	SkyBox();
	void Init(const char *);
	void Draw();
	~SkyBox();
};

