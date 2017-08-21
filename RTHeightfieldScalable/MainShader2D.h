#pragma once

#include "API_Headers.h"

#include "ShaderProgram.h"

#include "MatrixCalcs.h"
#include "MatrixStack.h"
#include "Color.h"

#include "Vao.h"

#include "Texture.h"

namespace MainShader2D {

    extern ShaderProgram shader;

    extern MatrixStack projection;
    extern MatrixStack view;
    extern MatrixStack model;
    
    bool initializeShader();
    void closeShader();

    void setColor( Color4C color );
    void setMatrixes();
    void setModelMatrix();

    void setTextureUnit( GLenum unit );
    void setTextureClip( float minX, float minY, float sizeX, float sizeY );

    // Render texture using its original size
    void renderQuad( Texture* texture, Color4C color = { 255, 255, 255, 255 } );
    // Render texture scaled to the specified size
    void renderQuad( Texture* texture, float width, float height, Color4C color = { 255, 255, 255, 255 } );

    // Render texture as tiles fitting the specified size
    void renderQuadFit( Texture* texture, float width, float height, Color4C color = { 255, 255, 255, 255 } );
    // Render texture as tiles fitting the specified size with phase
    void renderQuadFit( Texture* texture, float phaseX, float phaseY, float width, float height, Color4C color = { 255, 255, 255, 255 } );

    // Render texture using its original size with phase
    void renderQuadPhased( Texture *texture, float phaseX, float phaseY, Color4C color = { 255, 255, 255, 255 } );
    // Render texture scaled to the specified size with phase
    void renderQuadPhased( Texture *texture, float phaseX, float phaseY, float width, float height, Color4C color = { 255, 255, 255, 255 } );

    // Render texture using a quad of its original size but with repetition
    void renderQuadRepeating( Texture *texture, float stretchX, float stretchY, Color4C color = { 255, 255, 255, 255 } );
    // Render texture using a quad scaled to the specified size with repetition
    void renderQuadRepeating( Texture *texture, float stretchX, float stretchY, float width, float height, Color4C color = { 255, 255, 255, 255 } );

    // Render texture using its original size and rotation (0 = No rotation, 0-180 = CCW, 180-360 = CW)
    void renderQuadRotated( Texture *texture, float orientation, Color4C color = { 255, 255, 255, 255 } );
    // Render texture scaled to the specified size and rotation (0 = No rotation, 0-180 = CCW, 180-360 = CW)
    void renderQuadRotated( Texture *texture, float orientation, float width, float height, Color4C color = { 255, 255, 255, 255 } );

    // Render cell (x,y) of from a spritesheet (numCellsX,numCellsY) - (0,0) = top-left
    void renderQuadClip( Texture* texture, int numCellsX, int numCellsY, int x, int y, Color4C color = { 255, 255, 255, 255 } );
    // Render cell (x,y) of from a spritesheet (numCellsX,numCellsY) with scale (width,height) - (0,0) = top-left
    void renderQuadClip( Texture* texture, int numCellsX, int numCellsY, int x, int y, int width, int height, Color4C color = { 255, 255, 255, 255 } );
    // Render the only cell (x,y) from spritesheet (cellsX,cellsY)
    void renderQuadClipInPlace( Texture *texture, int numCellsX, int numCellsY, int x, int y, Color4C color = { 255, 255, 255, 255 } );
}