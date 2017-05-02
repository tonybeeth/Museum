#version 420
#define MAX_LIGHTS 10 //Max Number of light sources

// This is a Phong fragment shader 
in  vec4 vPosition;
in  vec4 vNormal;
in  vec2 Texcoord;
in vec3 N;
in vec3 L; 
in vec3 E;

out  vec4 fColor;
uniform sampler2D tex;
uniform bool textureused;
uniform mat4 ModelView;

//Directional Light properties
uniform vec3 LAmbient, LDiffuse, LSpecular; 
uniform vec4 LightPosition;

//Point lights properties
uniform vec3 LightAmbient[MAX_LIGHTS], LightDiffuse[MAX_LIGHTS], LightSpecular[MAX_LIGHTS];
in vec3 LV[MAX_LIGHTS];
in float ATT[MAX_LIGHTS];
uniform int NumLights;

//Material properties
uniform vec3 MAmbient, MDiffuse, MSpecular;
uniform float Shininess;

uniform bool UseDirectional;

vec4 CalculateColor( vec3 L, vec3 EE, vec3 NN, vec3 LAmb, vec3 LDif, 
	 vec3 LSpec, vec3 MAmb, vec3 MDif, vec3 MSpec, float MShininess,
	float Att)
{
	vec3 LL = normalize(L); 
	vec3 H = normalize( LL+EE);
	float Kd=max(dot(LL,NN),0.0);
	float Ks = pow( max(dot(NN, H), 0.0), MShininess );
	vec3 ambient = LAmb*MAmb;
	vec3 diffuse = Kd*LDif*MDif;
	vec3 specular;
	if( dot(LL, NN) < 0.0 ) {
    	specular = vec3(0.0, 0.0, 0.0);
	} else	specular = Ks * LSpec*MSpec;

	return vec4(ambient + diffuse + specular,1.0)*Att;
}

void main()
{
	vec3 NN=normalize(N);
	vec3 EE=normalize(E);
	vec4 color = vec4(0.0,0.0,0.0,0.0);

	if(UseDirectional){
		color = CalculateColor(L, EE, NN, LAmbient, LDiffuse, LSpecular, MAmbient, 
			MDiffuse, MSpecular, Shininess, 1);
	}
	else{
		for(int i = 0; i < NumLights; i++)
		{
			color += CalculateColor(LV[i], EE, NN, LightAmbient[i], LightDiffuse[i], LightSpecular[i], 
				MAmbient, MDiffuse, MSpecular, Shininess, ATT[i]);	   
		}
	}

	if(textureused)
		fColor =(color*texture(tex, Texcoord));
	else
		fColor = color;// there is no texture so just use the color
}
