#pragma once
#include "uniformGrid.h"

namespace Saga {

    template <class T>
    void UniformGrid<T>::insert(int x, int y, int z, T item) {
        cellMap[{x,y,z}].push_back(item);
    }

    template <class T>
    void UniformGrid<T>::remove(int x, int y, int z, T item) {
        std::tuple<int,int,int> key = std::make_tuple(x,y,z);
        if (!cellMap.count(key)) return;

        // loop through collection in cells, find the item, swap it to be the
        // last item, and then removal is easy.
        GridCell& cell = cellMap[key];
        cell.remove(item);
    }

    template <class T>
    std::optional<std::list<T>> UniformGrid<T>::getCell(int x, int y, int z) {
        std::tuple<int,int,int> key = std::make_tuple(x,y,z);
        if (!cellMap.count(key)) return {};
        return cellMap[key];
    }
}
