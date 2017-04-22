
#ifndef OBJECT_H
#define OBJECT_H

#pragma once
#include "Angel.h"
#include "assimp/Importer.hpp"
#include "assimp/PostProcess.h"
#include "assimp/Scene.h"
#include "Mesh.h"

class Object
{
private:
	GLuint program;
	vector<Mesh> meshes;
	float BoxMax;
	vec4 centerBox;
	friend class Museum;

public:
	vector<vec3> boundBox;
	Object(const char* filename);
	Object();
	~Object(void);
	bool LoadFile(const char* filename);
	bool Load(GLuint & program);
	bool Draw(GLenum mode);
	vec4 BoxCenter();
	float MaxWidth();
	vec3 minBoxpt();
	vec3 maxBoxpt();
};

#endif // !OBJECT_H

