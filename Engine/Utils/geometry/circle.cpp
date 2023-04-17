#include "circle.h"
#include <algorithm>
#include <glm/geometric.hpp>
#include "Engine/Utils/math.h"

namespace Saga::Geometry {

    glm::vec2 detectCircleCollision(
            const glm::vec2& aPos, const float &aRadius, 
            const glm::vec2& bPos, const float &bRadius) {

        glm::vec2 dis = (aPos - bPos);
        float len2 = glm::dot(dis, dis);
        // no collision if the distance^2 is larger than sum of radius squared
        if (len2 > (aRadius + bRadius) * (aRadius + bRadius)) 
            return glm::vec2(0,0);
        if (len2) {
            float len = sqrt(len2);
            return ((aRadius + bRadius) / len - 1.0f) * dis;
        } else return glm::vec2(aRadius + bRadius, 0); // if the objects has the same center, then mtv is to the right.
    }

    std::optional<std::tuple<float,float>> rayUnitCircleAtOriginIntersection(const glm::vec2& origin, const glm::vec2& rayDirection) {
        float a = glm::dot(rayDirection, rayDirection);
        float b = 2 * glm::dot(rayDirection, origin);
        float c = glm::dot(origin, origin) - 1;

        std::vector<float> sols = Math::solveQuadraticReals(a,b,c);
        // this ensures that the resulting tlo and thi are ordered
        if (sols.size() == 2 && sols[0] > sols[1]) 
            std::swap(sols[0], sols[1]);
        // no collision
        if (!sols.size()) 
            return {};
        if (sols.size() == 1)
            return std::make_tuple(sols[0], sols[0]);
        return std::make_tuple(sols[0], sols[1]);
    }
}
