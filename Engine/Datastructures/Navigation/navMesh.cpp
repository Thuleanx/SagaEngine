#include "navMesh.h"
#include <algorithm>
#include <exception>
#include <glm/geometric.hpp>
#include <iterator>
#include <limits>
#include <random>
#include <unordered_map>
#include <queue>
#include "Engine/Utils/geometry/triangle.h"
#include "Engine/Utils/tupleHash.h"
#include "Engine/_Core/asserts.h"
#include "Engine/_Core/logger.h"
#include "Graphics/global.h"
#include "glm/gtx/string_cast.hpp"
#include <chrono>
#include <random>
#include <functional>

namespace Saga {

void NavMesh::buildFromFile(const std::string &filepath) {
    SINFO("Building nav mesh from file %s", filepath.c_str());
    auto [positions, inputFaces] = GraphicsEngine::Global::graphics.getNavmeshData(filepath);
    build(positions, inputFaces);
    SINFO("Complete building nav mesh from file %s. The mesh contains %d vertices and %d faces.", filepath.c_str(), positions.size(), inputFaces.size());
}

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
    // useful for matching half edges that should belong to the same edge together.
    std::unordered_map<std::tuple<int,int>, std::tuple<int,int>> edgeMap;

    // initialize all the vertices.
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

			// halfEdges.back() is the current edge
            HalfEdge *currentEdge = &halfEdges.back();

            vertices[u].degree++;
            vertices[u].firstEdge = currentEdge;

            std::tuple<int,int> edgePair = {std::min(u, v), std::max(u, v)};

			// If that edge has been encountered before
			if (edgeMap.count(edgePair)) {
                auto [halfEdgeIndex, edgeIndex] = edgeMap[edgePair];

                // we match it with its twin
				currentEdge->edge = &edges[edgeIndex];
				currentEdge->twin = &halfEdges[halfEdgeIndex];
                halfEdges[halfEdgeIndex].twin = currentEdge;

			} else {
                // otherwise we need to create a new edge for it.
                edges.emplace_back(Edge{
                    .halfEdge = currentEdge,
                    .highestAdminissibleRadius = glm::distance(vertices[u].pos, vertices[v].pos),
                    .center = (vertices[u].pos + vertices[v].pos) / 2.f
                });
				currentEdge->edge = &edges.back();
				edgeMap[edgePair] = {halfEdges.size() - 1, edges.size() - 1};
			}
            SASSERT(currentEdge->edge != nullptr);

			faceHalfEdge[j] = currentEdge;
		}

        faces.emplace_back(Face{
            .halfEdge = faceHalfEdge[0]
        });
        SASSERT(faces.back().halfEdge != nullptr);

		// Assign nxt variables to all the half edges of the face.
        for (int j = 0; j < EDGES_IN_FACE; j++) {
			faceHalfEdge[j]->nxt = faceHalfEdge[(j + 1) % EDGES_IN_FACE];
			faceHalfEdge[j]->face = &faces.back();
		}
	}

    initialized = true;
}

