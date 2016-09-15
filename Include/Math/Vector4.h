

#pragma once

#include <math.h>

namespace Osm
{
    /// A logical 4D vector. It stores 4 values for padding and
    /// homogenuos coordinates
    class Vector4
    {
    public:
        union
        {
            /// Holds all the values
            float f[4];
            
            struct
            {
                /// Holds the value along the x axis.
                float x;
                
                /// Holds the value along the y axis.
                float y;
                
                /// Holds the value along the z axis.
                float z;
                
                /// Padding to ensure 4 word alignment.
                float w;
            };
        };
        
    public:
        /// The default constructor creates a zero vector.
        Vector4() : x(0), y(0), z(0), w(1) {}
        
        /// The explicit constructor creates a vector with the given components.
        Vector4(float x, float y, float z)
        : x(x), y(y), z(z) , w(1) {}

        /// The explicit constructor creates a vector with the given components.
        Vector4(float x, float y, float z, float w)
        : x(x), y(y), z(z) , w(w) {}

        
        float operator[](unsigned i) const
        {
            if (i == 0) return x;
            if (i == 1) return y;
            if (i == 2) return z;
            return w;
        }
        
        float& operator[](unsigned i)
        {
            if (i == 0) return x;
            if (i == 1) return y;
            if (i == 2) return z;
            return w;
        }
        
        /// Returns the value of the given vector added to this.
        Vector4 operator+(const Vector4& v) const
        {
            return Vector4(x+v.x, y+v.y, z+v.z);
        }
        
        /// Returns the value of the given vector subtracted from this.
        Vector4 operator-(const Vector4& v) const
        {
            return Vector4(x-v.x, y-v.y, z-v.z);
        }
        
        /// Returns a copy of this vector scaled the given value.
        Vector4 operator*(const float value) const
        {
            return Vector4(x*value, y*value, z*value);
        }
        
        /// Calculates and returns the dot product of this vector
        /// with the given vector.
        float operator *(const Vector4 &vector) const
        {
            return x*vector.x + y*vector.y + z*vector.z;
        }
        
        /// Calculates and returns the cross product of this vector
        /// with the given vector.
        Vector4 operator%(const Vector4 &vector) const
        {
            return Vector4(y*vector.z-z*vector.y,
                           z*vector.x-x*vector.z,
                           x*vector.y-y*vector.x);
        }
        
        /// Adds the given vector to this.
        void operator+=(const Vector4& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
        }
        
        /// Subtracts the given vector from this.
        void operator-=(const Vector4& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
        }
        
        /// Multiplies this vector by the given scalar.
        void operator*=(const float value)
        {
            x *= value;
            y *= value;
            z *= value;
        }
        
        /// Calculates and returns a component-wise product of this
        /// vector with the given vector.
        Vector4 ComponentProduct(const Vector4 &vector) const
        {
            return Vector4(x * vector.x, y * vector.y, z * vector.z);
        }
        
        /// Performs a component-wise product with the given vector and
        /// sets this vector to its result.
        void ComponentProductUpdate(const Vector4 &vector)
        {
            x *= vector.x;
            y *= vector.y;
            z *= vector.z;
        }
        
        /// Calculates and returns the cross product of this vector
        /// with the given vector.
        Vector4 Cross(const Vector4 &vector) const
        {
            return Vector4(y*vector.z-z*vector.y,
                           z*vector.x-x*vector.z,
                           x*vector.y-y*vector.x);
        }
        
        /// Calculates and returns the dot product of this vector
        /// with the given vector.
        float Dot(const Vector4 &vector) const
        {
            return x*vector.x + y*vector.y + z*vector.z;
        }
        
        /// Gets the magnitude of this vector.
        float Magnitude() const
        {
            return sqrtf(x*x+y*y+z*z);
        }
        
        /// Gets the squared magnitude of this vector.
        float SquareMagnitude() const
        {
            return x*x+y*y+z*z;
        }
        
        /// Limits the size of the vector to the given maximum.
        void Trim(float size)
        {
            if (SquareMagnitude() > size*size)
            {
                Normalize();
                x *= size;
                y *= size;
                z *= size;
            }
        }
        
        /// Turns a non-zero vector into a vector of unit length.
        void Normalize()
        {
            float l = Magnitude();
            if (l > 0)
            {
                (*this) *= ((float)1)/l;
            }
        }
        
        
        void NormalizeW()
        {
            if (w != 0.0f)
            {
                x /= w;
                y /= w;
                z /= w;
                w = 1;
            }
        }
        
        /// Returns the normalised version of a vector.
        Vector4 Unit() const
        {
            Vector4 result = *this;
            result.Normalize();
            return result;
        }
        
        /// Updates this vector to be the cross product of its current
        /// value and the given vector.
        inline void operator %=(const Vector4 &vector)
        {
            *this = Cross(vector);
        }
        
        /// Checks if the two vectors have identical components.
        bool operator==(const Vector4& other) const
        {
            return x == other.x &&
            y == other.y &&
            z == other.z;
        }
        
        /// Checks if the two vectors have non-identical components.
        bool operator!=(const Vector4& other) const
        {
            return !(*this == other);
        }
        
        /// Zero all the components of the vector.
        void Clear()
        {
            x = y = z = 0;
            w = 1;
        }
        
        /// Flips all the components of the vector.
        void Invert()
        {
            x = -x;
            y = -y;
            z = -z;
            // w = -w //???
        }
    };
}

// end //
