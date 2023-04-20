#pragma once

#include "Engine/Datastructures/Navigation/navMesh.h"

namespace Platformer {
struct SimpleTestAI {
    float movementSpeed;
    std::optional<Saga::NavMesh::WalkablePath> path;
};
}
