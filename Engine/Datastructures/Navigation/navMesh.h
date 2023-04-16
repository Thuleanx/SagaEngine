#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <optional>

namespace Saga {
    class NavMesh {
    public:
        struct CellPlacement {
            int cell;
            glm::vec3 projectedPosition;
        };

        void build(const std::vector<glm::vec3> &position, const std::vector<glm::ivec3> &faces);
        std::optional<CellPlacement> getCell(glm::vec3 pos);
    private:
        bool initialized;

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
		};
		struct Face {
			HalfEdge *halfEdge;
		};

        std::vector<Vertex> vertices;
        std::vector<Face> faces;
        std::vector<Edge> edges;
        std::vector<HalfEdge> halfEdges;
    };
}

