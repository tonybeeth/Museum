
#ifndef MUSEUM_H
#define MUSEUM_H

#pragma once
#include"Object.h"
#include<fstream>

class Museum
{
	//Enums defining indices of museum models in the models vector
	//Models that are modularized must be the first set of models in the set
	enum {WALL, DOOR, LAMP_HOLDER, BENCH, FRAME, BASE, SERPENTJET, XIUHCOATL, SSUSANOO, 
		YSUSANOO, MASKSTATUE, STEP, STEP2, AMENEMHAT, CUBE};

private:
	//Number of modularized models(gotten from file with modular data)
	int NumModulars; 

	//Method to read modular data from file
	void readModularData(const char* filename); 

	//Program object, location of modelview and projection matrices in shader
	GLuint program, ModelViewLoc, ProjectionLoc;

	//vector containing data of modularized ojects
	vector<vector<pair<vec3, vec3>>> modularData;

	//vector of Light properties
	vector<vec3> LightAmbient, LightDiffuse, LightSpecular;
	vector<vec4> LightPosition;
	vector<bool> LightStatus; //true for active, false otherwise

	//Method to load textures for frames
	void LoadFrameTextures(vector<string>&);

	//vector of textures on frames
	vector<Texture> frameTextures;

	GLfloat time, rotAngle;
	GLfloat GemAtt, DGemAtt;

	vector<Object> Lamps;
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