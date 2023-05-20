#pragma once

#include "Ethane/Renderer/Mesh.h"

namespace Ethane {

	class AssetManager 
	{
	public:
		static void Init();
		static void Shutdown();

		static Ref<Mesh> GetAssetMesh(const std::filesystem::path& filepath);

	private:
		static std::unordered_map<std::string, Ref<Mesh>> s_LoadedMeshes; // TODO: temp
	};

}