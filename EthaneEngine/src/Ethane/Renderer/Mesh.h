#pragma once
#include <iostream>

#include "Pipeline.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Shader.h"

#include "Ethane/Math/AABB.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace Assimp {
	class Importer;
}

namespace Ethane {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Binormal;
		glm::vec2 Texcoord;
	};

	struct Index
	{
		uint32_t V1, V2, V3;
	};

	struct Triangle
	{
		Vertex V0, V1, V2;

		Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
			: V0(v0), V1(v1), V2(v2) {}
	};

	class Submesh
	{
	public:
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t IndexCount;
		uint32_t VertexCount;

		glm::mat4 Transform{ 1.0f };
		AABB Aabb;

		std::string NodeName, MeshName;
	};

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const std::string& filename);
		~Mesh() = default;

        void Upload();
        
		// Getter
		std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
		const std::vector<Triangle> GetTrianglesCacheInSubmesh(uint32_t index) const { return m_TriangleCache.at(index); }
		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		uint64_t GetVertexNum() const { return m_StaticVertices.size(); };
		uint64_t GetIndexNum() const { return m_Indices.size();}
	private:
		void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);
	private:
		std::string m_FilePath;
		std::unique_ptr<Assimp::Importer> m_Importer;

		const aiScene* m_Scene;
		glm::mat4 m_InverseTransform;

		std::vector<Submesh> m_Submeshes;
		std::unordered_map<uint32_t, std::vector<Triangle>> m_TriangleCache;

//		Ref<Shader> m_MeshShader;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		std::vector<Vertex> m_StaticVertices;
		std::vector<Index> m_Indices;

		AABB m_BoundingBox;

		std::unordered_map<aiNode*, std::vector<uint32_t>> m_NodeMap;

	friend class Renderer;
	friend class VulkanRendererAPI;
	};

}
