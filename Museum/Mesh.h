
#ifndef MESH_H
#define MESH_H

#pragma once
#include "Angel.h"
#include "assimp/Scene.h"
#include<map>
#include<string>
#include <vector>
#include"soil.h"
#include"Texture.h"

using namespace std;

class Mesh
{
private:
	GLuint vao; //vao for this mesh
	GLuint program;
	//vertices, normals and UVs list
	vector<vec4> vertexList, normalList; 
	vector<vec2> uvList;
	//buffer for vertex attributes and id for texture
	GLuint buffer;
	string textureFileName;
	bool textureAvailable;
	string name; //mesh's name, if available
	//lighting properties
	vec3 ambient, specular, diffuse, emission;
	float shininess;
	//bounding box
	vector<vec3> boundBox;

	//Maximum width of bounding box
	GLfloat BoxMax; 
	vec4 centerBox; //center of bounding box
	Texture texture;
	friend class Museum;

public:
	//contrutor to load mesh from scene imported using Assimp
	Mesh(const aiScene* scene, unsigned int meshIndex);
	bool Load(GLuint program);
	bool Draw(GLenum mode);
	float MaxWidth();
	vec4 BoxCenter();
	vec3 minBoxpt();
	vec3 maxBoxpt();
	string textureName();
	bool hasTexture();
	~Mesh();
};

#endif // !MESH_H