#include "ethpch.h"
#include "AssetManager.h"

#include "ShaderLibrary.h"

namespace Ethane {

	std::unordered_map<std::string, Ref<Mesh>> AssetManager::s_LoadedMeshes;

	void AssetManager::Init()
	{
		ShaderLibrary::Init();
	}

	void AssetManager::Shutdown()
	{
		ShaderLibrary::Shutdown();
	}

	Ref<Mesh> AssetManager::GetAssetMesh(const std::filesystem::path& filepath)
	{
		std::string fp = filepath.string();
		if (s_LoadedMeshes[fp] == nullptr)
		{
			s_LoadedMeshes[fp] = CreateRef<Mesh>(fp);
		}
		return s_LoadedMeshes[fp];
	}
}