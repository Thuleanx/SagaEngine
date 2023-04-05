#pragma once

#include <unordered_map>
#include <set>
#include <glm/vec3.hpp>
#include <optional>
#include <vector>
#include "Engine/Utils/tupleHash.h"

namespace Saga {
    template <class T>
    class UniformGrid {
        using GridCell = std::vector<T>;
    public:
        void insert(int x, int y, int z, T item);
        void remove(int x, int y, int z, T item);
        std::optional<GridCell> getCell(int x, int y, int z);
    private:
        std::unordered_map<std::tuple<int,int,int>, GridCell> cellMap;
    };
}

#include "uniformGrid.inl"

