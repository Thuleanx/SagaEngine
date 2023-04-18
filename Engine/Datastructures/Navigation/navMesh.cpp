#include "navMesh.h"
#include <algorithm>
#include <glm/geometric.hpp>
#include <iterator>
#include <limits>
#include <unordered_map>
#include <queue>
#include "Engine/Utils/geometry/triangle.h"
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

std::optional<NavMesh::Path> NavMesh::findPath(glm::vec3 src, glm::vec3 dest) {
    std::optional<LocationInCell> fromLoc = getCell(src);
    std::optional<LocationInCell> toLoc = getCell(dest);
    if (!fromLoc || !toLoc) return {};

    if (fromLoc.value().cell == toLoc.value().cell) {
        // if in the same cell, shortest distance is simply from -> to.
        return {};
    }

    int n = edges.size();


    float d[n]; float h[n]; int p[n];
    std::fill(d, d+n, std::numeric_limits<float>::infinity());
    std::fill(h, h+n, std::numeric_limits<float>::infinity());
    std::fill(p, p+n, -1);

    using distanceEstimate = std::tuple<float, int>;
    std::priority_queue<distanceEstimate, 
        std::vector<distanceEstimate>, std::greater<distanceEstimate>> pq;

    { // loop through all edges in the start cell
        HalfEdge* halfEdge = faces[fromLoc.value().cell].halfEdge;
        for (int _ = 0; _ < 3; _++, halfEdge = halfEdge->nxt) {
            int edgeIndex = std::distance(halfEdge->edge, &edges[0]);
            d[edgeIndex] = glm::distance(src, edges[edgeIndex].center);
            h[edgeIndex] = glm::distance(dest, edges[edgeIndex].center);
            pq.push({d[edgeIndex] + h[edgeIndex], edgeIndex});
        }
    }

    std::array<int, 3> goalEdges;
    { // loop through all edges in the end cell
        HalfEdge* halfEdge = faces[toLoc.value().cell].halfEdge;
        for (int _ = 0; _ < 3; _++, halfEdge = halfEdge->nxt) {
            int edgeIndex = std::distance(halfEdge->edge, &edges[0]);
            goalEdges[_] = edgeIndex;
        }
    }

    int bestEdge = -1;

    while (!pq.empty()) {
        int e = std::get<1>(pq.top());
        float d_e = std::get<0>(pq.top());
        pq.pop();
        if (d_e != d[e] + h[e]) continue;

        // if is one of the goal edges
        if (std::find(goalEdges.begin(), goalEdges.end(), e) != goalEdges.end()) {
            bestEdge = e;
            break;
        }

        for (auto* halfEdge : { edges[e].halfEdge, edges[e].halfEdge->twin }) if (halfEdge) {
            for (auto* nxtEdge : {halfEdge->nxt, halfEdge->nxt->nxt} ) {
                int to = std::distance(nxtEdge->edge, &edges[0]);
                float dist = glm::distance(edges[e].center, nxtEdge->edge->center);
                if (d[to] > d[e] + dist) {
                    d[to] = d[e] + dist;
                    p[to] = e;
                    if (h[to] == -1) h[to] = glm::distance(edges[to].center, dest);
                    pq.push({d[to] + h[to], to});
                }
            }
        }
    }

    // we can't find a path between the two
    // then we go find whereever the heuristic distance is the shortest
    if (bestEdge == -1) {
        // find edge that's closest to destination
        for (int i = 0; i < n; i++) {
            if (bestEdge == -1 || h[bestEdge] > h[i]) 
                bestEdge = i;
        }
        // now change to new destination
        dest = edges[bestEdge].center;
    };

    float pathLength = d[bestEdge] + h[bestEdge];

    std::vector<int> edgesInPath;
    while (bestEdge != -1) {
        edgesInPath.push_back(bestEdge);
        bestEdge = p[bestEdge];
    }
    std::reverse(edgesInPath.begin(), edgesInPath.end());

    return Path{
        .length = pathLength,
        .from = src,
        .to = dest,
        .edges = edgesInPath
    };
}

std::optional<NavMesh::LocationInCell> NavMesh::getCell(glm::vec3 pos) {
    // if navmesh not initialized, then return nothing
    if (!initialized) return {};

    float closestDistance2 = std::numeric_limits<float>::infinity();
    int closestCell = -1;
    glm::vec3 closestPos;

    // loop through all cells
    // TODO: optimize with oct-tree
    for (int cell = 0; cell < faces.size(); cell++) {
        glm::vec3 projectedPoint = Geometry::getClosestPoint(pos, toTriangle(faces[cell]));
        float distance2 = glm::dot(projectedPoint - pos, projectedPoint - pos);
        if (distance2 < closestDistance2) {
            closestDistance2 = distance2;
            closestCell = cell;
            closestPos = projectedPoint;
        }
    }

    return NavMesh::LocationInCell{
        .cell = closestCell,
        .projectedPosition = closestPos
    };
}

Geometry::Triangle NavMesh::toTriangle(const Face& face) {
    return Geometry::Triangle{
        face.halfEdge->vertex->pos,
        face.halfEdge->nxt->vertex->pos,
        face.halfEdge->nxt->nxt->vertex->pos
    };
}

}
