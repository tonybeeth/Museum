#version 150

// This is NOT a Phong fragment shader 
in  vec4 vPosition;
in  vec4 vNormal;
in  vec2 Texcoord;

out vec4 fColor;
uniform sampler2D tex;
uniform bool textureused;

in vec4 color;

void main()
{
	if(textureused)
		fColor =(color*texture(tex, Texcoord));
	else
		fColor = color;// there is no texture so just use the color
}
