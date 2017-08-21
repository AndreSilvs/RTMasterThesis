#include "OBJloader.h"

#include <fstream>
#include <string>

#include <iostream>

#include "FileReading.h"

#include "StringManipulation.h"
#include "SystemErrors.h"
#include "Logging.h"

OBJModel::OBJModel() :
    _isLoaded( false )
{

}
OBJModel::~OBJModel() {

}

bool OBJModel::isLoaded() const {
    return _isLoaded;
}

void OBJModel::addVertex( float x, float y, float z ) {
	_positions.push_back( x );
    _positions.push_back( y );
    _positions.push_back( z );
}
void OBJModel::addNormal( float x, float y, float z ) {
	_normals.push_back( x );
	_normals.push_back( y );
	_normals.push_back( z );
}
void OBJModel::addTexCoord( float s, float t ) {
	_textureCoords.push_back( s );
	_textureCoords.push_back( t );
}

void OBJModel::addFace( int v1v, int v1t, int v1n, int v2v, int v2t, int v2n, int v3v, int v3t, int v3n ) {
	OBJFace face;
	face.v1[0] = v1v;
	face.v1[1] = v1t;
	face.v1[2] = v1n;

	face.v2[0] = v2v;
	face.v2[1] = v2t;
	face.v2[2] = v2n;

	face.v3[0] = v3v;
	face.v3[1] = v3t;
	face.v3[2] = v3n;

	_faces.push_back( face );
}

bool OBJModel::loadOBJfile( const std::string &fileName ) {
	std::ifstream file( fileName );

	if ( !file.is_open() ) {
		return false;
	}

	while ( !file.eof() ) {
		std::string segment = readSegment( file );

		if ( segment == "#" ) {
			// Comments
			ignoreFileLine( file );
		}
		else if ( segment == "o" ) {
			// Object name
            //getline( file, _objectName );
			ignoreFileLine( file );
		}
		else if ( segment == "v" ) {
			// Vertex
			float x,y,z;
			file >> x >> y >> z;
			ignoreFileWhitespace( file );
			addVertex( x, y, z );
		}
		else if ( segment == "vt" ) {
			// Tex Coord
			float s,t; // s,t ??
			file >> s >> t;
			ignoreFileWhitespace( file );
			addTexCoord( s, t );
		}
		else if ( segment == "vn" ) {
			float x,y,z;
			file >> x >> y >> z;
			ignoreFileWhitespace( file );
			addNormal( x, y, z );
		}
		else if ( segment == "s" ) {
			// Smoothing, not used
			ignoreFileLine( file );
		}
		else if ( segment == "f" ) {
			//Face
			int faceData[9];

			for ( int i = 0; i < 3; ++i ) {
				file >> faceData[ i*3 ];
				file.get();
				file >> faceData[ i*3 + 1 ];
				file.get();
				file >> faceData[ i*3 + 2 ];
				ignoreFileWhitespace( file );
			}

			// Adding -1 because OBJ indices start at 1
			addFace( faceData[0] - 1, faceData[1] - 1, faceData[2] - 1,
				faceData[3] - 1, faceData[4] - 1, faceData[5] - 1,
				faceData[6] - 1, faceData[7] - 1, faceData[8] - 1 );
		}
		else {
            Logging::error.reportWarning( 0, "OBJ Loader: Unknown command " + quote( segment ) );
            ignoreFileLine( file );
			break;
		}
	}

	file.close();

    _isLoaded = true;

	return true;
}

const std::vector< float > &OBJModel::getVertices() const {
    return _positions;
}
const std::vector< float > &OBJModel::getTextureCoords() const {
    return _textureCoords;
}
const std::vector< float > &OBJModel::getNormals() const {
    return _normals;
}

const std::vector< OBJFace > &OBJModel::getFaces() const {
    return _faces;
}

const std::string &OBJModel::getObjectName() const {
    return _objectName;
}
