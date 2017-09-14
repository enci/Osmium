#include <Math/Matrix44.h>

#include <algorithm>

Osm::Matrix44::Matrix44(	float m00, float m01, float m02, float m03,
					float m10, float m11, float m12, float m13,
					float m20, float m21, float m22, float m23,
					float m30, float m31, float m32, float m33)
{
	m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
	m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
	m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
	m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
}

Osm::Matrix44::Matrix44()
{
	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}

Osm::Vector3 Osm::Matrix44::operator*(const Vector3& vec) const
{
	return Vector3(
		vec.x * m[0][0] +
		vec.y * m[1][0] +
		vec.z * m[2][0] + m[3][0],

		vec.x * m[0][1] +
		vec.y * m[1][1] +
		vec.z * m[2][1] + m[3][1],

		vec.x * m[0][2] +
		vec.y * m[1][2] +
		vec.z * m[2][2] + m[3][2]);
}

Osm::Vector4 Osm::Matrix44::operator*(const Vector4& vec) const
{
	Vector4 res(f[0] * vec.f[0] + f[4] * vec.f[1] + f[8] * vec.f[2] + f[12] * vec.f[3],
		f[1] * vec.f[0] + f[5] * vec.f[1] + f[9] * vec.f[2] + f[13] * vec.f[3],
		f[2] * vec.f[0] + f[6] * vec.f[1] + f[10] * vec.f[2] + f[14] * vec.f[3],
		f[3] * vec.f[0] + f[7] * vec.f[1] + f[11] * vec.f[2] + f[15] * vec.f[3]);
	return res;
}

Osm::Vector3 Osm::Matrix44::GetTranslation() const
{
	return Vector3(m[3][0], m[3][1], m[3][2]);
}

void Osm::Matrix44::SetTranslation(const Vector3& vec)
{
	m[3][0] = vec.x;
	m[3][1] = vec.y;
	m[3][2] = vec.z;
}

Osm::Vector3 Osm::Matrix44::GetXAxis() const
{
	return Vector3(m[0][0], m[0][1], m[0][2]);
}

Osm::Vector3 Osm::Matrix44::GetYAxis() const
{
	return Vector3(m[1][0], m[1][1], m[1][2]);
}

Osm::Vector3 Osm::Matrix44::GetZAxis() const
{
	return Vector3(m[2][0], m[2][1], m[2][2]);
}

/*
Vector3 Matrix44::GetScale() const
{
	// Extract the orientation vectors
	const Matrix44& trns = transform.local;
	Vector3 x(trns.m00, trns.m01, trns.m02);
	Vector3 y(trns.m10, trns.m11, trns.m12);
	Vector3 z(trns.m20, trns.m21, trns.m22);

	// Get their scale
	return Vector3(x.Magnitude(), y.Magnitude(), z.Magnitude());
}
*/

Osm::Matrix44 Osm::Matrix44::CreateIdentity()
{
	return Matrix44();
}

Osm::Matrix44 Osm::Matrix44::CreateTranslation(float x, float y, float z)
{
	Matrix44 result;
	//
	result.m[3][0] = x;
	result.m[3][1] = y;
	result.m[3][2] = z;
	//
	return result;
}

Osm::Matrix44 Osm::Matrix44::CreateTranslation(const Vector3& vec)
{
	return CreateTranslation(vec.x, vec.y, vec.z);
}

Osm::Matrix44 Osm::Matrix44::CreateScale(Vector3 scale)
{
	// Set zeros
	Matrix44 res;
	memset(static_cast<void*>(res.m), 0x0, 4 * 4 * sizeof(float));
	//
	res.m[0][0] = scale.x;
	res.m[1][1] = scale.y;
	res.m[2][2] = scale.z;
	res.m[3][3] = 1.0f;
	//
	return res;
}

Osm::Matrix44 Osm::Matrix44::operator+(const Matrix44& mat) const
{
	Matrix44 res;
	//
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			res.m[i][j] = m[i][j] + mat.m[i][j];

	return res;
}

Osm::Matrix44 Osm::Matrix44::operator-(const Matrix44& mat) const
{
	Matrix44 res;
	//
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			res.m[i][j] = m[i][j] - mat.m[i][j];

	return res;
}

