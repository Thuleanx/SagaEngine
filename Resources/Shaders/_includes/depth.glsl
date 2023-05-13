
float linearizePerspectiveDepth(float depth) {
    float fon = far/near;
    depth = (1 - fon) * depth + fon;
    depth = 1.0 / depth;
    return depth;
}
