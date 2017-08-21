#pragma once

#include "Texture.h"
#include "MatrixCalcs.h"
#include "VectorCalcs.h"

class TextureProjector {
public:
    TextureProjector();
    ~TextureProjector();

    void setPerspective( float fov, float aspect, float nearz, float farz );

    void setPosition( const Vector3D& pos );
    void setTargetPoint( const Vector3D& point );
    void setUpVector( const Vector3D& up );
    void setLookAt( const Vector3D& pos, const Vector3D& point, const Vector3D& up );

    Matrix4D& getProjectorMatrix();

private:
    void calcMatrix();
    Texture* _texture;

    float _fov, _aspect, _near, _far;

    Vector3D _center;
    Vector3D _at;
    Vector3D _up;

    Vector3D _scale;

    Matrix4D _projectorMatrix;
};