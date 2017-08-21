#include "Collision3D.h"

bool checkCollision( AABB3D& aabb1, AABB3D &aabb2 ) {
    return abs( aabb1.center.x - aabb2.center.x ) < ( aabb1.dimensions.x + aabb2.dimensions.x ) / 2 ||
        abs( aabb1.center.y - aabb2.center.y ) < ( aabb1.dimensions.y + aabb2.dimensions.y ) / 2 ||
        abs( aabb1.center.z - aabb2.center.z ) < ( aabb1.dimensions.z + aabb2.dimensions.z ) / 2;
}