std::optional<NavMesh::Path> NavMesh::findPath(glm::vec3 src, glm::vec3 dest, float radius) {
    // first projects the two points onto the nav mesh.
    std::optional<LocationInCell> fromLoc = getCell(src);
    std::optional<LocationInCell> toLoc = getCell(dest);
    if (!fromLoc || !toLoc) return {};
    if (fromLoc.value().cell == toLoc.value().cell) {
        // if in the same cell, shortest distance is simply from -> to.
        return Path{
            .length = glm::distance(src, dest),
            .from = fromLoc->projectedPosition,
            .to = toLoc->projectedPosition,
            .portals = {}
        };
    }

    // src and dest are no longer useful here
    int n = edges.size();

    float sourceDistance[n]; 
    float heuristicToDest[n]; 
    int previousEdgeIndex[n];
    HalfEdge* halfEdgeToVertex[n];

    float oo = std::numeric_limits<float>::infinity();

    std::fill(sourceDistance, sourceDistance+n, oo);
    std::fill(heuristicToDest, heuristicToDest+n, oo);
    std::fill(previousEdgeIndex, previousEdgeIndex+n, -1);
    std::fill(halfEdgeToVertex, halfEdgeToVertex+n, nullptr);


    using distanceEstimate = std::tuple<float, int>; // distance, halfEdge
    std::priority_queue<distanceEstimate,
        std::vector<distanceEstimate>, std::greater<distanceEstimate>> pq;

    // loop through faces next to starting location,
    // and populate the values there as well as push the edges to
    // the priority queue
    loopThroughFace(faces[fromLoc->cell], [&](HalfEdge* halfEdge) {
        Edge* edge = halfEdge->edge;

        int edgeIndex = getIndex(halfEdge->edge);
        int halfEdgeIndex = getIndex(halfEdge);

        sourceDistance[edgeIndex] = glm::distance(fromLoc->projectedPosition, edge->center);
        heuristicToDest[edgeIndex] = glm::distance(toLoc->projectedPosition, edge->center);

        halfEdgeToVertex[edgeIndex] = halfEdge;
        SASSERT(&halfEdges[halfEdgeIndex] == halfEdgeToVertex[edgeIndex]);
        pq.push({sourceDistance[edgeIndex] + heuristicToDest[edgeIndex], halfEdgeIndex});
    });

    std::vector<int> goalEdges;
    // loop through the ending face and determine where the goal edges are.
    // as soon as A* hits one of them, we're done.
    loopThroughFace(faces[toLoc->cell], [&](HalfEdge* halfEdge) {
        goalEdges.push_back(getIndex(halfEdge->edge));
    });

    int bestEdge = -1;

    // standard A* implementation
    while (pq.size()) {
        auto [distanceToEdge, halfEdgeIndex] = pq.top();
        int edgeIndex = getIndex(halfEdges[halfEdgeIndex].edge);
        pq.pop();

        // if we already visit this edge with a better distance
        // TODO: this should really use approximately
        if (&halfEdges[halfEdgeIndex] != halfEdgeToVertex[edgeIndex])
            continue;
        if (distanceToEdge != sourceDistance[edgeIndex] + heuristicToDest[edgeIndex])
            continue;

        // if is one of the goal edges, we can quit looking
        if (std::find(goalEdges.begin(), goalEdges.end(), edgeIndex) != goalEdges.end()) {
            bestEdge = edgeIndex;
            break;
        }

        auto relaxHalfEdge = [&](HalfEdge* nxtEdge) {
            if (nxtEdge == &halfEdges[edgeIndex]) return;

            int to = getIndex(nxtEdge->edge);

            float destinationDistance = glm::distance(edges[edgeIndex].center, nxtEdge->edge->center);

            // if relaxable
            if (sourceDistance[to] > sourceDistance[edgeIndex] + destinationDistance) {

                // compute heuristic distance, if not already computed.
                if (heuristicToDest[to] == std::numeric_limits<float>::infinity()) 
                    heuristicToDest[to] = glm::distance(edges[to].center, dest);

                sourceDistance[to] = sourceDistance[edgeIndex] + destinationDistance;
                previousEdgeIndex[to] = edgeIndex;
                halfEdgeToVertex[to] = nxtEdge;
                pq.push({sourceDistance[to] + heuristicToDest[to], getIndex(nxtEdge)});

            }
        };

        // look through and process all adjacent half edges
        for (auto* halfEdge : { edges[edgeIndex].halfEdge, edges[edgeIndex].halfEdge->twin }) if (halfEdge) {
            relaxHalfEdge(halfEdge->nxt);
            relaxHalfEdge(halfEdge->nxt->nxt);
        }
    }

    // we can't find a path between the two
    // then we go find whereever the heuristic distance is the shortest
    if (bestEdge == -1) {
        // find edge that's closest to destination
        for (int i = 0; i < n; i++) {
            if (sourceDistance[i] == oo) continue; // if unreachable
            if (bestEdge != -1 && heuristicToDest[bestEdge] < heuristicToDest[i]) continue; // if worst than best
            bestEdge = i;
        }
        // now change to new destination
        toLoc->projectedPosition = edges[bestEdge].center;
    };

    float pathLength = sourceDistance[bestEdge] + glm::distance(edges[bestEdge].center, toLoc->projectedPosition);

    std::vector<std::pair<glm::vec3, glm::vec3>> portalsInPath;
    while (bestEdge != -1) {
        // becareful not to use twin, as it might not exist
        portalsInPath.push_back({halfEdgeToVertex[bestEdge]->vertex->pos, halfEdgeToVertex[bestEdge]->nxt->vertex->pos});
        bestEdge = previousEdgeIndex[bestEdge];
    }
    std::reverse(portalsInPath.begin(), portalsInPath.end());

    return Path{
        .length = pathLength,
        .from = fromLoc->projectedPosition,
        .to = dest,
        .portals = portalsInPath
    };
}

std::optional<NavMesh::LocationInCell> NavMesh::getCell(glm::vec3 pos) const {
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
    SINFO("found closest cell: %d", closestCell);

    return NavMesh::LocationInCell{
        .cell = closestCell,
        .projectedPosition = closestPos
    };
}


NavMesh::WalkablePath NavMesh::tracePath(const Path &path) const {
    glm::vec3 apexPoint = path.from;

    std::vector<glm::vec3> positions;

    positions.push_back(apexPoint);

    // trace through the center of each portal
    for (int i = 0; i < path.portals.size(); i++) {
        auto [left, right] = path.portals[i];
        positions.push_back((left + right) / 2.f);
    }

    positions.push_back(path.to);

    std::reverse(positions.begin(), positions.end());

    return WalkablePath{
        .radius = path.radius,
        .positions = positions
    };
}

// UTILITY FUNCTIONS

std::optional<glm::vec3> NavMesh::getRandomPosition() const {
    if (!faces.size()) return {};

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    int faceIndex = std::uniform_int_distribution<int>(0, faces.size()-1)(rng);
    
    const Face& face = faces.at(faceIndex);

    float u = std::uniform_real_distribution<float>(0, 1)(rng);
    float v = std::uniform_real_distribution<float>(0, 1 - u)(rng);
    float w = 1 - u - v;

    float coef[3] = {u,v,w};
    glm::vec3 point = glm::vec3(0,0,0);

    const HalfEdge* halfEdge = face.halfEdge;

    for (int i = 0; i < 3; i++) {
        point += halfEdge->vertex->pos * coef[i];
        halfEdge = halfEdge->nxt;
    }
    return point;
}

Geometry::Triangle NavMesh::toTriangle(const Face& face) const {
    return Geometry::Triangle{
        face.halfEdge->vertex->pos,
        face.halfEdge->nxt->vertex->pos,
        face.halfEdge->nxt->nxt->vertex->pos
    };
}

void NavMesh::loopThroughFace(const Face& face, std::function<void(NavMesh::HalfEdge*)> processHalfEdge) {
    HalfEdge* currentEdge = face.halfEdge;
    int edgesLeft = 3;
    while (edgesLeft --> 0) {
        processHalfEdge(currentEdge);
        currentEdge = currentEdge->nxt;
    }
}

int NavMesh::getIndex(HalfEdge* halfEdge) { return std::distance(halfEdges.data(), halfEdge); }
int NavMesh::getIndex(Edge* edge) { return std::distance(edges.data(), edge); }
}
