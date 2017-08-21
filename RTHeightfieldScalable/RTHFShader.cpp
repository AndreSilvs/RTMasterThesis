#include "RTHFShader.h"

#include "RTInitialization.h"

#include "API_Utility.h"

#include "DebugPrint.h"

#include <iostream>

RTHFShader::~RTHFShader() {
    closeShader();
}

bool RTHFShader::loadShader() {
    if ( !_shader.compileProgram( "Shaders/RTVS.vert", "Shaders/RTFS.frag" ) ) {
        return false;
    }

    std::cout << "Sucessfully compiled shader." << std::endl;
    auto unifList = _shader.getUniformData();
    std::cout << "Active uniforms\n";
    for ( auto& el : unifList ) {
        std::cout << el.second << "\t" << el.first << std::endl;
    }
    std::cout << "===================================\n";

    _shader.useProgram();

    // Get locations
    _ulHfSampler = _shader.getUniformLocation( "uHeightmap" );
    _ulMaxMipmapSampler = _shader.getUniformLocation( "uMaximumMips" );
    _ulPhotoTexSampler = _shader.getUniformLocation( "uPhotoTex" );
    _ulNormalSampler = _shader.getUniformLocation( "uNormalMap" );
    _ulLastLOD = _shader.getUniformLocation( "uLastLOD" );
    //ulTestCurrentLOD = _shader.getUniformLocation( "currentLOD" );
    _ulFarZ = _shader.getUniformLocation( "uFarZ" );
    _ulU = _shader.getUniformLocation( "uU" );
    _ulV = _shader.getUniformLocation( "uV" );
    _ulN = _shader.getUniformLocation( "uN" );
    _ulScreenDims = _shader.getUniformLocation( "uScreenDims" );
    _ulScreenRatios = _shader.getUniformLocation( "uScreenRatios" );
    _ulHeightmapDims = _shader.getUniformLocation( "uHeightmapDims" );
    _ulHeightmapLL = _shader.getUniformLocation( "uHeightmapLL" );
    _ulHeightmapBounds = _shader.getUniformLocation( "uHeightmapBounds" );
    _ulCameraEye = _shader.getUniformLocation( "uCameraEye" );

    _ulMaxLOD = _shader.getUniformLocation( "uMaxLOD" );
    _ulNumTiles = _shader.getUniformLocation( "uNumTiles" );
    _ulTileIndices = _shader.getUniformLocation( "uTileIndices[0]" );
    _ulTileDimensions = _shader.getUniformLocation( "uTileDimensions" );
    _ulLLTile = _shader.getUniformLocation( "uLLTile" );

    _ulLodZParam = _shader.getUniformLocation( "uLodZParam" );
    _ulLightDirection = _shader.getUniformLocation( "uLightDirection" );

    _ulRealPatchSize = _shader.getUniformLocation( "uRealPatchSize" );
    
    // Fill in data that never changes
    glUniform1i( _ulHfSampler, 0 );
    glUniform1i( _ulMaxMipmapSampler, 1 );
    glUniform1i( _ulPhotoTexSampler, 2 );
    glUniform1i( _ulNormalSampler, 3 );

    glGenBuffers( 1, &_atomicRayCounterID );
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, _atomicRayCounterID );
    glBufferData( GL_ATOMIC_COUNTER_BUFFER, sizeof( GLuint ), nullptr, GL_DYNAMIC_DRAW );
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 );

    glBindBufferBase( GL_ATOMIC_COUNTER_BUFFER, 0, _atomicRayCounterID );

    _shader.disableProgram();

    return true;
}
void RTHFShader::closeShader() {
    if ( _shader.isCompiled() ) {
        _shader.deleteProgram();
    }
}
bool RTHFShader::isLoaded() {
    return _shader.isCompiled();
}

/*void RTHFShader::renderScene( const RTHFScene& scene ) {
    glDisable( GL_DEPTH_TEST );
    _shader.useProgram();

    auto heightmapTex = scene.heightfield->getCompactHeightmap();

    apiut::setUniform3fv( _ulU, scene.getU() );
    apiut::setUniform3fv( _ulV, scene.getV() );
    apiut::setUniform3fv( _ulN, scene.getN() );
    
    apiut::setUniform2fv( _ulScreenDims, scene.screenResolution );
    apiut::setUniform2fv( _ulScreenRatios, scene.getScreenRatios() );

    Vector3D heightmapDimensions = Vector3D( (float)heightmapTex->getWidth(), (float)heightmapTex->getHeight(), scene.sceneHeight );
    //std::cout << "Heightmap Dims: "; printArray( &heightmapDimensions[ 0 ], 3 );
    apiut::setUniform3fv( _ulHeightmapDims, heightmapDimensions );
    apiut::setUniform3fv( _ulCameraEye, scene.cameraEye );

    glUniform1f( _ulFarZ, scene.farZ );

    glUniform1i( _ulLastLOD, scene.heightfield->getLastMipLevel() );

    GLuint shadingFunc = 0;
    if ( scene.renderMode == RT_RENDERMODE_PHOTO ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "photoShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_HEIGHT ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "heightShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_DEPTH ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "depthShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_GRID ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "gridShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_LOD ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "lodShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_LIGHTING ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "lightingShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_NORMAL ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "normalShading" );
    }
    /*else if ( scene.renderMode == RT_RENDERMODE_SPECIAL ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "specialShading" );
    }/*/
    /*glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &shadingFunc );

    heightmapTex->bindTexture( 0 );
    scene.heightfield->getMaximumMipmap()->bindTexture( 1 );
    scene.heightfield->getPhotoTexture()->bindTexture( 2 );
    myrt::screenQuad.render();
    Texture::unbindTexture( 2 );
    Texture::unbindTexture( 1 );
    Texture::unbindTexture( 0 );

    _shader.disableProgram();
    glEnable( GL_DEPTH_TEST );
}*/

