#pragma once

#pragma once

#include "API_Headers.h"

#include "Vector.h"

namespace apiut {
    void setUniform2fv( GLuint location, const Vector2D& v );
    void setUniform3fv( GLuint location, const Vector3D& v );
    void setUniform4fv( GLuint location, const Vector4D& v );
}
