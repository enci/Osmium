#include <Graphics/Mesh.h>
#include <map>
#include <sstream>
#include <Defines.h>
#include <Utils.h>
#include <fstream>

using namespace std;
using namespace Osm;

namespace
{

uint64_t trihash(short k, short l, short m)
{
	ulong h = k;
	h <<= 20;
	h += l;
	h <<= 20;
	h += m;
	return h;
}

}

Mesh::Mesh(const std::string& filename)
	: Resource(RESOURCE_TYPE_MESH)
	, _indexCount(0)
	, _vbo{0, 0}
{
	_resourcePath = filename;
	bool success = Load(filename);
	if (!success)
	{
		ASSERT(false);
		LOG("Mesh::Mesh(%s) - Unable to load mesh.", filename.c_str());
			return;
	}
	Apply();
}

Mesh::Mesh()
	: Resource(RESOURCE_TYPE_MESH)
	, _indexCount(0)
	, _vbo{ 0, 0 }
{
	// Needs this for proper ID-ing
	static int count = 0;
	string path = "Mesh" + to_string(++count);
	_resourcePath = path;
}

Osm::Mesh::~Mesh()
{
	ClearGL();
}

bool Mesh::Load(const std::string& filename)
{
	if (StringEndsWith(filename, ".obj"))
		return LoadOBJ(filename);
	if (StringEndsWith(filename, ".binarymesh"))
		return LoadBINMESH(filename);
	return false;
}

bool Mesh::LoadOBJ(const string& filename)
{
	const std::string dataString = Osm::ReadFile(filename);

	if (dataString.empty())
	{
		LOG("Unable to open obj file or it is empty: %s\n", filename.c_str());
		return false;
	}

	std::stringstream is(dataString);


	char input[512];
	char line[512];

	float x, y, z, u, v;
	ushort k, l, m;

	vector<Vector3>         offsetVec;      // Position offsets
	vector<Vector2>         textureVec;     // Texture coordinates
	vector<Vector3>         normalVec;      // Normals
	vector<ushort>			indicesVec;     // Indices
														
	vector<VertexFormat>	vertexVec;      // Generated vertices

	ushort idx = 0;
	map<uint64_t, uint> indexForVertex;

	while (is >> input)
	{
		if (input[0] == '#')
		{
			// Comment, skip line
			is.getline(line, 255, '\n');
		}
		else if (strcmp(input, "v") == 0)
		{
			// Vertex offset
			is >> x >> y >> z;
			offsetVec.push_back(Vector3(x, y, z));
		}
		else if (strcmp(input, "vt") == 0)
		{
			// Texture coord
			is >> u >> v;
			textureVec.push_back(Vector2{ u, 1.0f - v });
		}
		else if (strcmp(input, "vn") == 0)
		{
			// Vertex normal
			is >> x >> y >> z;
			normalVec.push_back(Vector3(x, y, z));
		}
		else if (strcmp(input, "f") == 0)
		{
			// Face - only triangles will work!!!

			char slash;
			// Three vertices on a triangle
			for (int i = 0; i < 3; ++i)
			{
				// Read indices
				is >> k >> slash >> l >> slash >> m;
				k -= 1;
				l -= 1;
				m -= 1;

				// Generate unique hash
				uint64_t hash = trihash(k, l, m);

				// Check if this a new vertex
				auto itr = indexForVertex.find(hash);
				if (itr != indexForVertex.end())
				{
					// Vertex in set
					indicesVec.push_back(itr->second);
				}
				else
				{
					// New vertex
					VertexFormat vertex;
					vertex.Position = offsetVec[k];
					vertex.Normal = normalVec[m];
					vertex.Texture = textureVec[l];

					// Add it to the vector
					vertexVec.push_back(vertex);
					// Book-keep index by vertex hash
					indexForVertex[hash] = idx;
					// Add index to indices vector
					indicesVec.push_back(idx);
					idx++;
				}
			}
		}
	}

	_vertices = move(vertexVec);
	_indices = move(indicesVec);

	return true;
}

bool Mesh::LoadBINMESH(const std::string& filename)
{
	ifstream fin(filename, ios::binary | ios::in);

	if (!fin.is_open())
		return false;

	BinaryMeshHeader header;
	fin.read((char*)&header, sizeof(Mesh::BinaryMeshHeader));
	if (header.Version != 1)
		return false;
	
	_vertices.resize(header.VertexCount);
	_indices.resize(header.IndexCount);
	auto sizeVert = sizeof(decltype(_vertices[0]));
	auto sizeIdx = sizeof(decltype(_indices[0]));
	fin.read((char*)&_vertices[0], _vertices.size() * sizeVert);
	fin.read((char*)&_indices[0], _indices.size() * sizeIdx);

	return true;
}

void Mesh::SetVertices(vector<VertexFormat>&& vertices)
{
	_vertices = move(vertices);
}

void Mesh::SetIndices(vector<GLushort>&& indices)
{
	_indices = move(indices);
}

void Mesh::Apply()
{
	ClearGL();

	// Allocate two buffers
	glGenBuffers(2, _vbo);
	GL_GET_ERROR();

	// Array buffer contains the attribute data
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
	GL_GET_ERROR();

	// Copy into VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices[0]) * _vertices.size(), &_vertices[0], GL_DYNAMIC_DRAW);
	GL_GET_ERROR();
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind buffer
	GL_GET_ERROR();

	// Element array buffer contains the indices.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[1]);
	GL_GET_ERROR();

	// Copy into VBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * _indices.size(), &_indices[0], GL_DYNAMIC_DRAW);
	GL_GET_ERROR();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind buffer
	GL_GET_ERROR();

	_indexCount =  static_cast<uint>(_indices.size());
	_vertices.clear();
	_indices.clear();
}

/*
GLuint Mesh::GetVertexArray() const
{
	return _vao;
}
*/

const GLuint* Mesh::GetVertexBuffers() const
{
	return _vbo;
}

bool Mesh::HasVertexBuffers() const
{
	return _vbo[0] != 0 && _vbo[1] != 0;
}

uint Mesh::GetIndexCount() const
{
	return _indexCount;
}

void Mesh::ClearGL()
{
	if (HasVertexBuffers())
	{
		glDeleteBuffers(2, _vbo);
		GL_GET_ERROR();
	}
}
/*
void Mesh::Reload()
{

}
*/