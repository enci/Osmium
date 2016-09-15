#pragma once

#include <Graphics/Texture.h>
#include <Math/Vector2.h>

namespace Osm
{

class Label : public Texture
{
public:
	Label(const std::string& labelText, const std::string& fontFilename, unsigned fontsize, float maxWidth);

	const Vector2& GetSize() const			{ return _size; }
	const Vector2& GetUVTo() const			{ return _uvTo; }
	float YOffset() const					{ return _yoffset; }
	const std::string& FontFile() const		{ return _font; }
	unsigned FontSize() const				{ return _fontsize; }
	const std::string& OriginalText() const	{ return _labelText; }

private:
	std::string		_labelText;
	std::string		_font;
	unsigned		_fontsize;
	float			_maxWidth;
	Vector2			_size;
	Vector2			_uvTo;
	float			_yoffset;
};

}
