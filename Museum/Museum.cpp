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
**Function to Load objects and textures in the museum
**Requires the shader program to draw this museum
*************************************************************/
void Museum::Load(GLuint program)
{
	this->program = program;
	glUseProgram(program);

	//Create objects for models
	/*Modularized data are the first set of models. (This is important to the implementation of the museum).
	  The order of the file names must be the same as the order of the enums in the header file.
	*/
	models = { "wall.obj", "door.obj", "lamp_holder.obj", "woodBench.obj", "Frame-1.obj",
		"sphere_bulb.obj", "cone_bulb.obj", "base.obj", "30-ts-serpent.obj", "Xiuhcoatl.obj",
		"SasukeSusanoo.obj", "YinYangSusanoo.obj", "Mask_Statue.obj", "stairs.obj", "cstairs.obj",
		"amenemhat.obj" };

	//Create frame textures
	string dir = ".\\textures\\frameImages\\";
	//I'm using different texture units just to verify that the texture object works well for all units
	frameTextures = { {(dir+"mendelev.jpg").c_str(), GL_TEXTURE1}, {(dir+"lastSupper.jpg").c_str(), GL_TEXTURE2},
		{(dir+"mozart.jpg").c_str(), GL_TEXTURE3}, {(dir+"norton.jpg").c_str(), GL_TEXTURE4 }, 
		{ (dir+"davinci.jpg").c_str(), GL_TEXTURE5 }, { (dir+"ouroboros.jpg").c_str(), GL_TEXTURE6 },
		{(dir+"pinterest.jpg").c_str(), GL_TEXTURE7}, {(dir+"benleader.jpg").c_str(), GL_TEXTURE8},
		{(dir+"2monks.jpg").c_str(), GL_TEXTURE9}, {(dir+"yamamoto.png").c_str(), GL_TEXTURE10},
		{(dir+"sasuke.jpg").c_str(), GL_TEXTURE11}, {(dir+"archer.jpg").c_str(), GL_TEXTURE12},
		{(dir+"hand.jpg").c_str(), GL_TEXTURE13}, {(dir+"hitler.jpg").c_str(), GL_TEXTURE14}
	};

	//Gems around yin yang susanoo
	Gems = { "Gem.obj", "Gem2.obj", "Gem3.obj", "Gem4.obj" };

	//Load Gems
	for (Object& gem : Gems) {
		gem.LoadGPU(program);
	}

	//load Museum models
	for (Object& model : models) {
		model.LoadGPU(program);
	}

	//read modular data: THIS MUST BE DONE AFTER THE MODELS HAVE BEEN LOADED
	ReadModularData("modular.dat");

	//The modular data must be read first so that the LightPosition is populated
		//before this can work
	LightStatus.resize(LightPosition.size(), true); //set all lights to be initially active

	//Set point light properties: THIS CAN BE DONE INDIVIDUALLY (OR SET FROM THE MODULAR DATA FILE)
	//TO GIVE EACH LIGHT A DIFFERENT PROPERTY.
	for (int i = 0; i < LightPosition.size(); ++i) {
		LightAmbient.push_back(vec3(0.5, 0.5, 0.5));
		LightDiffuse.push_back(vec3(0.5, 0.5, 0.5));
		LightSpecular.push_back(vec3(0.4, .4, .4));
	}
}

/**************************************************************************
**Draws the museum and all models in it
**Requires the draw mode, the modelview location in the shader, a function
	to determine if objects are to be drawn based on their bounding box location
**************************************************************************/
void Museum::Draw(GLenum mode, GLuint &ModelViewLoc, bool(*FrustumIntersect)(const mat4&, const vector<vec3>&))
{
	//Use program
	glUseProgram(program);

	vector<vec3> LAmb, LDif, LSpec;
	vector<vec4> MVLightPosition;

	//Get enabled lights' materials and position
	for (int i = 0; i < LightStatus.size(); ++i){
		if (LightStatus[i]) {
			LAmb.push_back(LightAmbient[i]);
			LDif.push_back(LightDiffuse[i]);
			LSpec.push_back(LightSpecular[i]);
			MVLightPosition.push_back(model_view*LightPosition[i]); //transform light position
		}
	}

	//Send enabled lights info to shader
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
			if (FrustumIntersect(PMVR, models[i].boundBox)) {
				glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view*RY);
				models[i].Draw(mode);
				glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view);
			}
		}
		else if (i == YSUSANOO) {
			//Rotate Ying Yang Susanoo about its axis
			mat4 TRT = Translate(models[YSUSANOO].centerBox)*RotateY(-rotAngle)*Translate(-models[YSUSANOO].centerBox);
			mat4 PMVTRT = PMV*TRT;
			if (FrustumIntersect(PMVTRT, models[i].boundBox)) {
				glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view*TRT);
				models[i].Draw(mode);
				glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view);
			}
		}
		else {
			//Apply frustum cull and draw models
			if (FrustumIntersect(PMV, models[i].boundBox)) {
				models[i].Draw(mode);
			}
		}
	}
	
	mat4 PMVT, TRANSFORM;
	int frameIdx = 0;
	//go through modular data and draw them
	for (int i = 0; i < NumModulars; i++) {		
		for (auto modData : modularData[i]) {

			if (i == FRAME) {
				//set texturing data for frames
				models[i].meshes[1].textureAvailable = true;
				models[i].meshes[1].texture = move(frameTextures[frameIdx]);
				++frameIdx;
			}

			//calculate transform matrix for modular data
			TRANSFORM = Translate(get<0>(modData))*RotateX(get<1>(modData).x)*RotateY(get<1>(modData).y)*
				RotateZ(get<1>(modData).z)*Scale(get<2>(modData));

			//multiply transform with modelview and projection matrices
			PMVT = PMV * TRANSFORM;
			//send modelview*transform matrix to shader
			glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, model_view*TRANSFORM);
			//Test for culling and draw model
			if (FrustumIntersect(PMVT, models[i].boundBox)) {
				models[i].Draw(mode);
			}
		}
	}

	//Update angle for jet and Attenuation values for gems
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
void Museum::ReadModularData(const char* filename)
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
	vec3 loc, rot, scale;

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
			infile >> loc.x >> loc.y >> loc.z;
			//second is rotation data
			infile >> rot.x >> rot.y >> rot.z;
			//third is scale data
			infile >> scale.x >> scale.y >> scale.z;

			modularData[i].push_back(make_tuple(loc, rot, scale));

			if (i == SPHERE_BULB || i == CONE_BULB) {
				//The centers of the light bulbs are the position of the point light sources
				//THIS MUST BE DONE AFTER THE BULB MODEL(S) HAVE BEEN LOADED
				LightPosition.push_back(Translate(loc)*RotateX(rot.x)*RotateY(rot.y)*
					RotateZ(rot.z)*Scale(scale)*models[i].centerBox);
			}
			infile.ignore();
			c = infile.peek();
		}
	}
}


Museum::~Museum()
{
}
