//
//  RTScene.h
//  Ethane-Editor
//
//  Created by 邱奕翔 on 2023/6/7.
//

#pragma once

#include "Ethane.h"
#include "RTScene.h"

namespace Ethane {

    void RTScene::Create(const std::vector<MeshRenderData>& meshes)
    {
        GpuModel::Material gray{GpuModel::MaterialType::Lambertian, glm::vec3(0.3f, 0.3f, 0.3f)};
        GpuModel::Material red{GpuModel::MaterialType::Lambertian, glm::vec3(0.9f, 0.1f, 0.1f)};
        GpuModel::Material green{GpuModel::MaterialType::Lambertian, glm::vec3(0.1f, 0.9f, 0.1f)};
        GpuModel::Material whiteLight{GpuModel::MaterialType::LightSource, glm::vec3(2.0f, 2.0f, 2.0f)};
        GpuModel::Material metal{GpuModel::MaterialType::Metal, glm::vec3(1.0f, 1.0f, 1.0f)};
        GpuModel::Material glass{GpuModel::MaterialType::Glass, glm::vec3(1.0f, 1.0f, 1.0f)};
        Materials.push_back(gray);
        Materials.push_back(red);
        Materials.push_back(green);
        Materials.push_back(whiteLight);
        Materials.push_back(metal);
        Materials.push_back(glass);

        // add triangles
        for (auto& mesh : meshes)
        {
            std::vector<GpuModel::Triangle> meshTriangles = GetTriangles(mesh.MeshRef, mesh.Transform, mesh.MatIndex);
            Triangles.insert(Triangles.end(), meshTriangles.begin(), meshTriangles.end());
        }

        std::vector<BVH::Object0> objects;
        for (uint32_t i = 0; i < Triangles.size(); i++)
        {
            GpuModel::Triangle t = Triangles[i];
            objects.push_back({i, t});
            if (Materials[t.materialIndex].type == GpuModel::MaterialType::LightSource)
            {
                float area = glm::length(glm::cross(t.v0, t.v1)) * 0.5f;
                Lights.push_back({i, area});
            }
        }
        BvhNodes = BVH::CreateBvh(objects);
    }



    std::vector<GpuModel::Triangle> RTScene::GetTriangles(Ref<Mesh> mesh, glm::mat4 transform, uint materialIndex)
    {
        std::vector<GpuModel::Triangle> triangles;
        auto& meshTriangles = mesh->GetTriangles();

        glm::vec4 v0, v1, v2;
        for(auto&& [m, vecTri] : meshTriangles)
        {
            for(auto&& triangle : vecTri)
            {
                v0 = transform * glm::vec4(triangle.V0.Position, 1);
                v1 = transform * glm::vec4(triangle.V1.Position, 1);
                v2 = transform * glm::vec4(triangle.V2.Position, 1);
                GpuModel::Triangle t{glm::vec3(v0), glm::vec3(v1), glm::vec3(v2), materialIndex};
                triangles.push_back(t);
            }
        }

        return triangles;
    }

}
