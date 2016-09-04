#pragma once

#include <vector>
#include <OpenGL.h>

namespace igad
{

class Texture
{
public:

	/// Create a texture by loading from a file
	/// Uses stb_image (as SOIL does). Check for supported formats
	/// in the stb_image header
	Texture(const std::string& filename);

	/// Creates an empty texture with given size. You will need to provide
	/// the data.
	Texture(int width, int height);

	/// Releases OpenGL resources
	~Texture();

	/// Get the texture ID in OpenGL
	GLuint GetTexture() const { return _texture; }

	/// Creates a texture from RGBA provided data
	void CreateGLTextureWithData(GLubyte* data, bool genMipMaps);

protected:
	/// Used for the label
	Texture() {}	

	GLuint _texture = 0;
	int _width		= 0;
	int _height		= 0;
	int _channels	= 0;
};

}
