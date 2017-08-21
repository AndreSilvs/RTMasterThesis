#include "Collision2D.h"

#include "MathCalcs.h"

#include <iostream>

Circle2D::Circle2D() {
}
Circle2D::Circle2D( const Vector2D& c, float r ) :
    center( c ), radius( r )
{
}


bool intersectAABBWithAABB( const AABB2D& box1, const AABB2D& box2 ) {
    return ( abs( box1.center.x - box2.center.x ) < ( box1.dimensions.x + box2.dimensions.x ) / 2.f ) &&
        ( abs( box1.center.y - box2.center.y ) < ( box1.dimensions.y + box2.dimensions.y ) / 2.f );
}

bool intersectAABBWithAABB( const AABB2Dmm& box1, const AABB2Dmm& box2 ) {
    return !( box1.max.x < box2.min.x || box1.min.x > box2.max.x ) &&
        !( box1.max.y < box2.min.y || box1.min.y > box2.max.y );
}

bool intersectMovingAABBWithAABB( const AABB2Dmm& box1, const AABB2Dmm& box2, const Vector2D& v1, const Vector2D& v2, float& tfirst, float& tlast ) {
    // Exit early if ‘a’ and ‘b’ initially overlapping
    if ( intersectAABBWithAABB( box1, box2 ) ) {
        tfirst = tlast = 0.0f;
        return true;
    }
    // Use relative velocity; effectively treating ’a’ as stationary
    Vector2D v = v2 - v1;

    // Initialize times of first and last contact
    tfirst = 0.0f;
    tlast = 1.0f;

    // For each axis, determine times of first and last contact, if any
    for ( int i = 0; i < 2; i++ ) {
        if ( v[ i ] < 0.0f ) {
            if ( box2.max[ i ] < box1.min[ i ] ) return false; // Nonintersecting and moving apart
            if ( box1.max[ i ] < box2.min[ i ] ) tfirst = mathCalcs::max( ( box1.max[ i ] - box2.min[ i ] ) / v[ i ], tfirst );
            if ( box2.max[ i ] > box1.min[ i ] ) tlast  = mathCalcs::min( ( box1.min[ i ] - box2.max[ i ] ) / v[ i ], tlast );
        }
        if ( v[ i ] >= 0.0f ) {
            if ( box2.min[ i ] > box1.max[ i ] ) return false; // Nonintersecting and moving apart
            if ( box2.max[ i ] < box1.min[ i ] ) tfirst = mathCalcs::max( ( box1.min[ i ] - box2.max[ i ] ) / v[ i ], tfirst );
            if ( box1.max[ i ] > box2.min[ i ] ) tlast  = mathCalcs::min( ( box1.max[ i ] - box2.min[ i ] ) / v[ i ], tlast );
        }
        // No overlap possible if time of first contact occurs after time of last contact
        if ( tfirst > tlast ) return false;
    }
    return true;
}

bool isPointInAABB( const Vector2D& point, AABB2D& box ) {
    return ( ( box.center.x - box.dimensions.x / 2 ) < point.x  && point.x < ( box.center.x + box.dimensions.x / 2 ) ) &&
        ( ( box.center.y - box.dimensions.y / 2 ) < point.y  && point.y < ( box.center.y + box.dimensions.y / 2 ) );
}


bool intersectCircleWithAABB( const Circle2D& circle, const AABB2Dmm& box ) {
    float sqDist = getSquareDistanceToPointOnAABB( box, circle.center );
    // Sphere and AABB intersect if the (squared) distance
    // between them is less than the (squared) sphere radius
    return sqDist <= circle.radius * circle.radius;
}

Vector2D getClosestPointOnAABB( const AABB2Dmm& box, const Vector2D& point ) {
    Vector2D res;
    for ( int i = 0; i < 2; i++ ) {
        float v = point[ i ];
        if ( v < box.min[ i ] ) v = box.min[ i ]; // v = max(v, b.min[i])
        if ( v > box.max[ i ] ) v = box.max[ i ]; // v = min(v, b.max[i])
        res[ i ] = v;
    }
    return res;
}
float getSquareDistanceToPointOnAABB( const AABB2Dmm& box, const Vector2D& point ) {
    float sqDist = 0.0f;
    for ( int i = 0; i < 2; i++ ) {
        // For each axis count any excess distance outside box extents
        float v = point[ i ];
        if ( v < box.min[ i ] ) sqDist += ( box.min[ i ] - v ) * ( box.min[ i ] - v );
        if ( v > box.max[ i ] ) sqDist += ( v - box.max[ i ] ) * ( v - box.max[ i ] );
    }
    return sqDist;
}