#include <sstream>
#include <fstream>
#include <Utils.h>
#include <Defines.h>

using namespace std;
using namespace Osm;

string Osm::ReadFile(const string& filename)
{
	unsigned int length;
	char* buffer;

	ifstream is;
	is.open(filename.c_str(), ifstream::in);

	// Check for bad paths
	if (!is.is_open())
	{
		LOG("Unable to open file: %s\n", filename.c_str());
		return "";
	}

	// get length of file:
	is.seekg(0, ios::end);
	length = static_cast<unsigned int>(is.tellg());
	is.seekg(0, ios::beg);

	//!! Create a null terminated string !!

	// allocate memory
	buffer = new char[length + 1];
	memset(buffer, 0, length + 1);

	// read data as a block:
	is.read(buffer, length + 1);
	is.close();

	// NB: specifying a length parameter will solve any null byte issues, even when
	// reading arbitrary binary data.
	string ret = string(buffer, length + 1);
	delete[] buffer;

	return ret;
}

bool Osm::SaveFile(const std::string& filename,
	const std::string& text)
{
	ofstream os;
	os.open(filename.c_str(), ofstream::out);

	// Check for bad paths
	if (!os.is_open())
	{
#ifdef DEBUG
		LOG("Unable to open file for writing: %s\n", filename.c_str());
#endif
		return false;
	}


	// get length of file:
	os.write(text.c_str(), text.size());

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Strig hashing function
// Taken from http://www.cse.yorku.ca/~oz/hash.html (public domain)
////////////////////////////////////////////////////////////////////////////////
ullong Osm::StringHash(const std::string& str)
{
	ullong hash = 0;
	int c;
	const char* cstr = str.c_str();

	while ((c = *cstr++))
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}

////////////////////////////////////////////////////////////////////////////////
// StringReplace
// Courtesy of: http://stackoverflow.com/questions/5878775/how-to-find-and-replace-string
////////////////////////////////////////////////////////////////////////////////
string Osm::StringReplace(const std::string& subject,
	const std::string& search,
	const std::string& replace)
{
	std::string result(subject);
	size_t pos = 0;

	while ((pos = subject.find(search, pos)) != std::string::npos)
	{
		result.replace(pos, search.length(), replace);
		pos += search.length();
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////
// StringStartsWith
// Determine if a string starts with a given string.
////////////////////////////////////////////////////////////////////////////////
bool Osm::StringStartsWith(const std::string& subject,
	const std::string& prefix)
{
	// Early out, prefix is longer than the subject:
	if (prefix.length() > subject.length())
	{
		return false;
	}

	// Compare per character:
	for (size_t i = 0; i < prefix.length(); ++i)
	{
		if (subject[i] != prefix[i])
		{
			return false;
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////
// StringEndsWith
// Determine if a string ends with a given string.
////////////////////////////////////////////////////////////////////////////////
bool Osm::StringEndsWith(const std::string& subject,
	const std::string& suffix)
{

	// Early out test:
	if (suffix.length() > subject.length())
	{
		return false;
	}

	// Resort to difficult to read C++ logic:
	return subject.compare(
		subject.length() - suffix.length(),
		suffix.length(),
		suffix
	) == 0;
}


void Osm::UpdateMatrix44FromMatrix33(Matrix44& output, const Matrix33& input)
{
	output.m[0][0] = input.m[0][0];
	output.m[0][2] = input.m[0][1];
	//
	output.m[2][0] = input.m[1][0];
	output.m[2][2] = input.m[1][1];
	//
	output.m[3][0] = input.m[2][0];
	output.m[3][2] = input.m[2][1];
}

Osm::SphericalCoordinates Osm::CartesianToSpherical(const Vector3& cvec)
{
	SphericalCoordinates scoord;
	scoord.r		= sqrt(cvec.x*cvec.x + cvec.y*cvec.y + cvec.z*cvec.z);
	scoord.theta	= acos(cvec.y / scoord.r);
	float ratio		= cvec.z / cvec.x;
	scoord.fi		= isnan(ratio) ? 0.0f : atan(ratio);
	return scoord;
}

Vector3 Osm::SphericalToCartesian(const SphericalCoordinates& scoord)
{
	Vector3 cvec;	
	cvec.x = scoord.r * sin(scoord.theta) * cosf(scoord.fi);
	cvec.y = scoord.r * cosf(scoord.theta);
	cvec.z = scoord.r * sinf(scoord.theta) * sinf(scoord.fi);
	return cvec;
}

Vector3 Osm::RandomOnUnitSphere()
{
	Vector3 v;
	do
	{
		v = Vector3(RandInRange(-1.0f, 1.0f),
			RandInRange(-1.0f, 1.0f),
			RandInRange(-1.0f, 1.0f));
	} 
	while (v.SquareMagnitude() > 1.0f);

	return v;
}
