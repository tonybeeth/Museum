#version 150
#define MAX_LIGHTS 10 //Max Number of light sources

in  vec4 vPosition;
in  vec4 vNormal;
in  vec2 tCorrdinate;
out vec2 Texcoord;//j sent to fragshader

uniform mat4 ModelView;
uniform mat4 Projection;

//uniform vec4 LightPosition; 
uniform vec4 LightP[MAX_LIGHTS];
uniform int NumLights; //Number of light sources we will be making use of
uniform bool GemUse;
uniform float GemAtt;

out vec3 N;
//out vec3 L;
out vec3 LV[MAX_LIGHTS];
out vec3 E;
out float ATT[MAX_LIGHTS];

void main()
{
	vec3 pos = (ModelView * vPosition).xyz;
	E=pos;
	gl_Position=Projection*ModelView*vPosition;
    N = (ModelView*vNormal).xyz; //transform the normal as well

	for(int i = 0; i < NumLights; i++)
	{
		//L = LightPosition.xyz-pos; 
		LV[i] = LightP[i].xyz-pos;

		if(GemUse){
			ATT[i] = 1.0/(1+GemAtt*pow(length(LV[i]), 2.0)); //Get Attenuation factor
		}
		else{
			ATT[i] = 1.0/(1+0.005*pow(length(LV[i]), 2.0)); //Get Attenuation factor
		}

		// if lightposition is sent in as a vector the
		// the following is needed. ie Infinite light source
		//if(LightPosition.w==0.0)L=LightPosition.xyz;
	}

	Texcoord=tCorrdinate;
}