Osm::Matrix44 Osm::Matrix44::operator*(const Matrix44& mat) const
{
	// Matrix multiplication, slow but reliable-ish :)

	Matrix44 res;
	//
	for (int i = 0; i<4; i++)
	{
		res.m[i][0] = mat.m[i][0] * m[0][0] +
			mat.m[i][1] * m[1][0] +
			mat.m[i][2] * m[2][0] +
			mat.m[i][3] * m[3][0];

		res.m[i][1] = mat.m[i][0] * m[0][1] +
			mat.m[i][1] * m[1][1] +
			mat.m[i][2] * m[2][1] +
			mat.m[i][3] * m[3][1];

		res.m[i][2] = mat.m[i][0] * m[0][2] +
			mat.m[i][1] * m[1][2] +
			mat.m[i][2] * m[2][2] +
			mat.m[i][3] * m[3][2];

		res.m[i][3] = mat.m[i][0] * m[0][3] +
			mat.m[i][1] * m[1][3] +
			mat.m[i][2] * m[2][3] +
			mat.m[i][3] * m[3][3];
	}
	//
	return res;
}

Osm::Matrix44 Osm::Matrix44::CreateRotate(float angle, const Vector3& axis)
{
	float c = cos(angle);
	float cp = 1.0f - c;
	float s = sin(angle);

	Matrix44 m(c + cp * axis.f[0] * axis.f[0],
		cp * axis.f[0] * axis.f[1] + axis.f[2] * s,
		cp * axis.f[0] * axis.f[2] - axis.f[1] * s,
		0.0f,
		cp * axis.f[0] * axis.f[1] - axis.f[2] * s,
		c + cp * axis.f[1] * axis.f[1],
		cp * axis.f[1] * axis.f[2] + axis.f[0] * s,
		0.0f,
		cp * axis.f[0] * axis.f[2] + axis.f[1] * s,
		cp * axis.f[1] * axis.f[2] - axis.f[0] * s,
		c + cp * axis.f[2] * axis.f[2],
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f);

	return m;
}

