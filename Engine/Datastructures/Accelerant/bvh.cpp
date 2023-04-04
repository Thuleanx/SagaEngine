#include "bvh.h"
#include <algorithm>
#include "Engine/Datastructures/Accelerant/boundingBox.h"
#include "Engine/Utils/geometry.h"
using namespace glm;
using namespace std;

#define oo 1e9

namespace Saga {

    void BoundingVolumeHierarchy::build(const vector<TriangleData> &shapes) {
        allShapes.clear();
        root = std::make_shared<Node>();

        vector<BoundedShapeData*> shapesWithBounds;
        root->box = BoundingBox::getExtremeBound();

        // we reserve space in allShapes beforehand so allocation doesn't happen 
        // and slow us down
        allShapes.reserve( shapes.size() );

        for (const TriangleData &shape : shapes) {
            BoundedShapeData bshape;
            bshape.data = shape;
            // remember to compute bound, else the bound on the struct
            // won't be accurate
            bshape.computeBound();

            // since we need the root's bounding box to be constructed, we 
            // shall do so by computing union with all the bounding boxes of 
            // the triangles
            root->box = BoundingBox::mergeBox(root->box, bshape.box);
            allShapes.push_back(bshape);
        }

        // again reserve space so allocation does not happen
        // we construct a temporary vector containing pointers to all
        // BoundedShapeData to pass in to the build function.
        // the reason why we use pointers is we dont't want destructors and constructors
        // to run in the process, and we will have to split this vector into smaller ones.
        shapesWithBounds.reserve( allShapes.size() );
        for (int i = 0; i < allShapes.size(); i++)
            shapesWithBounds.push_back(&allShapes[i]);

        if (shapesWithBounds.size()) build(root, shapesWithBounds);
    }

    void BoundingVolumeHierarchy::build(std::shared_ptr<Node> node, std::vector<BoundedShapeData*> &shapes) {
        if (shapes.size() <= NUM_TRIANGLES_PER_LEAF) {
            node->shapes = shapes;
            return;
        }

        // decide split position by longest dimension of centroid bounding box
        BoundingBox centroidBounds = BoundingBox::getExtremeBound();

        // first find the bounding box of all the centroids
        for (BoundedShapeData* bshape : shapes)
            centroidBounds = BoundingBox::mergeBox(centroidBounds, BoundingBox::pointBox(bshape->box.centroid()));

        // find longest dimension
        int longestDim = 0;
        for (int dim = 0; dim < 3; dim++) {
            float longestDimSize = centroidBounds.bounds[1][longestDim] - centroidBounds.bounds[0][longestDim];
            float dimSize = centroidBounds.bounds[1][dim] - centroidBounds.bounds[0][dim];
            if (dimSize > longestDimSize) 
                longestDim = dim;
        }

        // sort along longest centroid dimension, breaking ties arbitrarily
        sort(begin(shapes),end(shapes),
            [&](BoundedShapeData* a, BoundedShapeData* b) {
            return a->box.centroid()[longestDim] < b->box.centroid()[longestDim];
        });

        int leftSplitSize = shapes.size()/2;
        std::vector<BoundedShapeData*> leftShapes(leftSplitSize), rightShapes(shapes.size() - leftSplitSize);

        copy(begin(shapes), begin(shapes) + leftSplitSize, begin(leftShapes));
        copy(begin(shapes) + leftSplitSize, end(shapes), begin(rightShapes));

        std::shared_ptr<Node> left = std::make_shared<Node>();
        std::shared_ptr<Node> right = std::make_shared<Node>();

        // we compute the BoundingBox for each child.
        // TODO: speed up this computation since the data is ordered and we don't need to run as many max operations
        left->box = BoundingBox::getExtremeBound();
        right->box = BoundingBox::getExtremeBound();

        for (const BoundedShapeData* bshape : leftShapes)
            left->box = BoundingBox::mergeBox(bshape->box, left->box);
        for (const BoundedShapeData* bshape : rightShapes)
            right->box = BoundingBox::mergeBox(bshape->box, right->box);

        node->children.push_back(left);
        node->children.push_back(right);

        build(left, leftShapes);
        build(right, rightShapes);
    }

    std::optional<BoundingVolumeHierarchy::TracedData> BoundingVolumeHierarchy::traceEllipsoid(glm::vec3 pos, glm::vec3 dir, glm::vec3 scale) {
        std::optional<BoundingVolumeHierarchy::TracedData> result = {};
        if (root) traceEllipsoid(pos, dir, scale, root, result);
        return result;
    }

    void BoundingVolumeHierarchy::traceEllipsoid(glm::vec3 pos, glm::vec3 dir, glm::vec3 scale, std::shared_ptr<Node> node, std::optional<BoundingVolumeHierarchy::TracedData> &result) {
        // this should only run for terminal nodes
        // we loop through all shapes and see if any intersects with our ellipsoid
        for (BoundedShapeData* brsd : node->shapes) {
            float tc = Saga::Geometry::ellipsoidTriangleCollision(pos, dir, scale, 
                brsd->data.triangle[0], brsd->data.triangle[1], brsd->data.triangle[2]);

            if (tc >= 0 && (!result.has_value() || tc < std::get<1>(result.value()))) 
                result = {&brsd->data, tc};
        }

        // recurse through children
        for (std::shared_ptr<Node> child : node->children) {
            std::optional<float> tc = child->box.findCollisionWithBox(pos, dir, scale);
            if (tc.has_value() && (!result.has_value() || tc.value() < std::get<1>(result.value()))) 
                traceEllipsoid(pos, dir, scale, child, result);
        }
    }

    void BoundingVolumeHierarchy::BoundedShapeData::computeBound() {
        box = BoundingBox::getExtremeBound();
        // construct bounding box
        for (int tri_index = 0; tri_index < 3; tri_index++) {
            for (int dim = 0; dim < 3; dim++)
                box.bounds[0][dim] = std::min(box.bounds[0][dim], data.triangle[tri_index][dim]);
            for (int dim = 0; dim < 3; dim++)
                box.bounds[1][dim] = std::max(box.bounds[1][dim], data.triangle[tri_index][dim]);
        }
    }
}
