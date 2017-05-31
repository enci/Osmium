#include <Graphics/Label.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>
#include <utf8/utf8.h>

#include <Defines.h>
#include <Utils.h>
#include <Graphics/OpenGL.h>

#define _CRT_SECURE_NO_WARNINGS

using namespace std;
using namespace Osm;

const uint maxBufferWidth = 1028;
const uint maxBufferHeight = 512;

namespace
{
uint NextPowerOfTwo(uint val)
{
	uint t = 2;
	while (t < val) t *= 2;
	return t;
}	
}

unsigned char ttf_buffer[1 << 20];

Label::Label(	const string& labelText,
				const string& fontFilename,
				unsigned fontsize,
				float maxWidth)
	: _fontsize(fontsize)
	, _maxWidth(maxWidth)
{
	// We'll start basic by caching just the raw font blob,
	//auto fontdata = ReadFile(fontFilename);

#ifdef _WIN32 // defined to 32 and 64
	FILE* file;
	if (fopen_s(&file, fontFilename.c_str(), "rb") == 0)
		fread(ttf_buffer, 1, 1 << 20, file);
	else
		ASSERT(false);
	fclose(file);
#elif 
	fread(ttf_buffer, 1, 1 << 20, fopen(fontFilename.c_str(), "rb"));
#endif

	stbtt_fontinfo fontinfo;

	stbtt_InitFont(&fontinfo, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0));

	const auto& text = labelText;

	Vector2 outSize, outUV;

	// Check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
	auto end_it = utf8::find_invalid(text.begin(), text.end());
	if (end_it != text.end())
		LOG("Invalid UTF-8 encoding detected");

	vector<uint> utf32text;
	utf8::utf8to32(text.begin(), text.end(), back_inserter(utf32text));
	
	int ascent;
	int descent;
	int lineGap;
	float scale = stbtt_ScaleForPixelHeight(&fontinfo, fontsize); // get the scale for certain
	stbtt_GetFontVMetrics(&fontinfo, &ascent, &descent, &lineGap);                          // get the the ascent

	int bottom = (int)((ascent - descent + lineGap) * scale);                              // calculate the bottom in pixels
	int baseline = (int)((ascent + lineGap) * scale);                                      // calculate the baseline in pixels


	unsigned char* buffer = static_cast<unsigned char*>(calloc(maxBufferWidth * maxBufferHeight, sizeof(char)));

	float xpos = 0;
	float ymax = -FLT_MAX;
	for (int ch = 0; ch < utf32text.size(); ++ch)
	{
		auto codepoint = utf32text[ch];
		int advance, lsb, x0, y0, x1, y1;
		float x_shift = xpos - (float)floor(xpos);

		stbtt_GetCodepointHMetrics(&fontinfo,                   // font
			(int)codepoint,              // current
			&advance,                    // advance
			&lsb);                       // left side bearing

		stbtt_GetCodepointBitmapBoxSubpixel(&fontinfo,          // font
			(int)codepoint,     // current character
			scale,              // scale x (not in pixels)
			scale,              // scale y (not in pixels)
			x_shift,            // subpixel shift x
			0,                  // subpixel shift y
			&x0,                // x min (from)
			&y0,                // y min (from)
			&x1,                // x max (to)
			&y1);               // y max (to)

		int idx = (baseline + y0) * maxBufferWidth + (int)xpos + x0;

		if (idx < 0)
			idx = 0;

		// This output can be saved into a bitmap and
		stbtt_MakeCodepointBitmapSubpixel(
			&fontinfo,                                // font
			&buffer[idx],                             // pos in final image
			x1 - x0,                                  // output_width
			y1 - y0,                                  // output_height
			maxBufferWidth,                           // stride
			scale,                                    // scale x
			scale,                                    // scale y
			x_shift,                                  // subpixel shift x
			0,                                        // subpixel shift y
			(int)codepoint);                          // codepoint


		xpos += (advance * scale);
		if (ch < utf32text.size() - 1)
			xpos += scale * stbtt_GetCodepointKernAdvance(
				&fontinfo,
				(int)codepoint,
				(int)utf32text[ch + 1]);

		if ((y1 - y0) > ymax)
			ymax = (y1 - y0);
	}

	ymax = bottom;

	float xmax = xpos;

	outSize.x = xmax;
	outSize.y = ymax;
	_yoffset = ((ascent - descent) * 0.5f + descent) * scale * 0.25f;
	uint sizex = NextPowerOfTwo(xmax);
	uint sizey = NextPowerOfTwo(ymax + 1);

	outUV.x = ((float)xmax) / sizex;
	outUV.y = ((float)ymax) / sizey;

	std::stringstream ss;
	// We're encapsulating strings in single quotes, and replacing existing
	// quotes with an escaped variant. This is OK-robust, but once iOS has
	// proper c++11 regex support, this should be replaced.
	ss << "label:'" << Osm::StringReplace(text, "'", "\'")
		<< "' font:'" << Osm::StringReplace(_font, "'", "\'")
		<< "' size:'" << fontsize << "'";

	// Update the internal details:
	_resourcePath = ss.str();
	_resourceID = StringHash(_resourcePath);

	// Set the texture internal properties:
	_size = outSize;
	_uvTo = outUV;
	_width = sizex;
	_height = sizey;
	auto size = _width * _height; // *4;
	GLubyte*  imageData = (GLubyte*)calloc(size, 1);

	if (_texture != 0)
	{
		glDeleteTextures(1, &_texture);
		_texture = 0;
	}

	glGenTextures(1, &_texture);               // Gen
	
	glBindTexture(GL_TEXTURE_2D, _texture);    // Bind
	
	//
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // Minmization
	
	//
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // Magnification
	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	

	// Draw a solid label background:
	for (int i = 0; i < _height; i++)
	{
		for (int j = 0; j < _width; j++)
		{
			unsigned char c = buffer[i * maxBufferWidth + j];

			// Base index in buffer
			int baseIdx = (i * _width + j); // *4;

			imageData[baseIdx] = c;
		}
	}

	glTexImage2D(GL_TEXTURE_2D,         // What (target)
		0,								// Mip-map level
		GL_R8,							// Internal format
		_width,                         // Width
		_height,                        // Height
		0,								// Border
		GL_RED,							// Format (how to use)
		GL_UNSIGNED_BYTE,				// Type   (how to intepret)
		imageData);						// Data

	

	free(buffer);
	free(imageData);
}