Osm::Matrix44 Osm::Matrix44::CreateRotateX(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	//
	Matrix44 m(	1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, c, s, 0.0f,
				0.0f, -s, c, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
	//
	return m;
}

Osm::Matrix44 Osm::Matrix44::CreateRotateY(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	//
	Matrix44 m(c, 0.0f, -s, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		s, 0.0f, c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	//
	return m;
}

Osm::Matrix44 Osm::Matrix44::CreateRotateZ(float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);
	//
	Matrix44 m(c, s, 0.0f, 0.0f,
		-s, c, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	//
	return m;
}

Osm::Matrix44 Osm::Matrix44::CreateFrustum(float left, float right, float bottom, float top, float nearZ, float farZ)
{
	float       deltaX = right - left;
	float       deltaY = top - bottom;
	float       deltaZ = farZ - nearZ;
	//
	Matrix44    frust;

	if ((nearZ <= 0.0f) || (farZ <= 0.0f) ||
		(deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f))
		return frust;

	frust.m[0][0] = 2.0f * nearZ / deltaX;
	frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

	frust.m[1][1] = 2.0f * nearZ / deltaY;
	frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

	frust.m[2][0] = (right + left) / deltaX;
	frust.m[2][1] = (top + bottom) / deltaY;
	frust.m[2][2] = -(nearZ + farZ) / deltaZ;
	frust.m[2][3] = -1.0f;

	frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
	frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

	return frust;
}

Osm::Matrix44 Osm::Matrix44::CreatePerspective(float fovy, float aspect, float nearZ, float farZ)
{
	float cotan = 1.0f / tanf(fovy / 2.0f);

	Matrix44 m(cotan / aspect, 0.0f, 0.0f, 0.0f,
		0.0f, cotan, 0.0f, 0.0f,
		0.0f, 0.0f, (farZ + nearZ) / (nearZ - farZ), -1.0f,
		0.0f, 0.0f, (2.0f * farZ * nearZ) / (nearZ - farZ), 0.0f);
	return m;
}

Osm::Matrix44 Osm::Matrix44::CreateOrtho(float left, float right, float bottom, float top, float nearZ, float farZ)
{
	// Get bounds
	float       deltaX = right - left;
	float       deltaY = top - bottom;
	float       deltaZ = farZ - nearZ;
	//
	Matrix44    ortho;

	// Check bad parameters
	if ((deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f))
		return ortho;

	ortho.m[0][0] = 2.0f / deltaX;
	ortho.m[3][0] = -(right + left) / deltaX;
	ortho.m[1][1] = 2.0f / deltaY;
	ortho.m[3][1] = -(top + bottom) / deltaY;
	ortho.m[2][2] = -2.0f / deltaZ;
	ortho.m[3][2] = -(nearZ + farZ) / deltaZ;

	return ortho;
}

Osm::Matrix44 Osm::Matrix44::CreateLookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	Vector3 n = eye - center;
	n.Normalize();
	//
	Vector3 u = up.Cross(n);
	u.Normalize();
	//
	Vector3 v = n.Cross(u);

	Matrix44 m(u.f[0], v.f[0], n.f[0], 0.0f,
		u.f[1], v.f[1], n.f[1], 0.0f,
		u.f[2], v.f[2], n.f[2], 0.0f,
		(u * -1).Dot(eye),
		(v * -1).Dot(eye),
		(n * -1).Dot(eye),
		1.0f);

	return m;
}


float Osm::Matrix44::Determinant() const
{
	return  f[2] * f[5] * f[8] +
		f[1] * f[6] * f[8] +
		f[2] * f[4] * f[9] -
		f[0] * f[6] * f[9] -
		f[1] * f[4] * f[10] +
		f[0] * f[5] * f[10];
}

bool Osm::Matrix44::Invert()
{
	Matrix44 inv;
	
	inv.f[0] = f[5] * f[10] * f[15] -
		f[5] * f[11] * f[14] -
		f[9] * f[6] * f[15] +
		f[9] * f[7] * f[14] +
		f[13] * f[6] * f[11] -
		f[13] * f[7] * f[10];

	inv.f[4] = -f[4] * f[10] * f[15] +
		f[4] * f[11] * f[14] +
		f[8] * f[6] * f[15] -
		f[8] * f[7] * f[14] -
		f[12] * f[6] * f[11] +
		f[12] * f[7] * f[10];

	inv.f[8] = f[4] * f[9] * f[15] -
		f[4] * f[11] * f[13] -
		f[8] * f[5] * f[15] +
		f[8] * f[7] * f[13] +
		f[12] * f[5] * f[11] -
		f[12] * f[7] * f[9];

	inv.f[12] = -f[4] * f[9] * f[14] +
		f[4] * f[10] * f[13] +
		f[8] * f[5] * f[14] -
		f[8] * f[6] * f[13] -
		f[12] * f[5] * f[10] +
		f[12] * f[6] * f[9];

	inv.f[1] = -f[1] * f[10] * f[15] +
		f[1] * f[11] * f[14] +
		f[9] * f[2] * f[15] -
		f[9] * f[3] * f[14] -
		f[13] * f[2] * f[11] +
		f[13] * f[3] * f[10];

	inv.f[5] = f[0] * f[10] * f[15] -
		f[0] * f[11] * f[14] -
		f[8] * f[2] * f[15] +
		f[8] * f[3] * f[14] +
		f[12] * f[2] * f[11] -
		f[12] * f[3] * f[10];

	inv.f[9] = -f[0] * f[9] * f[15] +
		f[0] * f[11] * f[13] +
		f[8] * f[1] * f[15] -
		f[8] * f[3] * f[13] -
		f[12] * f[1] * f[11] +
		f[12] * f[3] * f[9];

	inv.f[13] = f[0] * f[9] * f[14] -
		f[0] * f[10] * f[13] -
		f[8] * f[1] * f[14] +
		f[8] * f[2] * f[13] +
		f[12] * f[1] * f[10] -
		f[12] * f[2] * f[9];

	inv.f[2] = f[1] * f[6] * f[15] -
		f[1] * f[7] * f[14] -
		f[5] * f[2] * f[15] +
		f[5] * f[3] * f[14] +
		f[13] * f[2] * f[7] -
		f[13] * f[3] * f[6];

	inv.f[6] = -f[0] * f[6] * f[15] +
		f[0] * f[7] * f[14] +
		f[4] * f[2] * f[15] -
		f[4] * f[3] * f[14] -
		f[12] * f[2] * f[7] +
		f[12] * f[3] * f[6];

	inv.f[10] = f[0] * f[5] * f[15] -
		f[0] * f[7] * f[13] -
		f[4] * f[1] * f[15] +
		f[4] * f[3] * f[13] +
		f[12] * f[1] * f[7] -
		f[12] * f[3] * f[5];

	inv.f[14] = -f[0] * f[5] * f[14] +
		f[0] * f[6] * f[13] +
		f[4] * f[1] * f[14] -
		f[4] * f[2] * f[13] -
		f[12] * f[1] * f[6] +
		f[12] * f[2] * f[5];

	inv.f[3] = -f[1] * f[6] * f[11] +
		f[1] * f[7] * f[10] +
		f[5] * f[2] * f[11] -
		f[5] * f[3] * f[10] -
		f[9] * f[2] * f[7] +
		f[9] * f[3] * f[6];

	inv.f[7] = f[0] * f[6] * f[11] -
		f[0] * f[7] * f[10] -
		f[4] * f[2] * f[11] +
		f[4] * f[3] * f[10] +
		f[8] * f[2] * f[7] -
		f[8] * f[3] * f[6];

	inv.f[11] = -f[0] * f[5] * f[11] +
		f[0] * f[7] * f[9] +
		f[4] * f[1] * f[11] -
		f[4] * f[3] * f[9] -
		f[8] * f[1] * f[7] +
		f[8] * f[3] * f[5];

	inv.f[15] = f[0] * f[5] * f[10] -
		f[0] * f[6] * f[9] -
		f[4] * f[1] * f[10] +
		f[4] * f[2] * f[9] +
		f[8] * f[1] * f[6] -
		f[8] * f[2] * f[5];

	float det = f[0] * inv.f[0] + f[1] * inv.f[4] + f[2] * inv.f[8] + f[3] * inv.f[12];

	if (det == 0)
		return false;

	det = 1.0f / det;

	for (int i = 0; i < 16; i++)
		f[i] = inv.f[i] * det;

	return true;
}


void Osm::Matrix44::Transpose()
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < i; j++)
			std::swap(m[i][j], m[j][i]);
}

