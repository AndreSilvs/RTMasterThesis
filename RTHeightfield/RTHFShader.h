#pragma once

#include "ShaderProgram.h"

#include "RTHFScene.h"

class RTHFShader {
public:
    // The _ul prefix on variable names means "Uniform location"
    ~RTHFShader();

    bool loadShader();
    void closeShader();

    bool isLoaded();

    void renderScene( const RTHFScene& scene );

    const std::string& getLastError() const;

    GLuint getNumRays();

private:
    ShaderProgram _shader;

    GLuint _ulHfSampler;
    GLuint _ulMaxMipmapSampler;
    GLuint _ulPhotoTexSampler;
    GLuint _ulNormalMapSampler;
    GLuint _ulLastLOD, ulTestCurrentLOD;

    GLuint _ulFarZ;

    GLuint _ulU, _ulV, _ulN;
    GLuint _ulScreenDims, _ulScreenRatios;

    GLuint _ulHeightmapDims;
    GLuint _ulRealDims;
    GLuint _ulCameraEye;

    GLuint _ulLightDirection;

    GLuint _ulLodZParam;

    GLuint _ulRealPatchSize;

    // Atomic counter for MRay per second
    GLuint _atomicRayCounterID = 0;
    GLuint* rayCounterPointer = nullptr;

    GLuint _raysPerFrame;
};