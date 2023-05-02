#pragma once

#include "Ray.h"
#include <glm/glm.hpp>

namespace Ethane {

	struct AABB
	{
        union
        {
            struct
            {
                glm::vec3 Min;
                glm::vec3 Max;
            };
            struct
            {
                glm::vec3 Bound[2];
            };
        };
		

		AABB()
			: Min(0.0f), Max(0.0f) {}

		AABB(const glm::vec3& min, const glm::vec3& max)
			: Min(min), Max(max) {}

        bool intersect(const Ray& r, float& t)
        {
            float tmin, tmax, tymin, tymax, tzmin, tzmax;

            tmin  = (Bound[r.sign[0]].x     - r.Origin.x) * r.InvDir.x;
            tmax  = (Bound[1 - r.sign[0]].x - r.Origin.x) * r.InvDir.x;
            tymin = (Bound[r.sign[1]].y     - r.Origin.y) * r.InvDir.y;
            tymax = (Bound[1 - r.sign[1]].y - r.Origin.y) * r.InvDir.y;

            if ((tmin > tymax) || (tymin > tmax))
                return false;
            if (tymin > tmin)
                tmin = tymin;
            if (tymax < tmax)
                tmax = tymax;

            tzmin = (Bound[r.sign[2]].z     - r.Origin.z) * r.InvDir.z;
            tzmax = (Bound[1 - r.sign[2]].z - r.Origin.z) * r.InvDir.z;

            if ((tmin > tzmax) || (tzmin > tmax))
                return false;
            if (tzmin > tmin)
                tmin = tzmin;
            if (tzmax < tmax)
                tmax = tzmax;

            t = tmin;

            if (t < 0) {
                t = tmax;
                if (t < 0) return false;
            }

            return true;

        }
    };

}