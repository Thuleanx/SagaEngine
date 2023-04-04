#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <set>
#include "boundingBox.h"
#include <optional>
#include <memory>

namespace Saga {

    /**
     * @brief A tree of bounding volumes, with leaf nodes containing a collection of triangles. 
     * Useful as a space-accelerant structure to speed up collision detection.
     */
    class BoundingVolumeHierarchy {
    public:
        /**
         * @brief Data on a single triangles in the structure. 
         */
        struct TriangleData {
            glm::vec3 triangle[3]; //!< coordinates of the triangle, in CCW order.
            Entity entity; //!< the entity the triangle is attached to.
        };

        /**
         * @brief Data on a bounded triangle, which includes the triangle itself 
         * and its BoundingBox.
         */
        struct BoundedShapeData {
            BoundingBox box; //!< triangle's bounding box.
            TriangleData data; //!< triangle's information.

            /**
             * @brief Recompute the bounding box of the triangle inside the data field.
             * Call this function after assigning data to this BoundedShapeData.
             */
            void computeBound();
        };

        /**
         * @brief Information about a trace operation on this datastructure, including
         * a pointer to the first TriangleData that the trace hits, as well as time t where it hits
         * this triangle.
         */
        using TracedData = std::tuple<TriangleData*, float>;
    private:
        /**
         * @brief A Node of the Bounding Volume Hierarchy.
         */
        struct Node {
            BoundingBox box; //!< Bounding box of the node, which is the union of the bounding box of all of its children's bounding volume.
            /** @brief All shapes that this bounding box contains, not including the shapes in its children. 
             * By convention, only the leaf nodes have this array be nonempty.
             */
            std::vector<BoundedShapeData*> shapes; 

            /**
             * Children nodes of this node. For a BoundingVolumeHierarchy, there is either two or zero children.
             */
            std::vector<std::shared_ptr<Node>> children;
        };
    public:
        /**
         * @brief Build a BoundingVolumeHierarchy based on a collection of TriangleData.
         *
         * @param triangles all the triangles to use to build this BoundingVolumeHierarchy.
         */
        void build(const std::vector<TriangleData> &triangles);


        /**
         * @brief Trace an ellipsoid through the Bounding Volume Hierarchy, reporting the first
         * intersection if it exists.
         *
         * @param pos the starting position of the ellipsoid.
         * @param dir the direction the ellipsoid is heading. We only trace up to this direction, meaning if the intersection happens 
         *      further than pos + dir, we do not count it
         * @param scale the radius of the ellipsoid in the three cardinal directions.
         * @return std::optional<TracedData> containing nothing, or a pair of TriangleData*, and a float t signifying
         *      that the intersection happens first at pos + dir
         */
        std::optional<TracedData> traceEllipsoid(glm::vec3 pos, glm::vec3 dir, glm::vec3 scale);

    private:

        /**
         * Maximum number of triangles we want in leaf nodes. If a node potentially
         * contains at most this number of triangles, we make it a leaf node.
         * This is because at a certain point, it is cheaper to detect intersection with all 
         * triangles within a node than to descend down its constructed tree, since 
         * detecting intersections with the bounding boxes has a cost.
         */
        const int NUM_TRIANGLES_PER_LEAF = 10;

        /**
         * @brief Build a BoundingVolumeHirarchy based on a collection of TriangleData at a certain
         * Node in the tree. This node must already have a valid bounding box that encapsulates all
         * triangles in shapes, and must not have any children to begin with.
         *
         * @param node a shared_ptr to the node to put these triangles in.
         * @param shapes the collection of triangles used to build the BoundingVolumeHierarchy.
         */
        void build(std::shared_ptr<Node> node, std::vector<BoundedShapeData*> &shapes);

        std::shared_ptr<Node> root; //!< root node of the tree.
        std::vector<BoundedShapeData> allShapes; //!< Collection of bounded shapes on the leaf.

        /**
         * @brief Trace an ellipsoid through the Bounding Volume Hierarchy, reporting the first
         * intersection if it exists. This also prunes intersections by the result parameter sent in.
         *
         * @param pos the starting position of the ellipsoid.
         * @param dir the direction the ellipsoid is heading. We only trace up to this direction, meaning if the intersection happens 
         *      further than pos + dir, we do not count it
         * @param scale the radius of the ellipsoid in the three cardinal directions.
         * @param node the node to start the search in.
         * @param result contains the result of the ellipsoid trace. If result already
         *      contains a value for t, we prune any intersection that happens after t.
         */
        void traceEllipsoid(glm::vec3 pos, glm::vec3 dir, glm::vec3 scale, 
                std::shared_ptr<Node> node, std::optional<TracedData> &result);
    };


};
