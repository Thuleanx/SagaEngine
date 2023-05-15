#pragma once

#include <cmath>
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
     * @brief Solve the quadratic formula for real solutions for the equation at^2 + bt + c = 0.
     *
     * @param a
     * @param b
     * @param c
     * @return a vector containing either 0, 1, or 2 solutions.
     */
    std::vector<float> solveQuadraticReals(float a, float b, float c);

    /**
     * @brief Function used to smooth between two values. Useful as a framerate 
     * independent lerp.
     *
     * @tparam T the type that 
     * @param a the source value.
     * @param b the target value.
     * @param smoothing coefficient. 0 is no smoothing, 1 is infinitely smooth (and will return a always). Any value
     *  not in the range [0,1] will be clamped to said range.
     * @param dt the change in time.
     *
     * @return T the smoothed damped value between a and b according to dt.
     */
    template<typename T>
    T damp(T a, T b, float smoothing, float dt) {
        smoothing = std::clamp<float>(smoothing, 0, 1);
        float t = std::lerp(0, 1, 1 - std::pow(smoothing, dt));
        return t*b + (1 - t) * a;
    }
}