void Osm::Matrix44::SetOrientation(	const Vector3 &x,
									const Vector3 &y,
									const Vector3 &z)
{
	// TODO: Check orthogonality with an assert

	m[0][0] = x.x; m[0][1] = x.y; m[0][2] = x.z;
	m[1][0] = y.x; m[1][1] = y.y; m[1][2] = y.z;
	m[2][0] = z.x; m[2][1] = z.y; m[2][2] = z.z;
}

void Osm::Matrix44::SetEulerAngles(const Vector3& angles)
{
	*this = CreateRotateX(angles.x) * CreateRotateY(angles.y) * CreateRotateZ(angles.z);
}

Osm::Vector3 Osm::Matrix44::TransformDirectionVector(const Vector3& dir)
{
	Vector3 res(dir.x * m[0][0] +
		dir.y * m[1][0] +
		dir.z * m[2][0],

		dir.x * m[0][1] +
		dir.y * m[1][1] +
		dir.z * m[2][1],

		dir.x * m[0][2] +
		dir.y * m[1][2] +
		dir.z * m[2][2]);
	return res;
}

Osm::Vector3 Osm::Matrix44::GetEulerAngles() const
{
	float sy = sqrt(m[0][0] * m[0][0] + m[1][0] * m[1][0]);

	bool singular = sy < 1e-6; // If

	float x, y, z;
	if (!singular)
	{
		x = atan2(m[2][1], m[2][2]);
		y = atan2(-m[2][0], sy);
		z = atan2(m[1][0], m[0][0]);
	}
	else
	{
		x = atan2(-m[1][2], m[1][1]);
		y = atan2(-m[2][0], sy);
		z = 0;
	}

	return Vector3(-x, -y, -z);
}