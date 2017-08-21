#pragma once

#include "ShaderProgram.h"

#include "RTHFScene.h"
#include "HeightfieldSC.h"

class RTHFShader {
public:
    // The _ul prefix on variable names means "Uniform location"
    ~RTHFShader();

    bool loadShader();
    void closeShader();

    bool isLoaded();

    //void renderScene( const RTHFScene& scene );
    void renderScene( const RTHFSceneScalable& scene );

    const std::string& getLastError() const;

    GLuint getNumRays();

private:
    ShaderProgram _shader;

    GLuint _ulHfSampler;
    GLuint _ulMaxMipmapSampler;
    GLuint _ulPhotoTexSampler;
    GLuint _ulNormalSampler;
    GLuint _ulLastLOD, ulTestCurrentLOD;

    GLuint _ulFarZ;

    GLuint _ulU, _ulV, _ulN;
    GLuint _ulScreenDims, _ulScreenRatios;

    GLuint _ulHeightmapLL, _ulHeightmapBounds;
    GLuint _ulHeightmapDims;
    GLuint _ulCameraEye;

    GLuint _ulLodZParam;
    GLuint _ulLightDirection;

    GLuint _ulMaxLOD;
    GLuint _ulNumTiles;
    GLuint _ulTileIndices;
    GLuint _ulTileDimensions;
    GLuint _ulLLTile;

    GLuint _ulRealPatchSize;

    // Atomic counter for MRay per second
    GLuint _atomicRayCounterID = 0;
    GLuint* rayCounterPointer = nullptr;
    GLuint _raysPerFrame;
};