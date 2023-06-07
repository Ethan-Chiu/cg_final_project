//
//  BVH.h
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/6/7.
//

#pragma once

#include <stack>
#include "Math.h"

namespace Ethane {

    namespace GpuModel {
        enum MaterialType
        {
            LightSource,
            Lambertian,
            Metal,
            Glass
        };

        struct Material
        {
            alignas(4) MaterialType type;
            alignas(16) glm::vec3 albedo;
        };
    
        struct Triangle
        {
            alignas(16) glm::vec3 v0;
            alignas(16) glm::vec3 v1;
            alignas(16) glm::vec3 v2;
            alignas(4) uint materialIndex;
        };
    
        struct BvhNode
        {
            alignas(16) glm::vec3 min;
            alignas(16) glm::vec3 max;
            alignas(4) int leftNodeIndex = -1;
            alignas(4) int rightNodeIndex = -1;
            alignas(4) int objectIndex = -1;
        };
    
        // Model of light used for importance sampling.
       struct Light
       {
           // Index in the array of triangles.
           alignas(4) uint triangleIndex;
           // Area of the triangle;
           alignas(4) float area;
       };
    }

    namespace BVH
    {
    
        // Utility structure to keep track of the initial triangle index in the triangles array while sorting.
        struct Object0
        {
           uint32_t index;
           GpuModel::Triangle t;
        };

        // Intermediate BvhNode structure needed for constructing Bvh.
        struct BvhNode0
        {
           AABB box;
           // index refers to the index in the final array of nodes. Used for sorting a flattened Bvh.
           int index = -1;
           int leftNodeIndex = -1;
           int rightNodeIndex = -1;
           std::vector<Object0> objects;

           GpuModel::BvhNode getGpuModel()
           {
               bool leaf = leftNodeIndex == -1 && rightNodeIndex == -1;

               GpuModel::BvhNode node;
               node.min = box.Min;
               node.max = box.Max;
               node.leftNodeIndex = leftNodeIndex;
               node.rightNodeIndex = rightNodeIndex;

               if (leaf)
               {
                   node.objectIndex = objects[0].index;
               }

               return node;
           }
        };

        const glm::vec3 eps(0.0001);
        inline AABB triangleBoundingBox(GpuModel::Triangle &t)
        {
            // Need to add eps to correctly construct an AABB for flat objects like planes.
            return {glm::min(glm::min(t.v0, t.v1), t.v2) - eps, glm::max(glm::max(t.v0, t.v1), t.v2) + eps};
        }
    
        inline bool boxCompare(GpuModel::Triangle &a, GpuModel::Triangle &b, int axis)
        {
            AABB boxA = triangleBoundingBox(a);
            AABB boxB = triangleBoundingBox(b);
            
            return boxA.Min[axis] < boxB.Min[axis];
        }

        inline bool boxXCompare(Object0 a, Object0 b)
        {
            return boxCompare(a.t, b.t, 0);
        }

        inline bool boxYCompare(Object0 a, Object0 b)
        {
            return boxCompare(a.t, b.t, 1);
        }

        inline bool boxZCompare(Object0 a, Object0 b)
        {
            return boxCompare(a.t, b.t, 2);
        }

        inline bool nodeCompare(BvhNode0 &a, BvhNode0 &b)
        {
            return a.index < b.index;
        }
    
        inline AABB objectListBoundingBox(std::vector<Object0> &objects)
        {
            AABB tempBox;
            AABB outputBox;
            bool firstBox = true;

            for (auto &object : objects)
            {
                tempBox = triangleBoundingBox(object.t);
                outputBox = firstBox ? tempBox : Math::SurroundingBox(outputBox, tempBox);
                firstBox = false;
            }

            return outputBox;
        }
    
        inline std::vector<GpuModel::BvhNode> CreateBvh(const std::vector<Object0> &srcObjects)
        {
            std::vector<BvhNode0> intermediate;
            int nodeCounter = 0;
            std::stack<BvhNode0> nodeStack;
            
            BvhNode0 root;
            root.index = nodeCounter;
            root.objects = srcObjects;
            nodeCounter++;
            nodeStack.push(root);
            
            while (!nodeStack.empty())
            {
                BvhNode0 currentNode = nodeStack.top();
                nodeStack.pop();
                
                currentNode.box = objectListBoundingBox(currentNode.objects);
                
                int axis = rand() % 3;
                auto comparator = (axis == 0)   ? boxXCompare
                : (axis == 1) ? boxYCompare
                : boxZCompare;
                
                size_t objectSpan = currentNode.objects.size();
                std::sort(currentNode.objects.begin(), currentNode.objects.end(), comparator);
                
                if (objectSpan <= 1)
                {
                    intermediate.push_back(currentNode);
                    continue;
                }
                else
                {
                    auto mid = objectSpan / 2;
                    
                    BvhNode0 leftNode;
                    leftNode.index = nodeCounter;
                    for (int i = 0; i < mid; i++)
                    {
                        leftNode.objects.push_back(currentNode.objects[i]);
                    }
                    nodeCounter++;
                    nodeStack.push(leftNode);
                    
                    BvhNode0 rightNode;
                    rightNode.index = nodeCounter;
                    for (int i = mid; i < objectSpan; i++)
                    {
                        rightNode.objects.push_back(currentNode.objects[i]);
                    }
                    nodeCounter++;
                    nodeStack.push(rightNode);
                    
                    currentNode.leftNodeIndex = leftNode.index;
                    currentNode.rightNodeIndex = rightNode.index;
                    intermediate.push_back(currentNode);
                }
            }
            std::sort(intermediate.begin(), intermediate.end(), nodeCompare);
            
            std::vector<GpuModel::BvhNode> output;
            output.reserve(intermediate.size());
            for (int i = 0; i < intermediate.size(); i++)
            {
                output.push_back(intermediate[i].getGpuModel());
            }
            return output;
        }
    }
}
