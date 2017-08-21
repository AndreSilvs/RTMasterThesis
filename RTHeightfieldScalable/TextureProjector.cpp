#include "TextureProjector.h"

TextureProjector::TextureProjector() :
    _center( 0.0f, -1.0f, 0.0f ), _at( 0.0f ), _up( 0.0f, 0.0f, 1.0f )
{
    _fov = 30.0f;
    _aspect = 1.0f;
    _near = 0.2f;
    _far = 1024.0f;
    calcMatrix();
}
TextureProjector::~TextureProjector() {
}

void TextureProjector::setPerspective( float fov, float aspect, float nearz, float farz ) {
    _fov = fov;
    _aspect = aspect;
    _near = nearz;
    _far = farz;
    calcMatrix();
}

void TextureProjector::setPosition( const Vector3D& pos ) {
    _center = pos;
    calcMatrix();
}
void TextureProjector::setTargetPoint( const Vector3D& point ) {
    _at = point;
    calcMatrix();
}
void TextureProjector::setUpVector( const Vector3D& up ) {
    _up = up;
    calcMatrix();
}
void TextureProjector::setLookAt( const Vector3D& pos, const Vector3D& point, const Vector3D& up ) {
    _center = pos;
    _at = point;
    _up = up;
    calcMatrix();
}

Matrix4D& TextureProjector::getProjectorMatrix() {
    return _projectorMatrix;
}

void TextureProjector::calcMatrix() {
    Matrix4D proj; setPerspectiveMatrix( proj, _fov, _aspect, _near, _far );
    Matrix4D view; setLookAtMatrix( view, _center, _at, _up );
    Matrix4D tsMatrix;
    setTranslationMatrix( tsMatrix, 0.5f, 0.5f, 0.5f );
    scaleMatrix( tsMatrix, 0.5f, 0.5f, 0.5f );
    _projectorMatrix = view * proj * tsMatrix;
}