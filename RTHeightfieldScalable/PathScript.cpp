#include "PathScript.h"

#include <fstream>

#include "FileReading.h"
#include "FileWriting.h"
#include "MathCalcs.h"


PathFrame::PathFrame() {}
PathFrame::PathFrame( const Vector3D& p, const Vector3D& d ) : position( p ), cameraDirection( d ) {
}

void CameraPathScript::addFrame( const Vector3D& position, const Vector3D& cameraDir ) {
    _frames.push_back( PathFrame( position, cameraDir ) );
}

int CameraPathScript::getNumberOfFrames() {
    return _frames.size();
}

void CameraPathScript::clear() {
    _frames.clear();
}

// FramePoint is a real number between 0 and NumFrames-1
Vector3D CameraPathScript::getPosition( float framePoint ) {
    if ( framePoint < 0 ) {
        return _frames[ (int)framePoint ].position;
    }
    if ( framePoint >= _frames.size() ) {
        return _frames.back().position;
    }
    float rate = framePoint - floor( framePoint );
    
    return ( 1 - rate ) * _frames[ (int)framePoint ].position + rate * _frames[ (int)framePoint + 1 ].position;
}
Vector3D CameraPathScript::getCameraDirection( float framePoint ) {
    if ( framePoint < 0 ) {
        return _frames[ (int)framePoint ].cameraDirection;
    }
    if ( framePoint >= _frames.size() ) {
        return _frames.back().cameraDirection;
    }
    float rate = framePoint - floor( framePoint );

    return ( 1 - rate ) * _frames[ (int)framePoint ].cameraDirection + rate * _frames[ (int)framePoint + 1 ].cameraDirection;
}

bool CameraPathScript::readPathFromFile( const std::string &filename ) {
    std::ifstream file{ filename, std::ios::binary };

    if ( file.is_open() ) {
        // Binary save mode
        if ( readExactString( file, "PATH" ) ) {
            unsigned short numFrames = 0;

            numFrames = readBinaryValue<unsigned short>( file );

            for ( int i = 0; i < numFrames; ++i ) {
                _frames.emplace_back();
                if ( !readBinaryValue<PathFrame>( file, _frames.back() ) ) { return false; }
            }
        
            return true;
        }

        // Text save mode - not working
        /*if ( readExactString( file, "PATH" ) ) {
            unsigned short numFrames;
            file >> numFrames;

            for ( int i = 0; i < numFrames; ++i ) {
                Vector3D p, d;
                file >> p.x >> p.y >> p.z >> d.x >> d.y >> d.z;
            }

            return true;
        }*/
    }

    return false;

}
bool CameraPathScript::savePathToFile( const std::string &filename ) {
    std::ofstream file{ filename, std::ios::binary };

    if ( file.is_open() ) {
        // Binary save mode
        file << "PATH";
        unsigned short numFrames = _frames.size();
        writeBinaryValue( file, numFrames );
        for ( int i = 0; i < numFrames; ++i ) {
            writeBinaryValue( file, _frames[ i ] );
        }

        file.close();

        return true;

        // Non-binary save mode
        /*file << "PATH\n";
        unsigned short numFrames = _frames.size();
        file << numFrames << std::endl;
        for ( int i = 0; i < numFrames; ++i ) {
            file << _frames[ i ].position.x << "," << _frames[ i ].position.y << "," << _frames[ i ].position.z << " "; 
            file << _frames[ i ].cameraDirection.x << "," << _frames[ i ].cameraDirection.y << "," << _frames[ i ].cameraDirection.z << "\n";
        }

        file.close();

        return true;*/
    }
    return false;
}