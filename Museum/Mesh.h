//Mesh.h: Header file for Mesh class
//Mesh is loaded using the Assimp library: http://www.assimp.org/
/* A Mesh contains vertex data and material properties defining a single mesh
*/

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
	//vertices, normals, UV and index lists
	vector<vec4> vertexList, normalList; 
	vector<vec2> uvList;
	vector<GLuint> indexList;
	//buffers for vertex data and indices
	GLuint buffer, indexBuffer;
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

	//Texture object for this Mesh
	Texture texture;

	//Make museum class a friend to access private data if neccessary
	friend class Museum; 

public:
	/*Construtor to load mesh from scene using Assimp:
		This extracts all mesh data from the scene
		Needs a pointer to the scene and the mesh's index in the array of meshes
	*/
	Mesh(const aiScene* scene, unsigned int meshIndex);

	//Loads Mesh's data on the GPU
	bool LoadGPU(GLuint program);

	//Draws the mesh in the specified mode
	bool Draw(GLenum mode);
	float MaxWidth();
	vec4 BoxCenter();
	vec3 MinBoxPt();
	vec3 MaxBoxPt();
	~Mesh();
};

#endif // !MESH_H