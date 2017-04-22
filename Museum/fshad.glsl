#version 150

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

uniform vec3 LAmbient, LDiffuse, LSpecular; 

uniform vec3 MAmbient, MDiffuse, MSpecular;
uniform mat4 ModelView;
uniform vec4 LightPosition;
uniform float Shininess;

void main()
{
	vec3 NN=normalize(N);
	vec3 EE=normalize(E);
	vec4 color = vec4(0.0,0.0,0.0,0.0);

	vec3 LL = normalize(L); //normalize(L);
	vec3 ambient,diffuse,specular;
	vec3 H = normalize( LL+EE);
	float Kd=max(dot(LL,NN),0.0);
	float Ks = pow( max(dot(NN, H), 0.0), Shininess );
	ambient = LAmbient * MAmbient; //LAmbient*MAmbient;
	diffuse = Kd*LDiffuse*MDiffuse;
		
	if( dot(LL, NN) < 0.0 ) {
    	specular = vec3(0.0, 0.0, 0.0);
	} else	specular = Ks * LSpecular*MSpecular;

	color += vec4(ambient + diffuse + specular,1.0);
	  

	if(textureused)
		fColor =(color*texture(tex, Texcoord));
	else
		fColor = color;// there is no texture so just use the color
}
