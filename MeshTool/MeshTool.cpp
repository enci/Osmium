#include <Graphics/Mesh.h>
#include <flatbuffers/flatbuffers.h>
#include <Graphics/MeshData/MeshData_generated.h>

//using namespace std;
using namespace Osm;
using namespace flatbuffers;

int main(int argc, char *argv[])
{
	if (argc != 2)
		return 1;

	const char* cfilename = argv[1];
	std::string filename(cfilename);

	Mesh* m = new Mesh();
	m->Load(filename);

	/*
	uint size = 0;
	size += sizeof(m->GetIndexCount());
	size += sizeof(m->GetVertices());
	size += sizeof(m->GetIndices());
	char* buffer
	*/

	flatbuffers::FlatBufferBuilder builder;

	auto vertices = m->GetVertices();
	auto indices = m->GetIndices();

	std::vector<Offset<Vertex>> flatVertices;
	for (size_t i = 0; i < vertices.size(); i++)
	{
		Vec3 pos = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 norm = Vec3(1.0f, 1.0f, 1.0f);
		Vec2 tex = Vec2(0.0f, 0.0f);

		Offset<Vertex> vert = CreateVertex(
			builder,
			&pos,
			&norm,
			&tex);
		flatVertices.push_back(vert);
	}

	std::vector<uint> flatInidices;

	auto mesh = CreateMeshData(
		builder,
		m->GetIndexCount(),
		builder.CreateVector(flatVertices),
		builder.CreateVector(indices));

	builder.Finish(mesh);

	auto size = builder.GetSize();	
	auto data = builder.GetBufferPointer();

	FILE* pFile;
	fopen_s(&pFile, "Planet.mesh", "wb");
	fwrite(data, 1, size, pFile);	
	fclose(pFile);

    return 0;
}

