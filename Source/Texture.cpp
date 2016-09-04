#include <Texture.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace igad;

Texture::Texture(const std::string& filename)
{
	GLubyte* data = stbi_load(filename.c_str(), &_width, &_height, &_channels, 4);

	if(data == nullptr)
	{
		LOG("Image %s could not be loaded!", filename.c_str());
	}
	else
	{
		CreateGLTextureWithData(data, true);
		stbi_image_free(data);
	}
}

Texture::Texture(int width, int height) : _texture(0), _width(width), _height(height) {}

Texture::~Texture()
{
	if (_texture)
		glDeleteTextures(1, &_texture);
}

void Texture::CreateGLTextureWithData(GLubyte* data, bool genMipMaps)
{	
	if (_texture)
		glDeleteTextures(1, &_texture);

	glGenTextures(1, &_texture);											// Gen    
	GL_GET_ERROR();

	glBindTexture(GL_TEXTURE_2D, _texture);                                 // Bind
	GL_GET_ERROR();

	//
	if (genMipMaps)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);    // Minmization
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);                   // Minmization
	GL_GET_ERROR();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);                       // Magnification
	GL_GET_ERROR();


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GL_GET_ERROR();

	glTexImage2D(
		GL_TEXTURE_2D,						// What (target)
		0,									// Mip-map level
		GL_RGBA,		                    // Internal format
		_width,								// Width
		_height,							// Height
		0,									// Border
		GL_RGBA,							// Format (how to use)
		GL_UNSIGNED_BYTE,					// Type   (how to intepret)
		data);								// Data
	GL_GET_ERROR();

	if (genMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	GL_GET_ERROR();
}