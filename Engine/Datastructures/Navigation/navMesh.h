#pragma once

#include "Engine/Utils/geometry/triangle.h"
#include <functional>
#include <glm/vec3.hpp>
#include <string>
#include <utility>
#include <vector>
#include <optional>

namespace Saga {
    /**
     * @brief Represents a navigation mesh, best used for agents to find path 
     * between distant locations.
     */
    class NavMesh {
    private: 
        struct Face; struct Edge; struct Vertex; struct HalfEdge;
		struct HalfEdge {
			HalfEdge *twin;
			HalfEdge *nxt;
			Vertex *vertex;
			Edge *edge;
			Face *face;
		};

        /**
         * @brief Vertex of the navmesh.
         */
		struct Vertex {
			HalfEdge *firstEdge; //!< Any outdoing halfedge of the vertex. We can use
                                 // pointer operations to traverse all the other edges 
                                 // from this one.
            glm::vec3 pos; //!< Position of the vertex.
			int degree; //!< Degree of the vertex, useful for mesh topology changes.
		};

        /**
         * @brief Edge of the navmesh.
         */
		struct Edge {
			HalfEdge *halfEdge; //!< Pointer to one of its half edges. There must be
                                // at least 1 and at most 2 half edges per edge.
            float highestAdminissibleRadius; //!< Precomputed value that dictates what's the highest radius an entity can be to pass through this edge.
            glm::vec3 center; //!< Center of the edge, useful as vertices for performing pathfinding.
		};

        /**
         * @brief Face of the navmesh. Since we use a triangular mesh, this represents
         * one triangle.
         */
		struct Face {
			HalfEdge *halfEdge; //!< any of the three half edges in the triangle.
		};

    public:
        /**
         * @brief Specify a location in a cell, which is a triangle on the navmesh.
         */
        struct LocationInCell {
            int cell; //!< Index of the cell.
            glm::vec3 projectedPosition; //!< a position guaranteed to be inside of the cell.
        };

        /**
         * @brief Specify a path through the navmesh. This includes the source and destination, as well as a 
         * series of portals. Portals are edges that an agent will step through to get from source to destination.
         */
        struct Path {
            float length; //!< Length of the path, from source passing through all the midpoints of the portals, then to the destination.
            float radius; //!< Radius of entity traversing this path.
            glm::vec3 from; //!< Starting position.
            glm::vec3 to; //!< Ending position.
            std::vector<std::pair<glm::vec3, glm::vec3>> portals; //!< List of portals to pass through to get to destination.
                                                                  // These are ordered from -> to.
        };

        /**
         * @brief Specify a walkable path through the navmesh, which is a series of waypoint positions.
         */
        struct WalkablePath {
            float radius; //!< Radius of entity allowed to walk this path.
            std::vector<glm::vec3> positions; //!< Series of positions to get from the source to destination of the path.
                                              // These are ordered in reverse, from destination to source for convenience.
        };

        /**
         * @brief Build the navigtaion mesh from a file. This must be an obj file.
         *
         * @param filepath the path to the file, taken from the run directory.
         */
        void buildFromFile(const std::string &filepath);

        /**
         * @brief Build the navigation mesh from a list of positions and faces.
         *
         * @param position the 3d positions of all the vertices in the mesh.
         * @param faces a list of 3 tuple of indices representing the faces in counterclockwise order.
         */
        void build(const std::vector<glm::vec3> &position, const std::vector<glm::ivec3> &faces);

        /**
         * @brief Find the shortest path between two points in the navigation mesh, with 
         * edge midpoints and the destination and source be traversable nodes.
         * Also restrict the path such that only ones of a certain radius is walkable.
         *
         * @param from the source position.
         * @param to the destination position.
         * @return Path from source to destination.
         * @return nothing if the navigation mesh is not instantiated.
         *
         * @note that this path might not start or end at the two given positions. If the positions are not on the 
         * navigation mesh, it will try to find the closest positions on the mesh and use that to construct the path.
         */
        std::optional<Path> findPath(glm::vec3 from, glm::vec3 to, float radius);

        /**
         * @brief Smooth a path over the surface of the navigation mesh.
         *
         * @param path the path.
         * @return WalkablePath a smoothed out path over the mesh's surface.
         */
        WalkablePath tracePath(const Path& path) const;


        /**
         * @brief Project a position in world space to a cell (triangle) in the navigation mesh.
         *
         * @return nothing if the navigation mesh is not instantiated.
         * @return 
         */
        std::optional<LocationInCell> getCell(glm::vec3 pos) const;
        
        /**
         * @brief Get a random position on the mesh.
         *
         * @return glm::vec3 a position on the navigation mesh.
         * @return nothing if the navigation mesh has not been initialized.
         */
        std::optional<glm::vec3> getRandomPosition() const;
    private:
        bool initialized; //!< Specifies if the navigation mesh has been initialized.

        std::vector<Vertex> vertices; //!< Vertices of the mesh.
        std::vector<Face> faces; //!< Faces of the mesh.
        std::vector<Edge> edges; //!< Edge of the mesh.
        std::vector<HalfEdge> halfEdges; //!< Half edges of the mesh.

        /**
         * @brief Converts a face of the nav mesh to a builtin triangle.
         * Used as a utility function because the Triangle class has a lot of
         * useful functions.
         *
         * @param face the face we want to convert.
         * @return Geometry::Triangle the triangle specified by the 3 vertices of that face in counterclockwise order.
         */
        Geometry::Triangle toTriangle(const Face& face) const;

        /**
         * @brief Utility function to process all half edges in a face.
         *
         * @param face the face we want to process.
         * @param processHalfEdge the function to call on all halfEdges of the face, in
         * counterclockwise order from the firstEdge.
         */
        void loopThroughFace(const Face& face, std::function<void(HalfEdge*)> processHalfEdge);

        /**
         * @brief Get index of a halfedge by its pointer. It must be the case that the half edge
         * is in the navigation mesh.
         *
         * @param halfEdge the halfedge.
         * @return its index.
         */
        int getIndex(HalfEdge* halfEdge);

        /**
         * @brief Get index of an edge by its pointer. It must be the case that the edge
         * is in the navigation mesh.
         *
         * @param edge the edge.
         * @return its index.
         */
        int getIndex(Edge* edge);
    };
}

