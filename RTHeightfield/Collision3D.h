#pragma once

#include <cmath>

#include "Vector.h"

struct Ray3D {
    Vector3D origin;
    Vector3D direction;
};

struct AABB3D {
    Vector3D center;
    Vector3D dimensions;
};

bool checkCollision( AABB3D& aabb1, AABB3D& aabb2 );