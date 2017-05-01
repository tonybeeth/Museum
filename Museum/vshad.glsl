#version 430

in  vec4 vPosition;
in  vec4 vNormal;
out vec4 pcolor;
in  vec2 tCorrdinate;

uniform bool particle;

out vec2 Texcoord;//j sent to fragshader

uniform mat4 ModelView;
uniform mat4 Projection;

uniform vec4 LightPosition; 

out vec3 N;
out vec3 L;
out vec3 E;

layout( std140, binding=6) buffer Col
{
	vec4 Colors[ ];
};

void main()
{
	vec4 p = vPosition;
	if(particle){
		p.w = 1;
		pcolor = vec4(0.0,0.0,0.0,0.0);
	}

	vec3 pos = (ModelView * p).xyz;
	E=pos;
	gl_Position=Projection*ModelView*p;
    N = (ModelView*vNormal).xyz; //transform the normal as well

	L = LightPosition.xyz-pos;

	//ATT[i] = 1.0/(1+0.005*pow(length(LV[i]), 2.0)); //Get Attenuation factor

	// if lightposition is sent in as a vector the
	// the following is needed. ie Infinite light source
	//if(LightPosition.w==0.0)L=LightPosition.xyz;

	Texcoord=tCorrdinate;
}
