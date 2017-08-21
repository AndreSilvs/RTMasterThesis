#include "RTInitialization.h"

namespace myrt {
    Vao screenQuad;

    bool initialize() {
        VaoInitializer vInit{ 1 };

        vInit.attributes[ 0 ].name = "position";
        vInit.attributes[ 0 ].usageHint = GL_STATIC_DRAW;
        vInit.attributes[ 0 ].attributeData = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            1.0f, 1.0f,
            -1.0f, 1.0f
        };
        vInit.attributes[ 0 ].attributeSize = 2;
        vInit.attributes[ 0 ].instanceDivisor = 0;

        vInit.useIndices = true;
        vInit.primitiveType = GL_TRIANGLES;
        vInit.indices = { 0, 1, 2, 0, 2, 3 };
        
        return screenQuad.createVao( vInit );
    }
}