#pragma once
#include "uniformGrid.h"

namespace Saga {

    template <class T>
    void UniformGrid<T>::insert(int x, int y, int z, T item) {
        std::tuple<int,int,int> key = std::make_tuple(x,y,z);
        cellMap[key].push_back(item);
    }

    template <class T>
    void UniformGrid<T>::remove(int x, int y, int z, T item) {
        std::tuple<int,int,int> key = std::make_tuple(x,y,z);
        if (!cellMap.count(key)) return;

        // loop through collection in cells, find the item
        GridCell& cell = cellMap[key];
        for (int i = 0; i < cell.size(); i++) {
            if (cell[i] == item) {
                std::swap(cell[i], cell[cell.size()-1]);
                cell.pop_back();
                break;
            }
        }
    }

    template <class T>
    std::optional<std::vector<T>> UniformGrid<T>::getCell(int x, int y, int z) {
        std::tuple<int,int,int> key = std::make_tuple(x,y,z);
        if (!cellMap.count(key))
            return {};
        return cellMap[key];
    }
}
