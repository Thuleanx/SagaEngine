#include "boundingBox.h"
#include "Engine/Utils/geometry.h"
#include <iostream>
#include <cstdio>
using namespace glm;
using namespace std;

#define f(i,a,b) for (int i = a; i < b; i++)
#define oo 1e9
#define sq(x) ((x)*(x))

namespace Saga {

    vec3 BoundingBox::centroid() {
        return (bounds[1] + bounds[0]) / 2.0f;
    }

    vec3 BoundingBox::size() {
        return bounds[1] - bounds[0];
    }

    bool BoundingBox::inside(vec3 point) {
        f(i,0,3) if (point[i] > bounds[1][i] || point[i] < bounds[0][i]) return false;
        return true;
    }

    float BoundingBox::volume(){
        float x = 1;
        f(i,0,3) {
            if (bounds[1][i] < bounds[0][i]) return 0;
            x *= bounds[1][i] - bounds[0][i];
        }
        return x;
    }

    float BoundingBox::lowerBoundSquareDistanceTo(vec3 point) {
        float x = 0;
        f(i,0,3) {
            if (bounds[0][i] <= point[i] && point[i] <= bounds[1][i])
                continue;
            x += std::min(
                    sq(point[i] - bounds[0][i]),
                    sq(point[i] - bounds[1][i])
                    );
        }
        return x;
    }

    float  BoundingBox::surfaceArea() {
        float x = 0;
        f(i,0,3) {
            if (bounds[0][i] > bounds[1][i]) return 0;
            f(j,i+1,3) x += 2 * (bounds[1][i] - bounds[0][i]) * (bounds[1][j] - bounds[0][j]);
        }
        return x;
    }

    bool BoundingBox::collidesWithRay(vec4 point, vec4 direction) {
        return findCollisionWithRay(point,direction) != -1;
    }

    float BoundingBox::findCollisionWithRay(vec4 point, vec4 direction) {
        vec4 raysourceOS = point;
        vec4 rayOS = direction;

        if (inside(point)) return 0;

        float t = oo;

        f(d,0,3) {
            f(dir,0,2) {
                float tCan = rayOS[d] ? (bounds[dir][d] - raysourceOS[d]) / rayOS[d] : oo; 
                if (tCan >= 0 && tCan < t) {
                    vec4 hit = raysourceOS + tCan * rayOS;

                    bool insideCube = true;
                    f(k,0,3) if (k != d && (hit[k] < bounds[0][k] || hit[k] > bounds[1][k]))
                        insideCube = 0;
                    if (insideCube) t = tCan;
                }
            }
        }

        return t == oo ? -1 : t;
    }

    std::optional<float> BoundingBox::findCollisionWithEllipsoid(glm::vec3 point, glm::vec3 dir, glm::vec3 ellipsoid) {
        return Saga::Geometry::rayBoxCollision(point, dir, bounds[0] - ellipsoid/2.0f, bounds[1] + ellipsoid/2.0f);
    }

    bool BoundingBox::intersectWith(const BoundingBox &other) {
        f(i,0,3) if (bounds[0][i] > other.bounds[1][i] || other.bounds[0][i] > bounds[1][i]) return false;
        return true;
    }

    BoundingBox BoundingBox::mergeBox(const BoundingBox &a, const BoundingBox &b) {
        return BoundingBox{
            vec3(
                    std::min(a.bounds[0].x, b.bounds[0].x),
                    std::min(a.bounds[0].y, b.bounds[0].y),
                    std::min(a.bounds[0].z, b.bounds[0].z)
                ), vec3(
                    std::max(a.bounds[1].x, b.bounds[1].x),
                    std::max(a.bounds[1].y, b.bounds[1].y),
                    std::max(a.bounds[1].z, b.bounds[1].z)
                    )
        };
    }

    BoundingBox BoundingBox::confineBox(const BoundingBox &a, const BoundingBox &b) {
        return BoundingBox{
            vec3(
                    std::max(a.bounds[0].x, b.bounds[0].x),
                    std::max(a.bounds[0].y, b.bounds[0].y),
                    std::max(a.bounds[0].z, b.bounds[0].z)
                ), vec3(
                    std::min(a.bounds[1].x, b.bounds[1].x),
                    std::min(a.bounds[1].y, b.bounds[1].y),
                    std::min(a.bounds[1].z, b.bounds[1].z)
                    )
        };
    }

    BoundingBox BoundingBox::pointBox(const vec3 &point) {
        return BoundingBox {
            vec3(point), vec3(point)
        };
    }

    BoundingBox BoundingBox::getExtremeBound() {
        return BoundingBox{
            vec3(oo,oo,oo),
                vec3(-oo,-oo,-oo)
        };
    }

    void BoxedShapeData::computeBound() {
        box = BoundingBox::getExtremeBound();
        for (int tri_index = 0; tri_index < 3; tri_index++) {
            for (int dim = 0; dim < 3; dim++)
                box.bounds[0][dim] = std::min(box.bounds[0][dim], data.triangle[tri_index][dim]);
            for (int dim = 0; dim < 3; dim++)
                box.bounds[1][dim] = std::max(box.bounds[1][dim], data.triangle[tri_index][dim]);
        }
    }

};
