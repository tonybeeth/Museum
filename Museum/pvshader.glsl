#version 150
#define MAX_LIGHTS 10 //Max Number of light sources

in  vec4 vPosition;
in  vec4 vNormal;
in  vec2 tCorrdinate;
out vec2 Texcoord;//j sent to fragshader

uniform mat4 ModelView;
uniform mat4 Projection;

uniform vec4 LightPosition;
uniform vec4 LightP[MAX_LIGHTS];

uniform vec3 LAmbient, LDiffuse, LSpecular;
uniform vec3 LightAmbient[MAX_LIGHTS], LightDiffuse[MAX_LIGHTS], LightSpecular[MAX_LIGHTS];

uniform int NumLights; //Number of light sources we will be making use of
uniform float Shininess;

uniform vec3 MAmbient, MDiffuse, MSpecular;

out vec4 color;

void main()
{
	vec3 pos = (ModelView * vPosition).xyz;
	vec3 L;
	vec3 E=pos;
    vec3 N = (ModelView*vNormal).xyz; //transform the normal as well
	float Att;

	E= normalize(E);
	N= normalize(N);
	color = vec4(0.0,0.0,0.0,0.0);
	vec3 ambient, diffuse, specular, H;
	float Kd, Ks;

	for(int i = 0; i < NumLights; i++)
	{
		L = LightP[i].xyz-pos;
		L= normalize(L);

		// if lightposition is sent in as a vector the
		// the following is needed.  ie Infinite light source
		//if(LightPosition.w==0.0)L=LightPosition.xyz;

		H = normalize(L+E);
		Kd = max(dot(L,N), 0.0);
		Ks = pow(max(dot(N, H), 0.0), Shininess);
		ambient = LightAmbient[i] * MAmbient;
		diffuse = Kd * LightDiffuse[i] * MDiffuse;

		if(dot(L,N) < 0.0) {
			specular = vec3(0.0,0.0,0.0);
		}
		else{
			specular = Ks * LightSpecular[i]*MSpecular;
		}

		Att = 1.0/(1 + 3.0*pow(length(L), 2.0)); //Get Attenuation factor
		color += vec4(ambient+diffuse+specular,1.0)*Att;
	}

	Texcoord=tCorrdinate;
	gl_Position=Projection*ModelView*vPosition;
}
