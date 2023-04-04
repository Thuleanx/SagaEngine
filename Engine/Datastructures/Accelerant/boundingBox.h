#pragma once

#include <glm/glm.hpp>
#include <optional>
#include "Engine/Entity/entity.h"

namespace Saga {

    struct BVHTriangleData {
        glm::vec3 triangle[3];
        Entity entity;
    };

    struct BoundingBox {
        glm::vec3 bounds[2];

        glm::vec3 centroid();
        glm::vec3 size();
        float surfaceArea();
        float volume();
        bool inside(glm::vec3 point);

        bool collidesWithRay(glm::vec4 point, glm::vec4 dir);
        float findCollisionWithRay(glm::vec4 point, glm::vec4 dir);
        bool intersectWith(const BoundingBox &other);
        float lowerBoundSquareDistanceTo(const glm::vec3 point);

        std::optional<float> findCollisionWithEllipsoid(glm::vec3 point, glm::vec3 dir, glm::vec3 ellipsoid);

        static BoundingBox getExtremeBound();
        static BoundingBox mergeBox(const BoundingBox &a, const BoundingBox &b);
        static BoundingBox confineBox(const BoundingBox &a, const BoundingBox &b);
        static BoundingBox pointBox(const glm::vec3 &point);
    };

    struct BoxedShapeData {
        BoundingBox box;
        BVHTriangleData data;

        void computeBound();
    };

};
