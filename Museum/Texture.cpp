#include "Texture.h"


Texture::Texture(GLuint texUnit)
{
	this->texUnit = texUnit;
}

Texture::Texture(const char * str, GLuint texUnit, int force_channels)
{
	Init(str, texUnit, force_channels);
}

bool Texture::Init(const char* str, GLuint texUnit, int force_channels)
{
	this->texUnit = texUnit;

	cout << "Initializing texture: " << str << endl;

	//load texture using soil
	texImage = SOIL_load_image(str, &texWidth, &texHeight, 0, force_channels);

	if (texImage) {
		//generate and bind texture Id
		glActiveTexture(texUnit);
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);

		//redefine standard tex values
		//linear interpolation for magnification and minification filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//specify texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB,
			GL_UNSIGNED_BYTE, texImage);

		//deal with odd texture dimension
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

		//free texture on cpu side
		SOIL_free_image_data(texImage);
		//unbind tex id
		glBindTexture(GL_TEXTURE_2D, 0);

		cout << str << " loading successful\n";
		return true;
	}
	else {
		cout << "ERROR: could not load " << str << endl;
		return false;
	}

	texImage = NULL;
}

void Texture::Use(GLuint program, const char *shaderSampler)
{
	//Bind tex object
	glBindTexture(GL_TEXTURE_2D, texId);
	//set tex unit in shader
	glUniform1i(glGetUniformLocation(program, shaderSampler), texUnit);//Tex in texture unit
}

Texture::~Texture()
{
}
