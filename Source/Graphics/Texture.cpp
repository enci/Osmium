#include <Graphics/Texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "Defines.h"
using namespace Osm;


Texture::Texture(const std::string& filename) : Resource(RESOURCE_TYPE_TEXTURE)
{
	_resourcePath = filename;

	Texture::Reload();
}

Texture::Texture(int width, int height)
	: Resource(RESOURCE_TYPE_TEXTURE)
	, _texture(0)
	, _width(width)
	, _height(height)
{}

Texture::~Texture()
{
	if (_texture)
		glDeleteTextures(1, &_texture);
}

void Texture::CreateGLTextureWithData(GLubyte* data, int width, int height, int channels, bool genMipMaps)
{
	// This is a public method, so the fields
	// might undefined before this call
	_width = width;
	_height = height;
	GLint format = GL_INVALID_VALUE;
	GLint usage = GL_INVALID_VALUE;
	_channels = channels; 
	switch (channels)
	{
	case 1:
		format = GL_R8;
		usage = GL_RED;
		break;
	case 4:
		format = GL_RGBA;
		usage = GL_RGBA;
		break;
	default:
		ASSERT(false);
	}

	if (_texture)
		glDeleteTextures(1, &_texture);

	glGenTextures(1, &_texture);											// Gen    

	glBindTexture(GL_TEXTURE_2D, _texture);                                 // Bind



	if (genMipMaps)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);    // Minmization
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);                   // Minmization

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);                       // Magnification

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(
		GL_TEXTURE_2D,						// What (target)
		0,									// Mip-map level
		format,								// Internal format
		_width,								// Width
		_height,							// Height
		0,									// Border
		usage,								// Format (how to use)
		GL_UNSIGNED_BYTE,					// Type   (how to intepret)
		data);								// Data

	if (genMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::Reload()
{
	GLubyte* data = stbi_load(_resourcePath.c_str(), &_width, &_height, &_channels, 4);

	if (data == nullptr)
	{
		LOG("Image %s could not be loaded!", _resourcePath.c_str());
	}
	else
	{
		CreateGLTextureWithData(data, _width, _height, 4, true);
		stbi_image_free(data);
	}
}

RenderTarget::RenderTarget(GLuint id)
{
	_texture = id;
}
