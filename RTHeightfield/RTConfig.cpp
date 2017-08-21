#include "RTConfig.h"
#include <iostream>

#include "XMLWriter.h"

#include "StringManipulation.h"

bool RTConfig::readConfigFile( const std::string &fileName ) {
    XMLParser parser;
    if ( !readConfig( parser, fileName ) ) {
        std::cout << parser.getErrorMessage() << std::endl;
        return false;
    }

    return true;
}

bool RTConfig::readConfig( XMLParser& xmlParser, const std::string& fileName ) {
    auto file = xmlParser.readXMLFile( fileName );
    if ( !file ) {
        return false;
    }

    if ( file->error ) {
        return false;
    }

	XMLAttribute* datasetNameAtt = file->rootNode->getAttribute( "dataset" );
	if ( !datasetNameAtt ) {
		std::cout << "Dataset name missing in root node. Cannot open file." << std::endl;
		return false;
	}
	std::string datasetName = datasetNameAtt->second;

	auto datasetNodes = file->rootNode->getAllChildren( "dataset" );
	XMLNode *selectedNode = nullptr;
	for ( auto dnode : datasetNodes ) {
		auto nameAtt = dnode->getAttribute( "name" );
		if ( nameAtt ) {
			if ( nameAtt->second == datasetName ) {
				selectedNode = dnode;
				break;
			}
		}
	}
	if ( selectedNode == nullptr ) {
		std::cout << "Dataset: \"" << datasetName << "\" does not exist." << std::endl;
		return false;
	}

    auto hmapNode = selectedNode->getChild( "heightmap" );
    auto ptexNode = selectedNode->getChild( "phototexture" );
    auto camscriptNode = selectedNode->getChild( "camscript" );
	auto camNode = selectedNode->getChild( "camera" );

    // Read heightmap data
    if ( hmapNode ) {
        auto hmapFile = hmapNode->getAttribute( "file" );
        if ( hmapFile ) { _heightmapFileName = hmapFile->second; }
        if ( _heightmapFileName.empty() ) { std::cout << "Heightmap Config entry contains an empty string instead of path." << std::endl; return false; }
        
        auto hmapBpp = hmapNode->getAttribute( "bpp" );
        if ( hmapBpp ) {
            if ( !xmlParser.readIntegerXML( &_hfBpp, hmapBpp ) ) {
                return false;
            }
        }
        else { _hfBpp = 8; }

        auto hmapCompact = hmapNode->getAttribute( "isCompact" );
        if ( hmapCompact && hmapCompact->second == "true" ) { _isHeightmapCompact = true; }
        else { _isHeightmapCompact = false; }
    }
    else {
        std::cout << "No entry found in Config file for Heightmap path." << std::endl;
        return false;
    }
    // Read phototexture data
    if ( ptexNode ) {
        auto ptexFile = ptexNode->getAttribute( "file" );
        if ( ptexFile ) { _photoTexName = ptexFile->second; }
    }

    if ( camscriptNode ) {
        auto camFile = camscriptNode->getAttribute( "file" );
        if ( camFile ) { _camPathFileName = camFile->second; }
    }

	_startPosition.first = false;
	_startDirection.first = false;
	if ( camNode ) {
		XMLAttribute* camPos = camNode->getAttribute( "position" );
		XMLAttribute* camDir = camNode->getAttribute( "direction" );

		if ( camPos ) {
			std::vector< std::string > posstring;
			splitString( posstring, camPos->second, ',' );
			
			if ( posstring.size() == 3 ) {
				try {
					for ( int i = 0; i < 3; ++i ) {
						_startPosition.second[ i ] = std::stof( posstring[ i ] );
					}
					_startPosition.first = true;
				}
				catch ( ... ) { std::cout << "Invalid data in camera position configuration." << std::endl; }
			}
		}

		if ( camDir ) {
			std::vector< std::string > dirstring;
			splitString( dirstring, camDir->second, ',' );

			if ( dirstring.size() == 2 ) {
				try {
					for ( int i = 0; i < 2; ++i ) {
						_startDirection.second[ i ] = std::stof( dirstring[ i ] );
					}
					_startDirection.first = true;
				}
				catch ( ... ) { std::cout << "Invalid data in camera direction configuration." << std::endl; }
			}
		}
	}

    // Read stats data
    auto statsNode = file->rootNode->getChild( "stats" );
    if ( statsNode ) {
        auto mapHeightAtt = statsNode->getAttribute( "mapheight" );
        if ( mapHeightAtt ) { xmlParser.readFloatXML( &_mapHeight, mapHeightAtt ); }

        auto patchSizeAtt = statsNode->getAttribute( "patchsize" );
        if ( patchSizeAtt ) {
            if ( !xmlParser.readFloatXML( &_patchSize, patchSizeAtt ) ) {
                std::cout << "Incorrect patch size value: " << patchSizeAtt->second << ". Defaulting to 1.0 patch size." << std::endl;
                _patchSize = 1.0f;
            }
        }
    }

	auto screensNode = file->rootNode->getChild( "screenresolutions" );
	if ( screensNode ) {
		auto resNodes = screensNode->getAllChildren( "resolution" );
		for ( auto res : resNodes ) {
			XMLAttribute* scWidthNode = res->getAttribute( "width" );
			XMLAttribute* scHeightNode = res->getAttribute( "height" );

			if ( scWidthNode && scHeightNode ) {
				int scWidth = 0;
				int scHeight = 0;

				if ( xmlParser.readIntegerXML( &scWidth, scWidthNode ) && xmlParser.readIntegerXML( &scHeight, scHeightNode ) ) {
					_screenResolutions.push_back( std::pair<int, int>( scWidth, scHeight ) );
				}
				else {
					std::cout << "Invalid data in <screenresolutions> node: " << scWidthNode->second << " " << scHeightNode->second << std::endl;
					return false;
				}
			}
			else {
				std::cout << "<screen> node missing either \"width\" or \"direction\"" << std::endl;
				return false;
			}
		}
	}

	if ( _screenResolutions.empty() ) {
		_screenResolutions.push_back( std::pair<int, int>( 1024, 768 ) );
	}

	successfulRead = true;

    return true;
}