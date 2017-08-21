#include "API_Utility.h"

namespace apiut {
    void setUniform2fv( GLuint location, const Vector2D& v ) {
        glUniform2f( location, v.x, v.y );
    }
    void setUniform3fv( GLuint location, const Vector3D& v ) {
        glUniform3f( location, v.x, v.y, v.z );
    }
    void setUniform4fv( GLuint location, const Vector4D& v ) {
        glUniform4f( location, v.x, v.y, v.z, v.w );
    }
}