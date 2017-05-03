//Object.h: Header file for Object class
//Objects are loaded using the Assimp library: http://www.assimp.org/
/* An object contains a vector of Meshes where the actual vertex and material properties
	are stored
*/

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
	string filename;
	vector<vec3> boundBox;
	
	//Make museum a friend class so it can access private properties if needed
	friend class Museum;

public:
	/*Default Constructor*/
	Object();

	/*Parameterized Constructor with object's filename*/
	Object(const char* filename);

	/*Destructor*/
	~Object();

	/*LoadFromFile method:
		Loads object's data from a file
	*/
	bool LoadFromFile(const char* filename);

	/*LoadGPU method:
		Loads object's data to the graphics card using the specified program
	*/
	bool LoadGPU(GLuint & program);

	/*Draw method:
		Renders this object in the given mode
	*/
	bool Draw(GLenum mode);

	/*BoxCenter method:
		Returns the center of the bounding box for this object
	*/
	vec4 BoxCenter();

	/*MaxWidth method:
		Returns the maximum width of this object
	*/
	float MaxWidth();

	/*MinBoxPtmethod:
		Returns the minimum point of this object's bounding box
	*/
	vec3 MinBoxPt();

	/*MaxBoxPt method:
		Returns the maximum point of this object's bounding box
	*/
	vec3 MaxBoxPt();
};

#endif // !OBJECT_H

