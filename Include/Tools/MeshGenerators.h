#pragma once
#include <vector>
#include <Graphics/Mesh.h>

namespace Osm
{
	/// Generates an icosphere with radius size.
	/// @param resilution: Number of subdivisions
	/// @param vertices: Resulting vertex positions
	/// @param indices: Resulting indices for use with glDrawElements
	void GenerateIcosphere(
		float size,
		int resolution,
		std::vector<Osm::VertexFormat>& vertices,
		std::vector<ushort>& indices);

	void GeneratePlane(
		float size,
		int resolution,
		std::vector<Osm::VertexFormat>& vertices,
		std::vector<ushort>& indices);
}
