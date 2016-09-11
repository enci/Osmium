////////////////////////////////////////////////////////////////////////////////
/// \file		ImprovedNoise.cpp
///	\brief		Improved Perlin noise
///	\author		Bojan Endrovski
///	\date		26-03-2012
///
/// This is a fast and modern and Perlin noise class based on Stefan Gustavson's
/// public domain implementation. Which in term is an implementation port Ken Perlin's
/// "Improved Noise" as presented at Siggraph 2002. Java reference code available on
/// www.noisemachine.com
////////////////////////////////////////////////////////////////////////////////

#pragma once

class ImprovedNoise
{
public:
	/// Do nothing Ctor
	ImprovedNoise() {}
    
	/// Do nothing Dtor
	~ImprovedNoise() {}

	/// 3D noise function
	static float Noise(float x, float y, float z);

	/// 3D periodic noise
    static float PeriodicNoise(float x, float y, float z, int px, int py, int pz);

 private:
    static unsigned char perm[];

	/// 3D gradient
    static float  Gradient(int hash, float x, float y , float z);
};
