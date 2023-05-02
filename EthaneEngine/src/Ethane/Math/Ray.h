#pragma once

#include <glm/glm.hpp>

namespace Ethane {

    struct Ray {
        glm::vec3 Origin;
        glm::vec3 Dir;
        glm::vec3 InvDir;
        int sign[3];

        Ray() {}
        Ray(const glm::vec3& origin, const glm::vec3& direction)
            : Origin(origin), Dir(direction)
        {
            InvDir.x = 1.0 / Dir.x;
            InvDir.y = 1.0 / Dir.y;
            InvDir.z = 1.0 / Dir.z;
            sign[0] = (InvDir.x < 0);
            sign[1] = (InvDir.y < 0);
            sign[2] = (InvDir.z < 0);
        }

        glm::vec3 at(float t) const {
            return Origin + t * Dir;
        }

        template<typename OStream>
        friend OStream& operator<<(OStream& os, const Ray& c)
        {
            os << "[origin; (" << c.Origin.x << ", " << c.Origin.y << ", " << c.Origin.z << "), ";
            os << "dir: (" << c.Dir.x << ", " << c.Dir.y << ", " << c.Dir.z << ")]" << std::endl;
            
            return os;
        }

        bool IntersectsTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float& t) const
        {
            const glm::vec3 E1 = b - a;
            const glm::vec3 E2 = c - a;
            const glm::vec3 N = cross(E1, E2);
            const float NdotDir = -glm::dot(Dir, N);
            if (NdotDir >= -1e-6f && NdotDir <= 1e-6f)
                return false;
            const float invNdotDir = 1.f / NdotDir;
            const glm::vec3 AO = Origin - a;
            t = glm::dot(AO, N) * invNdotDir;
            if (t < 0)
                return false;
            const glm::vec3 DAO = glm::cross(AO, Dir);
            const float u = glm::dot(E2, DAO) * invNdotDir;
            const float v = -glm::dot(E1, DAO) * invNdotDir;
            return (u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f);
        }
    };
       
}
