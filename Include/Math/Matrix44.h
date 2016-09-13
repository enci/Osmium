#pragma once

#include <Math/Vector3.h>
#include <Math/Vector4.h>

namespace Osm
{

struct Matrix44
{
	union
	{
		float m[4][4];
		float f[16];
		struct
		{
			Vector3 xAxis;
			float wx;
			Vector3 yAxis;
			float wy;
			Vector3 zAxis;
			float wz;
			Vector3 translation;
			float one;
		};
	};

	/// Construct a new matrix from explicit values
	Matrix44(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);

	/// Construct a new identity matrix
	Matrix44();

	/// Transform the given vector by this matrix.
	///
	/// @param vec vector that is asssumed to be homogenuos with w=1
	/// @return Resulting vector is asssumed to be homogenuos with w=1
	Vector3 operator*(const Vector3& vec) const;

	/// Transform the given vector by this matrix.
	///
	Vector4 operator*(const Vector4& vec) const;

	/// Matrix addition
	///
	/// @param mat Right side operand
	Matrix44 operator+(const Matrix44& mat) const;

	/// Matrix substraction
	///
	/// @param mat Right side operand
	Matrix44 operator-(const Matrix44& mat) const;

	/// Matrix multiplication
	///
	/// @param mat Right side operand
	Matrix44 operator*(const Matrix44& mat) const;

	/// Translation bit of the matrix
	Vector3 GetTranslation() const;
	
	/// Set the transltion of the matrix
	void SetTranslation(const Vector3& vec);

	/// Get the x orientation axis 
	Vector3 GetXAxis() const;

	/// Get the y orientation axis 
	Vector3 GetYAxis() const;

	/// Get the z orientation axis 
	Vector3 GetZAxis() const;

	/// Gets the local scale. Might be non uniform.
	//Vector3 GetScale() const;

	/// Get the determinant of this matrix
	float Determinant() const;

	/// Inverts this matrix
	bool Invert();

	/// Transposes this matrix
	void Transpose();


	/// Sets the orientation of the matrix to the orthogonal basis vector
	/// It perfoms no checks on the orthogonality!
	///
	/// @param x X orthogonal basis vector
	/// @param y Y orthogonal basis vector
	/// @param z Z orthogonal basis vector
	void SetOrientation(const Vector3& x,
						const Vector3& y,
						const Vector3& z);

	/// Set orientation using Euler angles. Broken at current!
	void SetEulerAxis(float yaw, float pitch, float roll);

	/// Creates an identity matrix
	///
	/// @return Identity matrix
	static Matrix44 CreateIdentity();

	/// Creates a transation matrix
	///
	/// @return Translation matrix
	static Matrix44 CreateTranslation(float x, float y, float z);

	static Matrix44 CreateScale(Vector3 scale);

	/// Creates a rotation matrix around an arbitrary axis
	static Matrix44 CreateRotate(float angle, const Vector3& axis);

	/// Angle in radians
	static Matrix44 CreateRotateX(float angle);

	/// Angle in radians
	static Matrix44 CreateRotateY(float angle);

	/// Angle in radians
	static Matrix44 CreateRotateZ(float angle);

	/// Creates an orthographic projection matrix
	static Matrix44 CreateOrtho(float left, float right, float bottom, float top, float nearZ, float farZ);

	/// Creates a frustum projection matrix
	static Matrix44 CreateFrustum(float left, float right, float bottom, float top, float nearZ, float farZ);

	/// Creates a perspective projection matrix from camera settings
	static Matrix44 CreatePerspective(float fovy, float aspect, float nearZ, float farZ);

	/// Creates a look at matrix, usualy a view matrix  
	static Matrix44 CreateLookAt(const Vector3& eye, const Vector3& center, const Vector3& up);

	/// Transfrom just the direction
	Vector3 TransformDirectionVector(const Vector3& direction);
};

}

