#pragma once

#include <random>
#include <glm/vec2.hpp>

namespace Saga::Random {
    extern std::mt19937 rng; //!< mersene twister used for generating random values.

    /**
     * @brief Generate a uniform random variable in the range [0,1]
     * @return the variable.
     */
    float getUniformRandom01();

    /**
     * @brief Generate a point uniform randomly on the unit disk.
     * @return the point.
     */
    glm::vec2 getUniformUnitDisk();
}
