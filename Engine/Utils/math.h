#pragma once

#include <optional>
#include <vector>

namespace Saga::Math {
    /**
    * @brief Solve for the minimum t such that quadratic equation at^2 + bt + c = 0 is satisfied
    * 
    * @param a 
    * @param b 
    * @param c 
    * @return float the minimum positive t.
    * @return nothing if no such value exists, or it's complex.
    */
    std::optional<float> solvePositiveQuadratic(float a, float b, float c);

    /**
     * Solve the quadratic formula for real solutions for the equation at^2 + bt + c = 0.
     *
     * @param a
     * @param b
     * @param c
     * @return a vector containing either 0, 1, or 2 solutions.
     */
    std::vector<float> solveQuadraticReals(float a, float b, float c);
}
