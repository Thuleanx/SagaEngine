#include "boundingBox.h"
#include "Engine/Utils/geometry/geometry.h"
#include <cstdio>
using namespace glm;
using namespace std;

namespace Saga {
    namespace {
        inline float square(float x) { return x*x; }
    }

    const float BoundingBox::oo = 1e9f;

    vec3 BoundingBox::centroid() {
        return (bounds[1] + bounds[0]) / 2.0f;
    }

    vec3 BoundingBox::size() {
        return bounds[1] - bounds[0];
    }

    bool BoundingBox::inside(vec3 point) {
        for (int dimension = 0; dimension < 3; dimension++)
            if (point[dimension] > bounds[1][dimension] || point[dimension] < bounds[0][dimension]) 
                return false;
        return true;
    }

    float BoundingBox::volume(){
        float volume = 1;
        for (int dimension = 0; dimension < 3; dimension++) {
            // we need this check because the bounding volume might not be valid 
            // in the sense that bounds[0] > bounds[1]
            // in this case we want volume to be 0.
            if (bounds[1][dimension] < bounds[0][dimension]) 
                return 0;
            volume *= bounds[1][dimension] - bounds[0][dimension];
        }
        return volume;
    }

    float BoundingBox::lowerBoundSquareDistanceTo(vec3 point) {
        float squaredDist = 0;
        for (int dimension = 0; dimension < 3; dimension++) {
            if (bounds[0][dimension] <= point[dimension] && point[dimension] <= bounds[1][dimension])
                continue;
            squaredDist += std::min(square(point[dimension] - bounds[0][dimension]),
                square(point[dimension] - bounds[1][dimension]));
        }
        return squaredDist;
    }

    float BoundingBox::surfaceArea() {
        float surfaceArea = 0;
        for (int i = 0; i < 3; i++) {
            // if the Bounding Volume is ill-formed, we say that the surface area is 0
            if (bounds[0][i] > bounds[1][i]) return 0;
            for (int j = i+1; j < 3; j++)
                surfaceArea += 2 * (bounds[1][i] - bounds[0][i]) * (bounds[1][j] - bounds[0][j]);
        }
        return surfaceArea;
    }

    bool BoundingBox::collidesWithRay(vec4 point, vec4 direction) {
        return findCollisionWithRay(point,direction) != -1;
    }

    std::optional<float> BoundingBox::findCollisionWithRay(vec4 point, vec4 direction) {
        return Saga::Geometry::rayBoxCollision(point, direction, bounds[0], bounds[1]);
    }

    std::optional<float> BoundingBox::findCollisionWithBox(glm::vec3 centroid, glm::vec3 dir, glm::vec3 size) {
        return Saga::Geometry::rayBoxCollision(centroid, dir, bounds[0] - size/2.0f, bounds[1] + size/2.0f);
    }

    bool BoundingBox::intersectWith(const BoundingBox &other) {
        for (int dimension = 0; dimension < 3; dimension++)
            if (bounds[0][dimension] > other.bounds[1][dimension] || other.bounds[0][dimension] > bounds[1][dimension]) 
                return false;
        return true;
    }

    BoundingBox BoundingBox::mergeBox(const BoundingBox &a, const BoundingBox &b) {
        return BoundingBox{
            vec3(
                std::min(a.bounds[0].x, b.bounds[0].x),
                std::min(a.bounds[0].y, b.bounds[0].y),
                std::min(a.bounds[0].z, b.bounds[0].z)
            ), vec3(
                std::max(a.bounds[1].x, b.bounds[1].x),
                std::max(a.bounds[1].y, b.bounds[1].y),
                std::max(a.bounds[1].z, b.bounds[1].z)
            )
        };
    }

    BoundingBox BoundingBox::confineBox(const BoundingBox &a, const BoundingBox &b) {
        return BoundingBox{
            vec3(
                std::max(a.bounds[0].x, b.bounds[0].x),
                std::max(a.bounds[0].y, b.bounds[0].y),
                std::max(a.bounds[0].z, b.bounds[0].z)
            ), vec3(
                std::min(a.bounds[1].x, b.bounds[1].x),
                std::min(a.bounds[1].y, b.bounds[1].y),
                std::min(a.bounds[1].z, b.bounds[1].z)
            )
        };
    }

    BoundingBox BoundingBox::pointBox(const vec3 &point) {
        return BoundingBox {
            vec3(point), vec3(point)
        };
    }

    BoundingBox BoundingBox::getExtremeBound() {
        return BoundingBox{
            vec3(oo,oo,oo),
                vec3(-oo,-oo,-oo)
        };
    }
};
