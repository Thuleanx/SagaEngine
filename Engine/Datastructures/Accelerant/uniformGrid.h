#pragma once

#include <unordered_map>
#include <set>
#include <glm/vec3.hpp>
#include <optional>
#include <vector>
#include "Engine/Utils/tupleHash.h"


namespace Saga {
    /**
    * @brief Models a uniform grid in 3D space, with the ability to insert and inspect 
    * certain cells in the grid. These cells contains a collection of objects.
    *
    * @tparam T the type of objects stored in each of the grid's cells.
    * @note useful for optimizing collisions of dynamic entities.
    */
    template <class T>
    class UniformGrid {
        using GridCell = std::vector<T>; //!< a grid cell is simply a container of objects
    public:
        /**
         * @brief Insert an item into the grid.
         *
         * @param x the x coordinate of the cell.
         * @param y the y coordinate of the cell.
         * @param z the z coordinate of the cell.
         * @param item the item to insert into the grid at (x,y,z).
         */
        void insert(int x, int y, int z, T item);

        /**
         * @brief Remove an item from a grid cell.
         * @param x the x coordinate of the cell.
         * @param y the y coordinate of the cell.
         * @param z the z coordinate of the cell.
         * @param item the item to remove from the grid cell at (x,y,z).
         */
        void remove(int x, int y, int z, T item);

        /**
         * @brief Get the grid cell at a certain coordinate.
         *
         * @param x the x coordinate of the cell.
         * @param y the y coordinate of the cell.
         * @param z the z coordinate of the cell.
         * @return {} if the grid cell at position at (x,y,z) has always been empty.
         * @return GridCell the list of objects at that cell position otherwise.
         */
        std::optional<GridCell> getCell(int x, int y, int z);
    private:
        std::unordered_map<std::tuple<int,int,int>, GridCell> cellMap; //!< Maps coordinates to GridCell.
    };
}

#include "uniformGrid.inl"

