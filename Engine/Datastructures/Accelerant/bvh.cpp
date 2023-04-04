#include "bvh.h"
#include <algorithm>
#include "Engine/Datastructures/Accelerant/boundingBox.h"
#include "Engine/Utils/geometry.h"
using namespace glm;
using namespace std;

#define f(i,a,b) for (int i = a; i < b; i++)
#define oo 1e9

namespace Saga {

    void BVH::build(const vector<BVHTriangleData> &shapes) {
        allShapes.clear();
        root = std::make_shared<BVHNode>();

        vector<BoxedShapeData*> shapesWithBounds;
        root->box = BoundingBox::getExtremeBound();

        allShapes.reserve( shapes.size() );

        for (const BVHTriangleData &shape : shapes) {

            BoxedShapeData bshape;
            bshape.data = shape;
            bshape.computeBound();

            root->box = BoundingBox::mergeBox(root->box, bshape.box);
            allShapes.push_back(bshape);
        }

        shapesWithBounds.reserve( allShapes.size() );
        for (int i = 0; i < allShapes.size(); i++)
            shapesWithBounds.push_back(&allShapes[i]);

        if (shapesWithBounds.size()) build(root, shapesWithBounds);
    }

    // simple BVH split by centroid, pretend all shapes are centroids
    void BVH::build(std::shared_ptr<BVHNode> node, std::vector<BoxedShapeData*> &shapes) {
        if (shapes.size() < 2) {
            node->shapes = shapes;
            return;
        }

        // decide split position by longest dimension of centroid bounding box
        BoundingBox centroidBounds = BoundingBox::getExtremeBound();
        for (BoxedShapeData* bshape : shapes)
            centroidBounds = BoundingBox::mergeBox(centroidBounds, BoundingBox::pointBox(bshape->box.centroid()));
        int dim = 0;
        f(i,1,3) if (centroidBounds.bounds[1][i] - centroidBounds.bounds[0][i] 
                > centroidBounds.bounds[1][dim] - centroidBounds.bounds[0][dim]) dim = i;

        // sort along dim, breaking ties arbitrarily
        sort(begin(shapes),end(shapes),
            [&](BoxedShapeData* a, BoxedShapeData* b) {
            return a->box.centroid()[dim] < b->box.centroid()[dim];
        });

        int lsz = shapes.size()/2;

        std::vector<BoxedShapeData*> leftShapes(lsz), rightShapes(shapes.size() - lsz);

        copy(begin(shapes), begin(shapes) + lsz, begin(leftShapes));
        copy(begin(shapes) + lsz, end(shapes), begin(rightShapes));

        std::shared_ptr<BVHNode> left = std::make_shared<BVHNode>();
        std::shared_ptr<BVHNode> right = std::make_shared<BVHNode>();

        left->box = BoundingBox::getExtremeBound();
        right->box = BoundingBox::getExtremeBound();

        for (const BoxedShapeData* bshape : leftShapes)
            left->box = BoundingBox::mergeBox(bshape->box, left->box);
        for (const BoxedShapeData* bshape : rightShapes)
            right->box = BoundingBox::mergeBox(bshape->box, right->box);

        node->children.push_back(left);
        node->children.push_back(right);

        build(left, leftShapes);
        build(right, rightShapes);
    }

    std::optional<BVH::BVHTracedData> BVH::traceEllipsoid(glm::vec3 pos, glm::vec3 dir, glm::vec3 scale) {
        std::optional<BVH::BVHTracedData> result = {};
        if (root) traceEllipsoid(pos, dir, scale, root, result);
        return result;
    }

    void BVH::traceEllipsoid(glm::vec3 pos, glm::vec3 dir, glm::vec3 scale, std::shared_ptr<BVHNode> node, std::optional<BVH::BVHTracedData> &result) {
        // this should only run for terminal nodes
        // we loop through all shapes and see if any intersects with our ellipsoid
        for (BoxedShapeData* brsd : node->shapes) {
            float tc = Saga::Geometry::ellipsoidTriangleCollision(pos, dir, scale, 
                brsd->data.triangle[0], brsd->data.triangle[1], brsd->data.triangle[2]);

            if (tc >= 0 && (!result.has_value() || tc < std::get<1>(result.value()))) 
                result = {&brsd->data, tc};
        }

        // recurse through children
        for (std::shared_ptr<BVHNode> child : node->children) {
            std::optional<float> tc = child->box.findCollisionWithEllipsoid(pos, dir, scale);
            if (tc.has_value() && (!result.has_value() || tc.value() < std::get<1>(result.value()))) 
                traceEllipsoid(pos, dir, scale, child, result);
        }
    }

}
