#include <Tools/MeshGenerators.h>
#include <cstdint>
#include <unordered_map>
#include <vector>


using namespace Osm;
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////
/// Generate Icosphere
/// Based on code Written by Frank McCoy for Seed of Andromeda 
/// https://pastebin.com/aFdWi5eQ
//////////////////////////////////////////////////////////////////////////////////////////

const static float GOLDEN_RATIO = 1.61803398875f;
const static int NUM_ICOSOHEDRON_VERTICES = 12;
const static Vector3 ICOSOHEDRON_VERTICES[12] =
{
	Vector3(-1.0f, GOLDEN_RATIO, 0.0f),
	Vector3(1.0f, GOLDEN_RATIO, 0.0f),
	Vector3(-1.0f, -GOLDEN_RATIO, 0.0f),
	Vector3(1.0f, -GOLDEN_RATIO, 0.0f),

	Vector3(0.0f, -1.0f, GOLDEN_RATIO),
	Vector3(0.0f, 1.0f, GOLDEN_RATIO),
	Vector3(0.0f, -1.0, -GOLDEN_RATIO),
	Vector3(0.0f, 1.0f, -GOLDEN_RATIO),

	Vector3(GOLDEN_RATIO, 0.0f, -1.0f),
	Vector3(GOLDEN_RATIO, 0.0f, 1.0f),
	Vector3(-GOLDEN_RATIO, 0.0f, -1.0f),
	Vector3(-GOLDEN_RATIO, 0.0, 1.0f)
};

const static int NUM_ICOSOHEDRON_INDICES = 60;
const static uint32_t ICOSOHEDRON_INDICES[60] = {
	0, 11, 5,
	0, 5, 1,
	0, 1, 7,
	0, 7, 10,
	0, 10, 11,

	1, 5, 9,
	5, 11, 4,
	11, 10, 2,
	10, 7, 6,
	7, 1, 8,

	3, 9, 4,
	3, 4, 2,
	3, 2, 6,
	3, 6, 8,
	3, 8, 9,

	4, 9, 5,
	2, 4, 11,
	6, 2, 10,
	8, 6, 7,
	9, 8, 1
};

// Hash functions for the unordered map
class Vec3KeyFuncs
{
public:
	ushort operator()(const Vector3& k) const
	{
		return (ushort)(std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z));
	}

	bool operator() (const Vector3& a, const Vector3& b)const
	{
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}
};

inline Vector3 findMidpoint(Vector3 vertex1, Vector3 vertex2)
{
	Vector3 mid = Lerp(vertex1, vertex2, 0.5f);
	mid.Normalize();
	return mid;
}



void Osm::GenerateIcosphere(
	float size,
	int resolution,
	vector<VertexFormat>& vertices,
	vector<ushort>& indices)
{
	if (resolution < 0)
		return;

	std::vector<ushort> newIndices;
	newIndices.reserve(256);

	std::unordered_map<Vector3, ushort, Vec3KeyFuncs, Vec3KeyFuncs> vertexLookup;

	indices.resize(NUM_ICOSOHEDRON_INDICES);
	for (ushort i = 0; i < NUM_ICOSOHEDRON_INDICES; i++)
	{
		indices[i] = ICOSOHEDRON_INDICES[i];
	}

	vertices.resize(NUM_ICOSOHEDRON_VERTICES);
	for (ushort i = 0; i < NUM_ICOSOHEDRON_VERTICES; i++)
	{
		Vector3 pos = ICOSOHEDRON_VERTICES[i];
		pos.Normalize();
		vertices[i].Position	= pos;
		vertices[i].Normal		= pos;
		vertexLookup[pos] = i;
	}

	for (ushort i = 0; i < (ushort)resolution; i++)
	{
		for (ushort j = 0; j < (ushort)indices.size(); j += 3)
		{
			/*
			j
			mp12   mp13
			j+1    mp23   j+2
			*/
			// Defined in counter clockwise order
			Vector3 vertex1 = vertices[indices[j + 0]].Position;
			Vector3 vertex2 = vertices[indices[j + 1]].Position;
			Vector3 vertex3 = vertices[indices[j + 2]].Position;

			Vector3 midPoint12 = findMidpoint(vertex1, vertex2);
			Vector3 midPoint23 = findMidpoint(vertex2, vertex3);
			Vector3 midPoint13 = findMidpoint(vertex1, vertex3);

			ushort mp12Index;
			ushort mp23Index;
			ushort mp13Index;

			auto iter = vertexLookup.find(midPoint12);
			if (iter != vertexLookup.end())
			{ 
				// It is in the map
				mp12Index = iter->second;
			}
			else
			{
				// Not in the map
				mp12Index = (ushort)vertices.size();
				vertices.push_back({ midPoint12, midPoint12, Vector2(0,0) });
				vertexLookup[midPoint12] = mp12Index;
			}

			iter = vertexLookup.find(midPoint23);
			if (iter != vertexLookup.end())
			{ 
				// It is in the map
				mp23Index = iter->second;
			}
			else
			{
				// Not in the map
				mp23Index = (ushort)vertices.size();
				vertices.push_back({ midPoint23, midPoint23, Vector2(0,0) });
				vertexLookup[midPoint23] = mp23Index;
			}

			iter = vertexLookup.find(midPoint13);
			if (iter != vertexLookup.end())
			{ 
				// It is in the map
				mp13Index = iter->second;
			}
			else
			{ 
				// Not in the map
				mp13Index = (ushort)vertices.size();
				vertices.push_back({ midPoint13, midPoint13, Vector2(0,0) });
				vertexLookup[midPoint13] = mp13Index;
			}

			newIndices.push_back(indices[j]);
			newIndices.push_back(mp12Index);
			newIndices.push_back(mp13Index);

			newIndices.push_back(mp12Index);
			newIndices.push_back(indices[j + 1]);
			newIndices.push_back(mp23Index);

			newIndices.push_back(mp13Index);
			newIndices.push_back(mp23Index);
			newIndices.push_back(indices[j + 2]);

			newIndices.push_back(mp12Index);
			newIndices.push_back(mp23Index);
			newIndices.push_back(mp13Index);
		}

		indices.swap(newIndices);
		newIndices.clear();
	}

	for (ushort i = 0; i < (ushort)vertices.size(); i++)
	{
		vertices[i].Position *= size;
	}
}

void Osm::GeneratePlane(
	float size,
	int resolution,
	vector<VertexFormat>& vertices,
	vector<ushort>& indices)
{
	if (resolution < 1)
		return;

	vertices.resize((resolution + 1) * (resolution + 1));
	indices.reserve(resolution * resolution * 6);

	float d = size / resolution;
	float y = -size * 0.5f;

	for(int i = 0; i <= resolution; i++)
	{
		float x = -size * 0.5f;
		for(int j = 0; j <= resolution; j++)
		{
			vertices[i * (resolution + 1) + j].Position = Vector3(x, 0.0f, y);
			vertices[i * (resolution + 1) + j].Normal = Vector3(0.0f, 1.0f, 0.0f);
			x += d;
		}
		y += d;
	}

	for (int i = 0; i < resolution; i++)
	{
		for (int j = 0; j < resolution; j++)
		{
			ushort idx = i * (resolution + 1) + j;

			indices.push_back(idx);
			indices.push_back(idx + 2 + resolution);
			indices.push_back(idx + 1);
						
			indices.push_back(idx);
			indices.push_back(idx + 1 + resolution);
			indices.push_back(idx + 2 + resolution);
		}
	}
}

