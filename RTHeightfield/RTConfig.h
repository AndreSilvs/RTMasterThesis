#pragma once

#include <string>

#include "XMLParser.h"

#include "Vector.h"

class RTConfig {
public:
    //RTConfig();

    bool readConfigFile( const std::string &fileName );


    std::string _heightmapFileName;

    bool _isHeightmapCompact;
    std::string _photoTexName;
    std::string _camPathFileName;

    float _mapHeight = 256.0f;
    int _hfBpp = 8;
    float _patchSize = 1.0f;

	// If true, then startingPos is set, false if not given in config file
	std::pair< bool, Vector3D > _startPosition;
	std::pair< bool, Vector2D > _startDirection;

	int _currentRes = 0;
	std::vector< std::pair< int, int > > _screenResolutions;
    float _cameraSpeed;


	bool successfulRead = false;

private:
    bool readConfig( XMLParser& parser, const std::string& fileName );
//private:
    //std::string _configFileName;

};
