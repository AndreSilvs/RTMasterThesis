#pragma once

#include <vector>

//#include "Vector.h"

struct OBJFace {
	int v1[3];
	int v2[3];
	int v3[3];
};

class OBJModel {
public:
    OBJModel();
	~OBJModel();

    bool isLoaded() const;
    bool loadOBJfile( const std::string &filename );

    const std::vector< float > &getVertices() const;
    const std::vector< float > &getTextureCoords() const;
    const std::vector< float > &getNormals() const;

    const std::vector< OBJFace > &getFaces() const;

    const std::string &getObjectName() const;

private:
    void addVertex( float x, float y, float z );
    void addNormal( float x, float y, float z );
    void addTexCoord( float x, float y );

    void addFace( int v1v, int v1t, int v1n, int v2v, int v2t, int v2n, int v3v, int v3t, int v3n );

    bool _isLoaded;

    std::string _objectName;

	std::vector< float > _positions;
	std::vector< float > _textureCoords;
	std::vector< float > _normals;

	std::vector< OBJFace > _faces;
};