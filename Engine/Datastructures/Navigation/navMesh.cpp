#include "navMesh.h"
#include <unordered_map>
#include "Engine/Utils/tupleHash.h"

namespace Saga {

void NavMesh::build(const std::vector<glm::vec3> &positions, const std::vector<glm::ivec3> &inputFaces) {
	const int EDGES_IN_FACE = 3;
    vertices.clear();
    edges.clear();
    faces.clear();
    halfEdges.clear();

    // reserve appropriate sizes
    vertices.reserve(positions.size());
    faces.reserve(inputFaces.size());
    edges.reserve(EDGES_IN_FACE * inputFaces.size());
    halfEdges.reserve(EDGES_IN_FACE * inputFaces.size());

	// maps edge indices (u,v) -> int(halfEdgeIndex, edgeIndex)
    std::unordered_map<std::tuple<int,int>, std::tuple<int,int>> edgeMap;

    for (const glm::vec3 &pos : positions) {
        vertices.emplace_back(Vertex{
            .pos = pos,
            .degree = 0
        });
    }

    for (const glm::ivec3& face : inputFaces) {
		HalfEdge *faceHalfEdge[EDGES_IN_FACE];

		// Create all 3 half edges to a face
        for (int j = 0; j < EDGES_IN_FACE; j++) {
			int u = face[j], v = face[(j + 1) % EDGES_IN_FACE];

			halfEdges.emplace_back(
                HalfEdge{.vertex = &vertices[u]}
            );
            HalfEdge *currentEdge = &halfEdges.back();

            vertices[u].degree++;
            vertices[u].firstEdge = currentEdge;

			// halfEdges.back() is the current edge
            std::tuple<int,int> edgePair = {std::min(u, v), std::max(u, v)};

			// If that edge has been encountered before
			if (edgeMap.count(edgePair)) {
                auto [halfEdgeIndex, edgeIndex] = edgeMap[edgePair];

				currentEdge->edge = &edges[edgeIndex];
				currentEdge->twin = &halfEdges[halfEdgeIndex];
                halfEdges[halfEdgeIndex].twin = currentEdge;

			} else {
                edges.emplace_back(Edge{
                    .halfEdge = currentEdge
                });
				currentEdge->edge = &edges.back();
				edgeMap[edgePair] = {halfEdges.size() - 1, edges.size() - 1};
			}

			faceHalfEdge[j] = currentEdge;
		}

        faces.emplace_back(Face{
            .halfEdge = faceHalfEdge[0]
        });

		// Assign nxt variables
        for (int j = 0; j < EDGES_IN_FACE; j++) {
			faceHalfEdge[j]->nxt = faceHalfEdge[(j + 1) % EDGES_IN_FACE];
			faceHalfEdge[j]->face = &faces.back();
		}
	}

    edges.shrink_to_fit();


    initialized = true;
}

}
