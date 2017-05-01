#include "Museum.h"

using namespace std;
extern mat4 model_view;
extern mat4 projection;

Museum::Museum()
{
	time = 0.0;
	rotAngle = 10.0;
	DGemAtt = 0.000075;
	GemAtt = 0.005;
}

/*************************************************************
**Function to Load textures for frames in the museum
**Requires a vector of strings containing the file names for
	the images to be used as textures
*************************************************************/
void Museum::LoadFrameTextures(vector<string>& frameImages)
{
	cout << "Loading Museum Frame Textures..." << endl;

	for (const string& imgName : frameImages) {
		frameTextures.push_back(Texture((".\\textures\\frameImages\\"+imgName).c_str(), GL_TEXTURE4));
	}
}

/*************************************************************
**Function to Load objects an textures in the museum
**Requires the shader program to draw this museum
*************************************************************/
void Museum::Load(GLuint program)
{
	glUseProgram(program);

	//vector of names for models
	/*Modularized data are the first set of models. (This is important to the implementation of the museum).
	  The order of the file names must be the same as the order of the enums in the header file.
	*/
	vector<string> objNames = { "wall.obj", "door.obj", "lamp_holder.obj", "woodBench.obj", "Frame-1.obj", 
		"base.obj", "30-ts-serpent.obj", "Xiuhcoatl.obj", "SasukeSusanoo.obj", "YinYangSusanoo.obj", "Mask_Statue.obj",
		"stairs.obj", "cstairs.obj", "amenemhat.obj", "cube.obj" };

	//object file names for lamps
	vector<string> LightSourceNames = { "sphere_bulb1.obj", "sphere_bulb2.obj", "sphere_bulb3.obj", "sphere_bulb4.obj",
		"cone_bulb1.obj", "cone_bulb2.obj", "cone_bulb3.obj", "cone_bulb4.obj"
	};

	//image names for images on frames
	vector<string> frameImages = { "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg",
		"7.jpg", "8.jpg", "9.jpg", "10.jpg" };

	//Gems around yin yang susanoo
	vector<string> gemNames = { "Gem.obj", "Gem2.obj", "Gem3.obj", "Gem4.obj" };

	//add models of gems
	for (string& str : gemNames) {
		Gems.push_back(str.c_str());
	}

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

	//set all lights to be initially active
	LightStatus.resize(LightSourceNames.size(), true); 

	//Load frame textures
	LoadFrameTextures(frameImages);

	//read modular data
	readModularData("modular.dat");

	//Load Gems
	for (Object& gem : Gems) {
		gem.Load(program);
	}

	//load Museum models
	this->program = program;
	for (Object& model : models) {
		model.Load(program);
	}

	//Load lamps
	for (Object& lamp : Lamps) {
		lamp.Load(program);
	}

}

/**************************************************************************
**Draws the museum and all models in it
**Requires the draw mode, the modelview location in the shader, a function
	to determine if objects are to be drawn based on their bounding box location
**************************************************************************/
void Museum::Draw(GLenum mode, GLuint &ModelViewLoc, bool(*frustumCull)(const mat4&, const vector<vec3>&))
{
	//Use program
	glUseProgram(program);

	vector<vec3> LAmb, LDif, LSpec;
	vector<vec4> MVLightPosition;

	//Get active light materials and position
	for (int i = 0; i < LightStatus.size(); ++i){
		if (LightStatus[i]) {
			LAmb.push_back(LightAmbient[i]);
			LDif.push_back(LightDiffuse[i]);
			LSpec.push_back(LightSpecular[i]);
			MVLightPosition.push_back(model_view*LightPosition[i]); //transform light position as well
		}
	}

	//Send active lights info to shader
	glUniform1i(glGetUniformLocation(program, "NumLights"), MVLightPosition.size());
	if (MVLightPosition.size() > 0) {
		glUniform3fv(glGetUniformLocation(program, "LightAmbient"), LAmb.size(), LAmb[0]);
		glUniform3fv(glGetUniformLocation(program, "LightDiffuse"), LDif.size(), LDif[0]);
		glUniform3fv(glGetUniformLocation(program, "LightSpecular"), LSpec.size(), LSpec[0]);
		glUniform4fv(glGetUniformLocation(program, "LightP"), MVLightPosition.size(), MVLightPosition[0]);
	}

	//Get projection * model view matrix
	mat4 PMV = projection*model_view;

	//send model view matrix to shader
	glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view);

	//Draw gems with glow effect
	for (Object& gem : Gems) {
		glUniform1i(glGetUniformLocation(program, "GemUse"), true);
		glUniform1f(glGetUniformLocation(program, "GemAtt"), GemAtt);
		gem.Draw(mode);
		glUniform1i(glGetUniformLocation(program, "GemUse"), false);
	}

	//Draw models in museum
	for (int i = BASE; i < models.size(); i++) {
		if (i == SERPENTJET) {
			//Rotate SERPENTJET jet around museum
			mat4 RY = Translate(models[BASE].centerBox.x, 0, models[BASE].centerBox.z)*RotateY(rotAngle);
			mat4 PMVR = PMV*RY;
			if (frustumCull(PMVR, models[i].boundBox)) {
				glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view*RY);
				models[i].Draw(mode);
				glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view);
			}
		}
		else if (i == YSUSANOO) {
			//Rotate Ying Yang Susanoo about its axis
			mat4 TRT = Translate(models[YSUSANOO].centerBox)*RotateY(-rotAngle)*Translate(-models[YSUSANOO].centerBox);
			mat4 PMVTRT = PMV*TRT;
			if (frustumCull(PMVTRT, models[i].boundBox)) {
				glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view*TRT);
				models[i].Draw(mode);
				glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view);
			}
		}
		else {
			//Apply frustum cull and draw models
			if (frustumCull(PMV, models[i].boundBox)) {
				models[i].Draw(mode);
			}
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

			if (i == FRAME && frameIdx < 10) {
				//set texturing data for frames
				models[i].meshes[1].textureAvailable = true;
				models[i].meshes[1].texture = move(frameTextures[frameIdx]);
				++frameIdx;
			}
			//calculate transform matrix for modular data
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

	//Update angle and Attenuation values for gems
	GemAtt += DGemAtt;
	if (GemAtt > 0.00500000 || GemAtt < 0.00000000) {
		DGemAtt *= -1;
	}
	rotAngle += 1.0;
	if (rotAngle > 360.00) {
		rotAngle = 1.0;
	}
}

/*************************************************************
**Function to process keyboard input from main program
**Currently enables and disables light sources in the museum
*************************************************************/
void Museum::keyboardFunction(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
		int idx = key-'0';
		LightStatus[idx-1] = !LightStatus[idx-1];
		break;

	}
}

/*************************************************************
**Function to Read modular data for modularized models from a file
**Requires the file containing location and rotation data for 
	duplicate models
*************************************************************/
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
	int a = 0;
}


Museum::~Museum()
{
}
