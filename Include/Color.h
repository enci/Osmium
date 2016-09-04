#pragma once

#include <string>
#include <algorithm>
#include <Defines.h>

using namespace std;

namespace igad
{
    class Color
    {
    public:       
        union
        {
            // Holds all the values
            char rgba[4];		
            
            struct
            {
                // Red
                unsigned char r;
                
                // Green
                unsigned char g;
                
                // Blue
                unsigned char b;
                
                // Alpha
                unsigned char a;	
            };
            
            // Holds all the values as a 32 bit unsigned int
            unsigned int integervalue;
        };
        
    public:
        /// Ctor as black
        Color(): r(0), g(0), b(0), a(255) {}
        
        /// Full ctor 
        Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
        : r(red), g(green), b(blue), a(alpha) {}
        
        /// Opaque ctor 
        Color(unsigned char red, unsigned char green, unsigned char blue)
        : r(red), g(green), b(blue), a(255) {}
        
		/// Constructor from a string represting a hex vavlue. Used for copy/pasting values from
		/// Photoshop or similar.
        Color(string hex);
        
        /// Returns the value of the given color added to this. Doesn't check range
        Color operator+(const Color& c) const
        {
            return Color( uchar(r+c.r), uchar(g+c.g), uchar(b+c.b), uchar(a+c.a));
        }
        
        /// Returns the value of the given color added to this.
        Color operator*(const Color& c) const;

        
        /// Returns the value of the given color subtracted from this.
        Color operator-(const Color& c) const
        {
			return Color(uchar(r-c.r), uchar(g-c.g), uchar(b-c.b), uchar(a-c.a));
        }
        
        /// Returns a copy of this color scaled the given value.
        Color operator*(const float value) const
        {
            return Color(uchar(r*value), uchar(g*value), uchar(b*value), uchar(a*value));
        }
        
        /// Multiplies this vector by the given scalar.
        void operator*=(const float value)
        {
            r = uchar(min(r * value, 255.0f));
            g = uchar(min(g * value, 255.0f));
            b = uchar(min(b * value, 255.0f));
            a = uchar(min(a * value, 255.0f));
        }
        
        /// Multiplies this vector by the given scalar.
        void operator+=(const Color& c)
        {
            r += c.r;
            g += c.g;
            b += c.b;
            a += c.a;
        }
        
        /// Checks if two colors are the same
        bool operator==(const Color& c) const
        {
            return r == c.r && g == c.g && b == c.b && a == c.a;
        }

        /// Checks if two colors are different
        bool operator!=(const Color& c) const
        {
            return !(r == c.r && g == c.g && b == c.b && a == c.a);
        }
                                            
        // Define some quick colors
        static Color Red;
        static Color Green;
        static Color Blue;
        static Color White;
        static Color Black;
        static Color Yellow;
        static Color Orange;
        static Color Purple;
        static Color Cyan;
        static Color Magenta;
        static Color Brown;
        static Color Grey;
        static Color CornFlower;
        
        static Color WhiteTransparent;
        static Color BlackTransparent;
        
        Color Transparent(float opacity = 0.0f) const;
    };
    
    /// Returns a copy of the color scaled the given value.
    inline Color operator*(float value, Color c)
    {
		return c * value;
    }
    
    

}

// end //
