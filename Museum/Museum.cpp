#include "Museum.h"
#include<fstream>

using namespace std;
extern mat4 model_view;
extern mat4 projection;
extern bool SWITCH;

Museum::Museum()
{
	
}

//Load frame textures on gpu
void Museum::LoadFrameTextures(vector<string>& frameImages)
{
	cout << "Loading Museum Frame Textures..." << endl;

	for (const string& imgName : frameImages) {
		frameTextures.push_back(Texture((".\\textures\\frameImages\\"+imgName).c_str(), GL_TEXTURE4));
	}
}

void Museum::Load(GLuint program)
{
	glUseProgram(program);

	//vector of object files
	vector<string> objNames = { "wall.obj", "door.obj", "lamp_holder.obj",  "frame.obj", "woodBench.obj", "base1.obj",
		"stairs.obj", "wood_step.obj", "amenemhat.obj", "cube.obj" };

	vector<string> LightSourceNames = { "sphere_bulb1.obj", "sphere_bulb2.obj", "sphere_bulb3.obj", "sphere_bulb4.obj",
		"cone_bulb1.obj", "cone_bulb2.obj", "cone_bulb3.obj", "cone_bulb4.obj"
	};

	vector<string> frameImages = { "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg",
		"7.jpg", "8.jpg", "9.jpg", "10.jpg" };

	//add models to vector of Objects
	for (string &str : objNames) {
		models.push_back(Object(str.c_str()));
	}

	//add light data
	for (string &str : LightSourceNames) {
		Lamps.push_back(Object(str.c_str()));
		LightPosition.push_back(Lamps.back().BoxCenter());

		LightAmbient.push_back(vec3(0.5, 0.5, 0.5));
		LightDiffuse.push_back(vec3(0.5, 0.5, 0.5));
		LightSpecular.push_back(vec3(0.4, .4, .4));
	}

	//Load frame textures
	LoadFrameTextures(frameImages);

	//read modular data
	readModularData("modular.dat");

	//load all models
	this->program = program;
	for (Object& model : models) {
		model.Load(program);
	}

	for (Object& lamp : Lamps) {
		lamp.Load(program);
	}

	glUniform3fv(glGetUniformLocation(program, "LightAmbient"), LightAmbient.size(), LightAmbient[0]);
	glUniform3fv(glGetUniformLocation(program, "LightDiffuse"), LightDiffuse.size(), LightDiffuse[0]);
	glUniform3fv(glGetUniformLocation(program, "LightSpecular"), LightSpecular.size(), LightSpecular[0]);
	glUniform1i(glGetUniformLocation(program, "NumLights"), 8);
}

void Museum::Draw(GLenum mode, GLuint &ModelViewLoc, bool(*frustumCull)(const mat4&, const vector<vec3>&))
{
	glUseProgram(program);

	//Apply model view to light positions also
	vector<vec4> MVLightPosition;
	for (vec4& pt : LightPosition) {
		MVLightPosition.push_back(model_view*pt);
	}
	glUniform4fv(glGetUniformLocation(program, "LightP"), LightPosition.size(), MVLightPosition[0]);

	float frust_Cull_arr[6][4];

	mat4 PMV = projection*model_view;

	//move model view matrix to shader
	glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view);

	//draw models
	for (int i = BASE; i < models.size(); i++) {
		if (frustumCull(PMV, models[i].boundBox)) {
			models[i].Draw(mode);
		}
	}

	//draw lamps
	for (Object& lamp : Lamps) {
		lamp.Draw(mode);
	}
	
	mat4 PMVT, TRANSFORM;
	int frameIdx = 0;
	//go through modular data and draw them
	for (int i = 0; i < NumModulars; i++) {		
		for (auto modData : modularData[i]) {

			if (i == frameDataIdx && frameIdx < 10) {
				//set texturing vars for frames
				models[i].meshes[0].textureAvailable = true;
				models[i].meshes[0].texture = move(frameTextures[frameIdx]);
				++frameIdx;
			}
			//calculate transform matrix
			TRANSFORM = Translate(modData.first)*RotateX(modData.second.x)*RotateY(modData.second.y)*
				RotateZ(modData.second.z);
			//multiply transform with modelview and projection matrices
			PMVT = PMV * TRANSFORM;
			//send modelview*transform matrix to shader
			glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view*TRANSFORM);
			//Test for culling and draw model
			if (frustumCull(PMVT, models[i].boundBox)) {
				models[i].Draw(mode);
			}
		}
	}
}

//Read modular data for walls
void Museum::readModularData(const char* filename)
{
	ifstream infile(filename);
	if (!infile) {
		cerr << "\nCould not load modular data from file: " << filename << endl;
		exit(1);
	}

	infile >> NumModulars;
	modularData.resize(NumModulars);

	cout << "\nLoading modular data from file: " << filename << endl;
	string modName, meshName;
	pair<vec3, vec3> modData;

	char c;
	for (int i = 0; i < NumModulars; i++) {
		infile >> modName;
		if (modName == "frame")
			frameDataIdx = i;
		cout << modName << endl;

		c = ' ';
		while (c != '\n' && infile >> meshName)
		{
			//c = infile.peek();
			if (meshName == "") {
				break;
			}
			//first is translation data
			infile >> modData.first.x >> modData.first.y >> modData.first.z;
			//second is rotation data
			infile >> modData.second.x >> modData.second.y >> modData.second.z;

			modularData[i].push_back(modData);
			infile.ignore();
			c = infile.peek();
		}
	}
}


Museum::~Museum()
{
}
