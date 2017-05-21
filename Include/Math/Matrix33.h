#pragma once

#include <cmath>
#include <algorithm>
#include <Math/Vector2.h> 

namespace  Osm
{

/// A matrix implemetation for transforming 2D vectors,
/// and sometimes 3D vectors
class Matrix33
{
public:
    union
    {
        float f[9];
        float m[3][3];
        struct
        {
            float   m11, m12, m13,
                    m21, m22, m23,
                    m31, m32, m33;
        };
    };
    
        
    /// Do care values init
    Matrix33()
    {
        SetIdentity();
    }
        
    /// Totally care values init
    Matrix33(float _m00, float _m01, float _m02,
             float _m10, float _m11, float _m12,
             float _m20, float _m21, float _m22)
    {
        m11 = _m00; m12 = _m01; m13 = _m02;
        m21 = _m10; m22 = _m11; m23 = _m12;
        m31 = _m20; m32 = _m21; m33 = _m22;
    }
        
    /// Default up vector if this matrix is applied as transormation
    /// @return the up vector of this matrix
    Vector2 Up() const                   { return Vector2(m21, m22); }
        
    /// Set the up vector
    /// @up New up vector
    void SetUp(const Vector2& up)        { m21 = up.x; m22 = up.y; }
        
    /// Right vector if this matrix is applied as transormation
    /// @return The right vector
    Vector2 Right() const                { return Vector2(m11, m12); }
        
    void SetRight(const Vector2& fwd)    { m11 = fwd.x; m12 = fwd.y; }
    
    /// Get translation from origin vector
    inline Vector2 Translation() const          { return Vector2(m31, m32); }
             
    /// Set translation without changing the rest of the properties.
    inline void SetTranslation(const Vector2& translation)
    { m31 = translation.x; m32 = translation.y; m33 = 1.0f; }

        
    /// Sets the matrix to identity
    inline void SetIdentity()
    {
        m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
        m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
        m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
    }
        
                
    /// Set orientation and traslation. To keep transformations rigid,
    ///  the up and right vectors must be perpedicular and normalized.
    ///
    /// @up             New up vector
    /// @size			New size vector
    /// @translation    New position vector
    inline void SetTransform(const Vector2& up,
                             const Vector2& size,
                             const Vector2& translation)
    {
        m11 = up.y * size.x;    m12 = -up.x * size.y;   m13 = 0.0f;
        m21 = up.x * size.x;	m22 = up.y * size.y;    m23 = 0.0f;
        m31 = translation.x;    m32 = translation.y;	m33 = 1.0f;
    }
        
        
    /// Set the rotation of the matrix. This does not effect the trasnaltion
    inline void SetRotation(float theta)
    {
        m11 = cosf(theta);  m12 = sinf(theta);
        m21 = -sinf(theta);  m22 = m11;
    }
        
    /// Multiplies this matrix with an another one and saves the result in the current one.
    Matrix33 operator*(const Matrix33& matrix) const
    {
		return Matrix33(            
		matrix.m11 * m11 + matrix.m12 * m21 + matrix.m13 * m31,
		matrix.m11 * m12 + matrix.m12 * m22 + matrix.m13 * m32,
		matrix.m11 * m13 + matrix.m12 * m23 + matrix.m13 * m33,            

		matrix.m21 * m11 + matrix.m22 * m21 + matrix.m23 * m31,
        matrix.m21 * m12 + matrix.m22 * m22 + matrix.m23 * m32,
        matrix.m21 * m13 + matrix.m22 * m23 + matrix.m23 * m33,            

        matrix.m31 * m11 + matrix.m32 * m21 + matrix.m33 * m31,
        matrix.m31 * m12 + matrix.m32 * m22 + matrix.m33 * m32,
        matrix.m31 * m13 + matrix.m32 * m23 + matrix.m33 * m33
		);
    }

	/// Multiply with a scalar
	///
	/// @f Scalar value
	///
	void Multiply(float f)
	{
		m11 *= f; m12 *= f; m13 *= f;
		m21 *= f; m22 *= f; m23 *= f;
		m31 *= f; m32 *= f; m33 *= f;
	}
        
    /// Inverse of this matrix
    ///
    /// @return The inverse matrix if there is one, undefined otherwise
    ///
    Matrix33 Inverse() const
    {
        float determinant = m11 * (m33 * m22 - m32 * m23)
                            - m21 * (m33 * m12 - m32 * m13)
                            + m31 * (m23 * m12 - m22 * m13);
        //
        float invrdet = 1.0f / determinant;
            
        Matrix33 t;
        //
        t.m11 = m33 * m22 - m32 * m23;
        t.m12 = -(m33 * m12 - m32 * m13);
        t.m13 = m23 * m12 - m22 * m13;
        //
        t.m21 = -(m33 * m21 - m31 * m23);
        t.m22 = m33 * m11 - m31 * m13;
        t.m23 = -(m23 * m11 - m21 * m13);
        //
        t.m31 = m32 * m21 - m31 * m22;
        t.m32 = -(m32 * m11 - m31 * m12);
        t.m33 = m22 * m11 - m21 * m12;
            
        t.Multiply(invrdet);
        //
        return t;
    }
        
    /// Transform a 2D vector
    /// @vector Transforms this vector
    Vector2 TransformVector(const Vector2& vector) const
	{
		return Vector2(
			(m11 * vector.x) + (m21 * vector.y) + m31,
			(m12 * vector.x) + (m22 * vector.y) + m32);
	}
        
    /// Tranforms a normal vector, does not affect translation
    Vector2 TransformNormal(const Vector2& vector) const
	{
		return Vector2(
			(m11 * vector.x) + (m21 * vector.y),
			(m12 * vector.x) + (m22 * vector.y));
	}
        

	/*
    /// Invert this matrix
    inline Matrix33& Invert()
    {
        *this = Inverse();
        return *this;
    }
        
        
    ///
    inline Matrix33& Transpose()
    {
        std::swap(m12, m21);
        std::swap(m13, m31);
        std::swap(m23, m32);
        return *this;
    }
	*/
        
/*
        
    static Matrix33 CreateScale(Vector2 scale)
    {
        // Set zeros
        Matrix33 res;
        memset((void*)res.m, 0x0, 3 * 3 * sizeof(float));
        //
        res.m[0][0] = scale.x;
        res.m[1][1] = scale.y;
        res.m[2][2] = 1.0f;
        //
        return res;
    }


	static Matrix33 CreateRotation(float theta)
    {
		Matrix33 mat;
		mat.SetIdentity();
		mat.SetRotation(theta);
		return mat;
    }

	static Matrix33 CreateOrientation()
    {
	    
    }

	static Matrix33 CreateTranslation()
    {
	    
    }
*/
        
    /// The identity matrix
    static Matrix33 Identity;
};
}