void RTHFShader::renderScene( const RTHFSceneScalable& scene ) {
    glDisable( GL_DEPTH_TEST );
    _shader.useProgram();

    auto heightmapTex = scene.heightfield->getCompactHeightmap();

    apiut::setUniform3fv( _ulU, scene.getU() );
    apiut::setUniform3fv( _ulV, scene.getV() );
    apiut::setUniform3fv( _ulN, scene.getN() );

    apiut::setUniform2fv( _ulScreenDims, scene.screenResolution );
    apiut::setUniform2fv( _ulScreenRatios, scene.getScreenRatios() );

    Vector3D heightmapDimensions = Vector3D( (float)scene.heightfield->getHeightmapWidth(), (float)scene.heightfield->getHeightmapHeight(), scene.sceneHeight );
    //std::cout << "Heightmap Dims: "; printArray( &heightmapDimensions[ 0 ], 3 );
    apiut::setUniform3fv( _ulHeightmapDims, heightmapDimensions );
    apiut::setUniform3fv( _ulCameraEye, scene.heightfield->getUserPosition() );

    glUniform1f( _ulFarZ, scene.farZ );

    glUniform1i( _ulLastLOD, scene.heightfield->getLastMipLevel() );
    glUniform1i( _ulMaxLOD, scene.heightfield->getMaxMipLevel() );

    glUniform1iv( _ulTileIndices, 9, scene.heightfield->getTileOrderArray() );

    glUniform1f( _ulLodZParam, scene.lodZParam );

    glUniform1f( _ulRealPatchSize, scene.heightfield->getPatchSize() );

    apiut::setUniform2fv( _ulHeightmapLL, scene.heightfield->getPointLL() );
    apiut::setUniform2fv( _ulHeightmapBounds, scene.heightfield->getBounds() );
    glUniform2i( _ulLLTile, scene.heightfield->getLowerLeftX(), scene.heightfield->getLowerLeftY() );
    apiut::setUniform2fv( _ulTileDimensions, scene.heightfield->getTileDimensions() );

    apiut::setUniform3fv( _ulLightDirection, scene.lightDirection );

    GLuint shadingFunc = 0;
    if ( scene.renderMode == RT_RENDERMODE_PHOTO ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "photoShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_HEIGHT ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "heightShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_DEPTH ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "depthShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_GRID ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "gridShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_LOD ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "lodShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_LIGHTING ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "lightingShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_LIGHTGRID ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "lightingGridShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_NORMAL ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "normalShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_HEATMAP ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "stepHeatmapShading" );
    }
    else if ( scene.renderMode == RT_RENDERMODE_CUSTOM ) {
        shadingFunc = _shader.getSubroutineLocation( GL_FRAGMENT_SHADER, "customShading" );
    }
    glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &shadingFunc );

    // Reset number of rays
    GLuint resetValue = 0;
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, _atomicRayCounterID );
    glBufferSubData( GL_ATOMIC_COUNTER_BUFFER, 0, sizeof( GLuint ), &resetValue );
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 );

    heightmapTex->bindTexture( 0 );
    scene.heightfield->getMaximumMipmap()->bindTexture( 1 );
    scene.heightfield->getPhotoTexture()->bindTexture( 2 );
    scene.heightfield->getNormalTexture()->bindTexture( 3 );
    myrt::screenQuad.render();
    Texture::unbindTexture( 3 );
    Texture::unbindTexture( 2 );
    Texture::unbindTexture( 1 );
    Texture::unbindTexture( 0 );

    // Obtain No. rays per frame
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, _atomicRayCounterID );
    glGetBufferSubData( GL_ATOMIC_COUNTER_BUFFER, 0, sizeof( GLuint ), &_raysPerFrame );
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 );

    _shader.disableProgram();
    glEnable( GL_DEPTH_TEST );
}

const std::string& RTHFShader::getLastError() const {
    return _shader.getLastError();
}

GLuint RTHFShader::getNumRays() {
    return _raysPerFrame;
}