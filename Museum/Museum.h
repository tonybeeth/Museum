/*Museum.h: Header file for museum class
	This Museum is made up of a base (floor), walls, statues, lights, frames, benches, and a Jet.
	Modular construction is used in constructing some of the models contained in the museum:
		(walls, lamps and holders, benches, image frames). 
	A single model of the modular object is loaded, and the location, rotation and scale data for 
	all the duplicates of the same model is read in from a file and used to draw them.

	Only the walls and the floor are models designed by Anthony Enem. Other models gotten for free. 
	All textures are downloaded from the internet.
*/

#ifndef MUSEUM_H
#define MUSEUM_H

#pragma once
#include"Object.h"
#include<fstream>
#include<tuple>

class Museum
{
	//Enums defining indices of museum models in the models vector
	//Models that are modularized must be the first set of models in the set
	enum {WALL, DOOR, LAMP_HOLDER, BENCH, FRAME, SPHERE_BULB, CONE_BULB, BASE, SERPENTJET, XIUHCOATL, SSUSANOO, 
		YSUSANOO, MASKSTATUE, STEP, STEP2, AMENEMHAT};

private:
	//Number of modularized models(gotten from file with modular data)
	int NumModulars; 

	//Method to read modular data from file
	void ReadModularData(const char* filename); 

	//Program object, location of modelview and projection matrices in shader
	GLuint program, ModelViewLoc, ProjectionLoc;

	//vector containing data of modularized ojects
	vector<vector<tuple<vec3, vec3, vec3>>> modularData;

	//vectors of Light properties
	vector<vec3> LightAmbient, LightDiffuse, LightSpecular;
	vector<vec4> LightPosition;
	vector<bool> LightStatus; //true for active lights, false otherwise

	//vector of textures on frames
	vector<Texture> frameTextures;

	GLfloat time, rotAngle;
	GLfloat GemAtt, DGemAtt;
	vector<Object> Gems;

public:
	Museum();
	vector<Object> models;
	void Load(GLuint program);
	void Draw(GLenum mode, GLuint &ModelViewLoc, bool (*frustumCull)(const mat4&, const vector<vec3>&));
	void keyboardFunction(unsigned char key, int x, int y);
	~Museum();
};

#endif