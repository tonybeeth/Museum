
#ifndef MUSEUM_H
#define MUSEUM_H

#pragma once
#include"Object.h"

class Museum
{
	enum {WALL, DOOR, LAMP_HOLDER, FRAME, BENCH, BASE, STEP, STEP2, AMENEMHAT, CUBE};

private:
	int NumModulars;
	void readModularData(const char* filename);
	GLuint program, ModelViewLoc, ProjectionLoc;
	vector<vector<pair<vec3, vec3>>> modularData;
	vector<vec3> LightAmbient, LightDiffuse, LightSpecular;
	vector<vec4> LightPosition;
	void LoadFrameTextures(vector<string>&);
	vector<Texture> frameTextures;
	int frameDataIdx;


public:
	vector<Object> models;
	vector<Object> Lamps;
	Museum();
	void Load(GLuint program);
	void Draw(GLenum mode, GLuint &ModelViewLoc, bool (*frustumCull)(const mat4&, const vector<vec3>&));
	~Museum();
};

#endif