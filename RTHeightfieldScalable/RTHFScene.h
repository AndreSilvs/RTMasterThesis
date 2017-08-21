#pragma once

#include "Vector.h"
#include "Texture.h"

#include "Heightfield.h"
#include "HeightfieldSC.h"

enum RTRenderMode {
    RT_RENDERMODE_PHOTO,
    RT_RENDERMODE_HEIGHT,
    RT_RENDERMODE_DEPTH,
    RT_RENDERMODE_GRID,
    RT_RENDERMODE_LOD,
    RT_RENDERMODE_LIGHTING,
    RT_RENDERMODE_LIGHTGRID,
    RT_RENDERMODE_NORMAL,
    RT_RENDERMODE_HEATMAP,
    RT_RENDERMODE_CUSTOM,
    RT_RENDERMODE_TOTAL
};
std::string getRenderModeString( RTRenderMode rm );

/*class RTHFScene {
public:
    Vector3D cameraEye;
    Vector3D cameraDirection;
    Vector3D cameraUp;
    
    float fovAngle;
    Vector2D screenResolution;

    Heightfield* heightfield;

    void precomputeUVN();

    const Vector2D& getScreenRatios() const;

    const Vector3D& getU() const;
    const Vector3D& getV() const;
    const Vector3D& getN() const;

    float sceneHeight;
    float farZ;

    RTRenderMode renderMode;

private:
    Vector3D _u, _v, _n;

    Vector2D _screenRatios;
};*/

class RTHFSceneScalable {
public:
    //Vector3D cameraEye; = heightfield->userPosition;
    Vector3D cameraDirection;
    Vector3D cameraUp;

    float fovAngle;
    Vector2D screenResolution;

    HeightfieldScalableInterface* heightfield;

    void precomputeUVN();

    const Vector2D& getScreenRatios() const;

    const Vector3D& getU() const;
    const Vector3D& getV() const;
    const Vector3D& getN() const;

    float sceneHeight;
    float farZ;

    float lodZParam = 256.0;
    Vector3D lightDirection;

    RTRenderMode renderMode;

private:
    Vector3D _u, _v, _n;

    Vector2D _screenRatios;

};
