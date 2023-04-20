#pragma once

#include "Engine/Utils/geometry/triangle.h"
#include <glm/vec3.hpp>
#include <string>
#include <utility>
#include <vector>
#include <optional>

namespace Saga {
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
		struct Vertex {
			HalfEdge *firstEdge;
            glm::vec3 pos;
			int degree;
		};
		struct Edge {
			HalfEdge *halfEdge;
            float highestAdminissibleRadius;
            glm::vec3 center;
		};
		struct Face {
			HalfEdge *halfEdge;
		};

    public:
        struct LocationInCell {
            int cell;
            glm::vec3 projectedPosition;
        };

        struct Path {
            float length;
            float radius;
            glm::vec3 from;
            glm::vec3 to;
            std::vector<std::pair<glm::vec3, glm::vec3>> portals;
        };

        struct WalkablePath {
            float radius;
            std::vector<glm::vec3> positions;
        };

        void buildFromFile(const std::string &filepath);
        void build(const std::vector<glm::vec3> &position, const std::vector<glm::ivec3> &faces);

        std::optional<Path> findPath(glm::vec3 from, glm::vec3 to, float radius);
        WalkablePath tracePath(const Path& path) const;
        std::optional<LocationInCell> getCell(glm::vec3 pos) const;
        glm::vec3 getRandomPosition() const;
    private:
        bool initialized;

        std::vector<Vertex> vertices;
        std::vector<Face> faces;
        std::vector<Edge> edges;
        std::vector<HalfEdge> halfEdges;

        Geometry::Triangle toTriangle(const Face& face) const;
    };
}

