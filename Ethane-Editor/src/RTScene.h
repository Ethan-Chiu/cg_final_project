//
//  RTScene.h
//  Ethane-Editor
//
//  Created by 邱奕翔 on 2023/6/7.
//

#pragma once

#include "Ethane.h"

namespace Ethane {

    struct MeshRenderData
    {
        Ref<Mesh> MeshRef;
        glm::mat4 Transform;
        uint MatIndex;
    };

    struct RTScene
    {
        std::vector<GpuModel::Material> Materials;
        std::vector<GpuModel::Triangle> Triangles;
        std::vector<GpuModel::Light> Lights;
        std::vector<GpuModel::BvhNode> BvhNodes;

        void Create(const std::vector<MeshRenderData>& meshes);


    private:
        std::vector<GpuModel::Triangle> GetTriangles(Ref<Mesh> mesh, glm::mat4 transform, uint materialIndex);
    };

}
