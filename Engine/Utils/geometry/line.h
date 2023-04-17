#pragma once

namespace Saga::Geometry {
    /**
     * @brief Detect collision (and return minimum translation vector) of two line segments. 
     * 
     * @param alo starting point of the first line segment.
     * @param ahi ending point of the first line segment. Must be at least the starting point.
     * @param blo starting point of the second line segment.
     * @param bhi ending point of the second line segment. Must be at least the starting point.
     * @return float minimum distance the first line segment has to move for both to not be intersecting
     */
    float detectLineSegmentCollision(const float &alo, const float &ahi, const float &blo, const float &bhi);
}
