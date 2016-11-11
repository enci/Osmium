#include <Graphics/Mesh.h>
#include <flatbuffers/flatbuffers.h>
//#include <Graphics/MeshData/MeshData_generated.h>
#include <fstream>

using namespace std;
using namespace Osm;
using namespace flatbuffers;

int main(int argc, char *argv[])
{
	if (argc != 2)
		return 1;

	const char* cfilename = argv[1];
	string filename(cfilename);

	if (!StringEndsWith(filename, ".obj"))
		return 2;

	Mesh* m = new Mesh();
	m->Load(filename);

	const auto& vertices = m->GetVertices();
	const auto& indices = m->GetIndices();
	Mesh::BinaryMeshHeader header;
	header.IndexCount = indices.size();
	header.VertexCount = vertices.size();

	auto sizeVert = sizeof(decltype(vertices[0]));
	auto sizeIdx = sizeof(decltype(indices[0]));

	string outFilename = Osm::StringReplace(filename, ".obj", ".binarymesh");
	ofstream fout(outFilename, ios::out | ios::binary);
	fout.write((char*)&header, sizeof(Mesh::BinaryMeshHeader));
	fout.write((char*)&vertices[0], vertices.size() * sizeVert);
	fout.write((char*)&indices[0], indices.size() * sizeIdx);
	fout.close();	

    return 0;
}

