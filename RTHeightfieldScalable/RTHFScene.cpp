#include "RTHFScene.h"

#include "MathCalcs.h"


std::string getRenderModeString( RTRenderMode rm ) {
    if ( rm == RT_RENDERMODE_PHOTO ) {
        return "photoShading";
    }
    else if ( rm == RT_RENDERMODE_HEIGHT ) {
        return "heightShading";
    }
    else if ( rm == RT_RENDERMODE_DEPTH ) {
        return "depthShading";
    }
    else if ( rm == RT_RENDERMODE_GRID ) {
        return "gridShading";
    }
    else if ( rm == RT_RENDERMODE_LOD ) {
        return "lodShading";
    }
    else if ( rm == RT_RENDERMODE_LIGHTING ) {
        return "lightingShading";
    }
    else if ( rm == RT_RENDERMODE_LIGHTGRID ) {
        return "lightingGridShading";
    }
    else if ( rm == RT_RENDERMODE_NORMAL ) {
        return "normalShading";
    }
    else if ( rm == RT_RENDERMODE_HEATMAP ) {
        return "stepHeatmapShading";
    }
    else if ( rm == RT_RENDERMODE_CUSTOM ) {
        return "customShading";
    }
    return "invalid";
}

/*void RTHFScene::precomputeUVN() {
    _n = cameraDirection.getNormalized();

    ( _u = Vector3D::crossProduct( _n, cameraUp ) ).normalize();

    ( _v = Vector3D::crossProduct( _u, _n ) ).normalize();

    float fov_v = mathCalcs::degreesToRadians( fovAngle );
    float fov_h = mathCalcs::degreesToRadians( fovAngle *( (float)screenResolution.y / (float)screenResolution.x ) );

    _screenRatios.x = 2.0f * tan( fov_h / 2.0f );
    _screenRatios.y = 2.0f * tan( fov_v / 2.0f );
}
const Vector2D& RTHFScene::getScreenRatios() const {
    return _screenRatios;
}


const Vector3D& RTHFScene::getU() const {
    return _u;
}
const Vector3D& RTHFScene::getV() const {
    return _v;
}
const Vector3D& RTHFScene::getN() const {
    return _n;
}*/


//
//
//


void RTHFSceneScalable::precomputeUVN() {
    _n = cameraDirection.getNormalized();

    ( _u = Vector3D::crossProduct( _n, cameraUp ) ).normalize();

    ( _v = Vector3D::crossProduct( _u, _n ) ).normalize();

    //float fov_v = mathCalcs::degreesToRadians( fovAngle );
    //float fov_h = mathCalcs::degreesToRadians( fovAngle *( (float)screenResolution.x / (float)screenResolution.y ) );
    //_screenRatios.x = 2.0f * tan( fov_h / 2.0f );
    //_screenRatios.y = 2.0f * tan( fov_v / 2.0f );

    float fov_rad = mathCalcs::degreesToRadians( fovAngle );

    _screenRatios.x = tan( fov_rad / 2.0f ) * ( (float)screenResolution.x / (float)screenResolution.y );
    _screenRatios.y = tan( fov_rad / 2.0f );
}

const Vector2D& RTHFSceneScalable::getScreenRatios() const {
    return _screenRatios;
}

const Vector3D& RTHFSceneScalable::getU() const {
    return _u;
}
const Vector3D& RTHFSceneScalable::getV() const {
    return _v;
}
const Vector3D& RTHFSceneScalable::getN() const {
    return _n;
}
