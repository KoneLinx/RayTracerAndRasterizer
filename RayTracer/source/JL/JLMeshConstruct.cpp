#include "JLMeshConstruct.h"
#include <fstream>

namespace JL
{

	Mesh_t::MeshData::Vertex& AsMeshVertex(OBJ::Vertex& vertex) noexcept
	{
		static_assert(sizeof(Mesh_t::MeshData::Vertex) == sizeof(OBJ::Vertex));
		return reinterpret_cast<Mesh_t::MeshData::Vertex&>(vertex);
	}

	Mesh_t::MeshData::Container<Mesh_t::MeshData::Vertex>& AsMeshVertexVector(decltype(OBJ::vertices)& vertex) noexcept
	{
		static_assert(sizeof(Mesh_t::MeshData::Container<Mesh_t::MeshData::Vertex>) == sizeof(decltype(OBJ::vertices)));
		return reinterpret_cast<Mesh_t::MeshData::Container<Mesh_t::MeshData::Vertex>&>(vertex);
	}

	Mesh_t::MeshData::Triangle& AsMeshTriangle(OBJ::Face& face) noexcept
	{
		static_assert(sizeof(Mesh_t::MeshData::Triangle) == sizeof(OBJ::Face));
		return reinterpret_cast<Mesh_t::MeshData::Triangle&>(face);
	}

	Mesh_t::MeshData::Container<Mesh_t::MeshData::Triangle>& AsMeshTriangleVector(decltype(OBJ::faces)& vertex) noexcept
	{
		static_assert(sizeof(Mesh_t::MeshData::Container<Mesh_t::MeshData::Triangle>) == sizeof(decltype(OBJ::faces)));
		return reinterpret_cast<Mesh_t::MeshData::Container<Mesh_t::MeshData::Triangle>&>(vertex);
	}

	void LoadMesh(Mesh_t& mesh, std::string_view filePath)
	{

		//This only works when the OBJ and Mesh have the same point structure. Which they do in my case.

		std::ifstream file{ filePath.data(), std::ios::in };

		OBJ objData;
		file >> objData;

		mesh.AsData().vertices = std::move(AsMeshVertexVector(objData.vertices));
		mesh.AsData().triangles = std::move(AsMeshTriangleVector(objData.faces));

		mesh.ResetCenter();

	}

}