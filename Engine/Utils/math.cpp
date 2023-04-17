#include "math.h"
#include "../_Core/logger.h"
#include <cmath>
#include <limits>

namespace Saga::Math {
    std::optional<float> solvePositiveQuadratic(float a, float b, float c) {
        std::vector<float> sols = solveQuadraticReals(a, b, c);
        if (!sols.size()) return {};

        if (sols.size() > 1 && sols[1] < sols[0])
            std::swap(sols[0], sols[1]);

        if (sols[1] < 0) return {};
        if (sols[0] >= 0) return sols[0];
        return sols[1];
    }

    std::vector<float> solveQuadraticReals(float a, float b, float c) {
        // degenerate cases
        if (!a && !b && !c) 
            return {-std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity()};
        else if (!a && !b)
            return {}; // no solution
        else if (!a)
            return {-c/b}; // only one solution

        float d = b*b - 4*a*c;
        if (d < 0) return {}; // no real solution
        d = std::sqrt(d);

        if (!d) return {-b/(2*a)}; // only one solution
                                   //
        float c1 = (-b-d) / (2*a), c2 = (-b+d) / (2*a);
        return {c1, c2};
    }
}

