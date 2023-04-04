#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <set>
#include "boundingBox.h"
#include <optional>

namespace Saga {

    struct BVHNode {
        BoundingBox box;
        std::vector<BoxedShapeData*> shapes;
        std::vector<BVHNode*> children;

        BVHNode();
        ~BVHNode();
    };


    class BVH {
        using BVHTracedData = std::tuple<BVHTriangleData*, float>;
    public:
        BVH();
        ~BVH();

        void build(const std::vector<BVHTriangleData> &shapes);
        void build(BVHNode* node, std::vector<BoxedShapeData*> &shapes);
        void reset();

        std::optional<BVHTracedData> traceEllipsoid(glm::vec3 pos, glm::vec3 dir, glm::vec3 scale);

    private:
        BVHNode* root;
        std::vector<BoxedShapeData> allShapes;
        void traceEllipsoid(glm::vec3 pos, glm::vec3 dir, glm::vec3 scale, BVHNode* node, std::optional<BVHTracedData> &result);
    };


};
