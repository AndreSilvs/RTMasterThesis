#pragma once

#include <string>

#include "XMLParser.h"

class RTConfig {
public:
    //RTConfig();

    bool readConfigFile( const std::string &fileName );


    std::string _heightmapFileName;
    std::string _heightmapExtension;
    std::string _camPathFileName;
    bool _isHeightmapCompact;
    int _hTilesX, _hTilesY;
    int _hTileSizeX, _hTileSizeY;

    std::string _photoTexName;
    std::string _photoExtension;


    float _mapHeight = 256.0f;
    float _patchSize = 1.0f;
    float _loadFar = 1024.0f;
    float _visibilityFar = 1024.0f;

    int _bpc = 8;

	int _currentRes = 0;
	std::vector< std::pair< int, int > > _screenResolutions;

	bool successfulRead = false;

private:
    bool readConfig( XMLParser& parser, const std::string& fileName );
//private:
    //std::string _configFileName;

};
