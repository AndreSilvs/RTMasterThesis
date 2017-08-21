#pragma once

#include <cmath>

#include "Vector.h"

// Bounding box 2D defined by center and dimensions
struct AABB2D {
    Vector2D center;
    Vector2D dimensions;
};

// Bounding box 2D defined by its MIN and MAX positions
struct AABB2Dmm {
    Vector2D min;
    Vector2D max;
};

// Check if two stationary AABB's are intersecting
bool intersectAABBWithAABB( const AABB2D& box1, const AABB2D& box2 );
// Check if two stationary AABB's are intersecting
bool intersectAABBWithAABB( const AABB2Dmm& box1, const AABB2Dmm& box2 );

// Check if two, possibly moving, AABB's will intersect
// #tfirst = time of first intersection, #tlast = time of last intersection
bool intersectMovingAABBWithAABB( const AABB2Dmm& box1, const AABB2Dmm& box2, const Vector2D& velocity1, const Vector2D& velocity2, float& tfirst, float& tlast );
bool isPointInAABB( const Vector2D& point, const AABB2D& box );

struct Circle2D {
    Vector2D center;
    float radius;

    Circle2D();
    Circle2D( const Vector2D& center, float radius );
};

bool intersectCircleWithAABB( const Circle2D& circle, const AABB2Dmm& box );

Vector2D getClosestPointOnAABB( const AABB2Dmm& b, const Vector2D& point );
float getSquareDistanceToPointOnAABB( const AABB2Dmm& b, const Vector2D& point );

/*struct Ray2D {
    Vector2D origin;
    Vector2D direction;

    Vector2D getPoint( float t ) {
        return origin + t * direction;
    }
};

struct LineSegment2D {
    Vector2D start;
    Vector2D end;

    LineSegment2D( const Vector2D& startPoint, const Vector2D& endPoint ) : start( startPoint ), end( endPoint ) {}
};*/
