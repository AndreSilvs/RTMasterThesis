#pragma once

#include <string>
#include <vector>

#include "Vector.h"

/*enum InputType {
    CAP_INPUT_KEY,
    CAP_INPUT_MOUSEBUTTON,
    CAP_INPUT_MOUSEMOTION
};*/


struct PathFrame {
    PathFrame();
    PathFrame( const Vector3D& position, const Vector3D& camDirection );
    Vector3D position;
    Vector3D cameraDirection;
};

class CameraPathScript {
public:
    void addFrame( const Vector3D& position, const Vector3D& cameraDir );

    void clear();
    
    int getNumberOfFrames();

    // FramePoint is a real number between 0 and NumFrames-1
    Vector3D getPosition( float framePoint );
    Vector3D getCameraDirection( float framePoint );

    bool readPathFromFile( const std::string &filename );
    bool savePathToFile( const std::string &filename );

private:
    std::vector< PathFrame > _frames;
};