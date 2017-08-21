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


    auto hmapNode = selectedNode->getChild( "heightmapscalable" );
    if ( hmapNode ) {
        XMLAttribute* hmapFile = hmapNode->getAttribute( "file" );
        XMLAttribute* hmapExt = hmapNode->getAttribute( "extension" );
        XMLAttribute* hmapTiles = hmapNode->getAttribute( "tiles" );
        XMLAttribute* hmapDims = hmapNode->getAttribute( "dimensions" );
        XMLAttribute* hmapIsComp = hmapNode->getAttribute( "isCompact" );
        XMLAttribute* hmapBPC = hmapNode->getAttribute( "bpc" );

        // If any component is missing, then not enough data
        if ( !hmapExt || !hmapFile || !hmapTiles || !hmapDims || !hmapIsComp ) {
            std::cout << "XML Config Error: Unspecified heightmap data: ";
            if ( !hmapFile ) { std::cout << "[file=\"...\"]"; }
            if ( !hmapExt ) { std::cout << "[extension=\"...\"]"; }
            if ( !hmapTiles ) { std::cout << "[tiles=\"x,y\"]"; }
            if ( !hmapDims ) { std::cout << "[dimensions=\"x,y\"]"; }
            if ( !hmapIsComp ) { std::cout << "[isCompact=\"true/false\"]"; }
            if ( !hmapBPC ) { std::cout << "[bpc=\"8,16\"]"; }
            std::cout << std::endl;
            return false;
        }

        // Process heightmap file name
        _heightmapFileName = hmapFile->second;
        if ( _heightmapFileName.empty() ) { std::cout << "XML Config Error: Heightmap file attribute is empty." << std::endl; return false; }

        if ( hmapIsComp->second == "true" ) { _isHeightmapCompact = true; }
        else { _isHeightmapCompact = false; }

        if ( hmapBPC && !hmapBPC->second.empty() ) {
            if ( !xmlParser.readIntegerXML( &_bpc, hmapBPC ) ) { std::cout << "BPC invalid value: " << hmapBPC->second << std::endl; return false; }
            if ( _bpc != 8 && _bpc != 16 ) { std::cout << "BPC invalid value: " << hmapBPC->second << "Only 8 and 16 bpc are supported." << std::endl; return false; }
        }
        else { _bpc = 8; }

        _heightmapExtension = hmapExt->second;
        if ( _heightmapExtension.empty() ) { std::cout << "XML Config Error: Heightmap extension attribute is empty." << std::endl; return false; }

        std::vector<std::string> nTiles;
        splitString( nTiles, hmapTiles->second, ',' );

        std::vector<std::string> tileDims;
        splitString( tileDims, hmapDims->second, ',' );

        // Check number of tiles and tile dimensions
        // Need to add checks for invalid integer values
        if ( nTiles.size() != 2 || tileDims.size() != 2 ) { std::cout << "XML Config Error: Heightmap tile/dimensions error." << std::endl; return false; }
        _hTilesX = std::stoi( nTiles[ 0 ] );
        _hTilesY = std::stoi( nTiles[ 1 ] );
        _hTileSizeX = std::stoi( tileDims[ 0 ] );
        _hTileSizeY = std::stoi( tileDims[ 1 ] );
    }
    else {
        std::cout << "XML Config Error: Heightmap name not found." << std::endl;
        return false;
    }

    // Read phototexture data
    auto ptexNode = selectedNode->getChild( "phototexturescalable" );
    if ( ptexNode ) {
        XMLAttribute* ptexFile = ptexNode->getAttribute( "file" );
        XMLAttribute* ptexExt = ptexNode->getAttribute( "extension" );
        XMLAttribute* ptexTiles = ptexNode->getAttribute( "tiles" );
        XMLAttribute* ptexDims = ptexNode->getAttribute( "dimensions" );

        if ( !ptexFile || !ptexExt || !ptexTiles || !ptexDims ) {
            std::cout << "XML Config Error: Unspecified phototexture data: ";
            if ( !ptexFile ) { std::cout << "[file=\"...\"]"; }
            if ( !ptexExt ) { std::cout << "[extension=\"...\"]"; }
            if ( !ptexTiles ) { std::cout << "[tiles=\"x,y\"]"; }
            if ( !ptexDims ) { std::cout << "[dimensions=\"x,y\"]"; }
            std::cout << std::endl;
            return false;
        }
        _photoTexName = ptexFile->second;
        if ( _photoTexName.empty() ) {
            std::cout << "XML Config Error: Phototexture file attribute is empty." << std::endl; return false;
        }
        _photoExtension = ptexExt->second;
        if ( _photoExtension.empty() ) {
            std::cout << "XML Config Error: Phototexture extension attribute is empty." << std::endl; return false;
        }
    }

    // Read stats data
    auto statsNode = selectedNode->getChild( "stats" );
    if ( statsNode ) {
        // Map Height
        auto mapHeightAtt = statsNode->getAttribute( "mapheight" );
        if ( mapHeightAtt ) { xmlParser.readFloatXML( &_mapHeight, mapHeightAtt ); }

        // Visibility Far
        auto statVisFar = statsNode->getAttribute( "visibilityfar" );
        if ( statVisFar ) { xmlParser.readFloatXML( &_visibilityFar, statVisFar ); }
        else { _visibilityFar = 1024.0f; }

        // Loading far (Active region shift distance)
        auto statLoadFar = statsNode->getAttribute( "loadingfar" );
        if ( statLoadFar ) { xmlParser.readFloatXML( &_loadFar, statLoadFar ); }
        else { _loadFar = 1024.0f; }

        // Patch Size
        auto patchSizeAtt = statsNode->getAttribute( "patchsize" );
        if ( patchSizeAtt ) {
            if ( !xmlParser.readFloatXML( &_patchSize, patchSizeAtt ) ) {
                std::cout << "Incorrect patch size value: " << patchSizeAtt->second << ". Defaulting to 1.0 patch size." << std::endl;
                _patchSize = 1.0f;
            }
        }
        else { _patchSize = 1.0f; }
    }

    auto camNode = selectedNode->getChild( "camscript" );
    if ( camNode ) {
        auto camFile = camNode->getAttribute( "file" );
        if ( camFile ) { _camPathFileName = camFile->second; }
    }

	// Read cycle resolutions
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