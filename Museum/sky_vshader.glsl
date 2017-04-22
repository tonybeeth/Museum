#version 150

in vec3 vPosition;
out vec3 TexCoords;

uniform mat4 Projection;
uniform mat4 ModelView;

void main()
{
	gl_Position = Projection * ModelView * vec4(vPosition, 1.0);
	TexCoords = vPosition;
}