#include "random.h"
#include <random>
#include <chrono>

namespace Saga::Random {
    std::mt19937 rng = std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());

    float getUniformRandom01() {
        return std::uniform_real_distribution<float>(0,1)(rng);
    }

    glm::vec2 getUniformUnitDisk() {
        // we repeatly sample in a square until it's in the triangle.
        // usually cos and sin takes very long to compute, which is why 
        // we're not directly sampling a point.

        // Although if you factor in the cost of branching, maybe this performs worst.
        float x, y;
        do {
            // notice we don't use glm::vec2 here. Who knows how slow that is to construct.
            x = 2 * getUniformRandom01() - 1; y = 2 * getUniformRandom01() - 1;
            // also don't compute length, but length squared
            if (x*x + y*y <= 1) return glm::vec2(x,y);
        } while (true);

        return glm::vec2(0,0);
    }
}
