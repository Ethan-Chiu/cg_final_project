#include "ethpch.h"
#include "Mesh.h"

#include "Renderer.h"
#include "Pipeline.h"

namespace Ethane {

	glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix)
	{
		glm::mat4 result;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
		result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
		result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
		result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;
		return result;
	}

	static const uint32_t s_MeshImportFlags =
		aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
		aiProcess_Triangulate |             // Make sure we're triangles
		aiProcess_SortByPType |             // Split meshes by primitive type
		aiProcess_GenNormals |              // Make sure we have legit normals
		aiProcess_GenUVCoords |             // Convert UVs if required 
		aiProcess_OptimizeMeshes |          // Batch draws where possible
		aiProcess_JoinIdenticalVertices |
		aiProcess_ValidateDataStructure;    // Validation

	Mesh::Mesh(const std::string& filename)
		: m_FilePath(filename)
	{

		ETH_CORE_INFO("Loading mesh: {0}", filename.c_str());

		m_Importer = std::make_unique<Assimp::Importer>();

		const aiScene* scene = m_Importer->ReadFile(filename, s_MeshImportFlags);
		if (!scene || !scene->HasMeshes())
		{
			ETH_CORE_ERROR("Failed to load mesh file: {0}", filename);
			ETH_CORE_ASSERT(false);
			return;
		}

		m_Scene = scene;

		// m_MeshShader = Shader::Create("assets/shaders/PBR_static.glsl");
//		m_MeshShader = ShaderLibrary::Get("PBR_static");
		m_InverseTransform = glm::inverse(Mat4FromAssimpMat4(scene->mRootNode->mTransformation));

		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		m_Submeshes.reserve(scene->mNumMeshes);
		for (unsigned m = 0; m < scene->mNumMeshes; m++)
		{
			aiMesh* mesh = scene->mMeshes[m];

			Submesh& submesh = m_Submeshes.emplace_back();
			submesh.BaseVertex = vertexCount;
			submesh.BaseIndex = indexCount;
			submesh.MaterialIndex = mesh->mMaterialIndex;
			submesh.VertexCount = mesh->mNumVertices;
			submesh.IndexCount = mesh->mNumFaces * 3;
			submesh.MeshName = mesh->mName.C_Str();

			vertexCount += mesh->mNumVertices;
			indexCount += submesh.IndexCount;

			ETH_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
			ETH_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

			// Vertices
			{
				for (size_t i = 0; i < mesh->mNumVertices; i++)
				{
					Vertex vertex;
					vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
					vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
					// ETH_CORE_INFO("{0} {1} {2}", mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
					
					// AABB
					auto& min = submesh.Aabb.Min;
					auto& max = submesh.Aabb.Max;
					min.x = glm::min(vertex.Position.x, min.x);
					min.y = glm::min(vertex.Position.y, min.y);
					min.z = glm::min(vertex.Position.z, min.z);
					max.x = glm::max(vertex.Position.x, max.x);
					max.y = glm::max(vertex.Position.y, max.y);
					max.z = glm::max(vertex.Position.z, max.z);

					if (mesh->HasTangentsAndBitangents())
					{
						vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
						vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
					}

					if (mesh->HasTextureCoords(0))
						vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

					m_StaticVertices.push_back(vertex);
				}
			}

			// Indices
			for (size_t i = 0; i < mesh->mNumFaces; i++)
			{
				ETH_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
				Index index = { mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] };
				m_Indices.push_back(index);

				m_TriangleCache[m].emplace_back(m_StaticVertices[index.V1 + submesh.BaseVertex], m_StaticVertices[index.V2 + submesh.BaseVertex], m_StaticVertices[index.V3 + submesh.BaseVertex]);
			}
		}

		// Set submesh transform and name
		TraverseNodes(scene->mRootNode);

		for (const auto& submesh : m_Submeshes)
		{
			AABB SubmeshAABB = submesh.Aabb;
			glm::vec3 min = glm::vec3(submesh.Transform * glm::vec4(SubmeshAABB.Min, 1.0f));
			glm::vec3 max = glm::vec3(submesh.Transform * glm::vec4(SubmeshAABB.Max, 1.0f));

			m_BoundingBox.Min.x = glm::min(m_BoundingBox.Min.x, min.x);
			m_BoundingBox.Min.y = glm::min(m_BoundingBox.Min.y, min.y);
			m_BoundingBox.Min.z = glm::min(m_BoundingBox.Min.z, min.z);
			m_BoundingBox.Max.x = glm::max(m_BoundingBox.Max.x, max.x);
			m_BoundingBox.Max.y = glm::max(m_BoundingBox.Max.y, max.y);
			m_BoundingBox.Max.z = glm::max(m_BoundingBox.Max.z, max.z);
		}
		
		// material
		// {
		// 	auto mi = Material::Create(m_MeshShader, "Hazel-Default");
		// 	mi->Set("u_MaterialUniforms.AlbedoColor", glm::vec3(0.8f));
		// 	mi->Set("u_MaterialUniforms.Metalness", 0.0f);
		// 	mi->Set("u_MaterialUniforms.Roughness", 0.8f);
		// 	mi->Set("u_MaterialUniforms.UseNormalMap", false);
		// 
		// 	mi->Set("u_AlbedoTexture", whiteTexture);
		// 	mi->Set("u_MetalnessTexture", whiteTexture);
		// 	mi->Set("u_RoughnessTexture", whiteTexture);
		// 	m_Materials.push_back(mi);
		// }
	}

    void Mesh::Upload()
    {
        {
            uint32_t size = (uint32_t)(m_StaticVertices.size() * sizeof(Vertex));
            m_VertexBuffer = VertexBuffer::Create(m_StaticVertices.data(), size);
        }

        m_IndexBuffer = IndexBuffer::Create((uint32_t*) m_Indices.data(), (uint32_t)(m_Indices.size() * 3 * sizeof(uint32_t))); // TODO: temp test change Decide to use byte or 4byte as unit
    }

    void Mesh::Unload()
    {
        m_IndexBuffer->Destroy();
        m_VertexBuffer->Destroy();
    }


	void Mesh::TraverseNodes(aiNode* node, const glm::mat4& parentTransform, uint32_t level)
	{
		glm::mat4 transform = parentTransform * Mat4FromAssimpMat4(node->mTransformation);
		m_NodeMap[node].resize(node->mNumMeshes);
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			uint32_t mesh = node->mMeshes[i];
			auto& submesh = m_Submeshes[mesh];
			submesh.NodeName = node->mName.C_Str();
			submesh.Transform = transform;
			m_NodeMap[node][i] = mesh;
		}

		// HZ_MESH_LOG("{0} {1}", LevelToSpaces(level), node->mName.C_Str());

		for (uint32_t i = 0; i < node->mNumChildren; i++)
			TraverseNodes(node->mChildren[i], transform, level + 1);
	}
}
