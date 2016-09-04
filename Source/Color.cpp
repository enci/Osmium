#include "Color.h"
#include <sstream>

#define bswap_32(x) ((((x) & 0xFF000000) >> 24) | \
                     (((x) & 0x00FF0000) >> 8)  | \
                     (((x) & 0x0000FF00) << 8)  | \
                     (((x) & 0x000000FF) << 24) )

using namespace igad;

Color Color::Red        = Color(255u,	 0u,	 0u,	   255u);
Color Color::Green      = Color(0u,      255u,   0u,       255u);
Color Color::Blue       = Color(0u,      0u,     255u,     255u);
Color Color::White      = Color(255u,    255u,   255u,     255u);
Color Color::Black      = Color(0u,      0u,     0u,       255u);
Color Color::Yellow     = Color(255u,    255u,   0u,       255u);
Color Color::Orange     = Color(255u,    165u,   0u,       255u);
Color Color::Purple     = Color(160u,    32u,    240u,     255u);
Color Color::Cyan       = Color("00ffff");
Color Color::Magenta    = Color("ff00ff");
Color Color::Brown      = Color("7B4A12");
Color Color::Grey       = Color(128u,    128u,   128u,     255u);
Color Color::CornFlower = Color(154u,    206u,   235u,     255u);
Color Color::WhiteTransparent	= Color::White.Transparent();
Color Color::BlackTransparent   = Color::Black.Transparent();

Color::Color(string hexstring)
{
    if (hexstring.length() == 6)
        hexstring.append("ff");
    
    stringstream ss;
    unsigned int temp;
    ss << std::hex << hexstring;
    ss >> temp;
    integervalue = bswap_32(temp);
}

Color Color::operator*(const Color& c) const
{
    Color t(    uchar(( uint(r) * uint(c.r)) / 255),
                uchar(( uint(g) * uint(c.g)) / 255),
                uchar(( uint(b) * uint(c.b)) / 255),
                uchar(( uint(a) * uint(c.a)) / 255) );
    //
    return t;
}

Color Color::Transparent(float opacity) const
{
    return Color(this->r, this->g, this->b, uchar(opacity * 255.0f));
}

