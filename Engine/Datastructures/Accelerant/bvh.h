#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <set>
#include "boundingBox.h"
#include <optional>
#include <memory>

namespace Saga {

    struct BVHNode {
        BoundingBox box;
        std::vector<BoxedShapeData*> shapes;
        std::vector<std::shared_ptr<BVHNode>> children;
    };


    class BVH {
        using BVHTracedData = std::tuple<BVHTriangleData*, float>;
    public:
        void build(const std::vector<BVHTriangleData> &shapes);
        void build(std::shared_ptr<BVHNode> node, std::vector<BoxedShapeData*> &shapes);

        std::optional<BVHTracedData> traceEllipsoid(glm::vec3 pos, glm::vec3 dir, glm::vec3 scale);

    private:
        std::shared_ptr<BVHNode> root;
        std::vector<BoxedShapeData> allShapes;
        void traceEllipsoid(glm::vec3 pos, glm::vec3 dir, glm::vec3 scale, std::shared_ptr<BVHNode> node, std::optional<BVHTracedData> &result);
    };


};
