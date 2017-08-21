#include "XMLParser.h"

#include <iostream>

#include <fstream>

#include "FileReading.h"

XMLNode::XMLNode() : error( true ) {
	
}
XMLAttribute* XMLNode::getAttribute( const std::string &attName ) {
	for ( int i = 0, iEnd = (int)attributes.size(); i < iEnd; ++i ) {
		if ( attributes[ i ].first == attName ) {
			return &attributes[ i ];
		}
	}

	return nullptr;
}
XMLNode* XMLNode::getChild( const std::string &cName ) {
	for ( int i = 0, iEnd = (int)subNodes.size(); i < iEnd; ++i ) {
		if ( subNodes[ i ]->name == cName ) {
			return subNodes[ i ].get();
		}
	}

	return nullptr;
}
std::list< XMLNode* > XMLNode::getAllChildren( const std::string& cName ) {
    std::list< XMLNode* > nodes;
    for ( int i = 0, iEnd = (int)subNodes.size(); i < iEnd; ++i ) {
        if ( subNodes[ i ]->name == cName ) {
            nodes.push_back( subNodes[ i ].get() );
        }
    }
    return nodes;
}

void XMLNode::createAttribute( const std::string &aName, const std::string &content ) {
	attributes.push_back( XMLAttribute( aName, content ) );
}
XMLNode *XMLNode::createChild( const std::string &cName ) {
	subNodes.push_back( std::make_shared< XMLNode >() );
	subNodes.back()->name = cName;
	return subNodes.back().get();
}

XMLFileNode::XMLFileNode() : error( true ) {

}

// Read a string in the same format as a directory path and return the corresponding XMLNode, if any exists
XMLNode* XMLFileNode::findNode( const std::string &path ) {
    // No content or malformed path
    if ( path.empty() || path.back() == '/' ) {
        return nullptr;
    }
    if ( path.substr( 0, rootNode->name.size() ) != rootNode->name ) {
        return nullptr;
    }

    unsigned int start = rootNode->name.size() + 1;
    unsigned int end = start;
    std::string nodeName;

    XMLNode *target = rootNode.get();
    do {
        end = path.find( '/', start );
        if ( end == std::string::npos ) {
            nodeName = path.substr( start );
        }
        else {
            nodeName = path.substr( start, end - start );
        }

        target = target->getChild( nodeName );
        if ( target == nullptr ) {
            break;
        }
        start = end + 1;
    } while ( end != std::string::npos );
    return target;
}
// Read a string in the same format as a directory path and return the corresponding XMLAttribute, if any exists
XMLAttribute* XMLFileNode::findAttribute( const std::string &path ) {
    // No content or malformed path
    if ( path.empty() || path.back() == '/' ) {
        return nullptr;
    }
    if ( path.substr( 0, rootNode->name.size() ) != rootNode->name ) {
        return nullptr;
    }

    unsigned int start = rootNode->name.size() + 1;
    unsigned int end = start;
    std::string nodeName;

    XMLNode *target = rootNode.get();
    do {
        end = path.find( '/', start );
        if ( end == std::string::npos ) {
            nodeName = path.substr( start );
            return target->getAttribute( nodeName );
        }
        else {
            nodeName = path.substr( start, end - start );
        }

        target = target->getChild( nodeName );
        if ( target == nullptr ) {
            break;
        }
        start = end + 1;
    } while ( end != std::string::npos );
    return nullptr;
}


// ==============================================================
// XML PARSER CODE
// ==============================================================

XMLParser::XMLParser() : _errorMessage( "" ) {
}
const std::string& XMLParser::getErrorMessage() const {
    return _errorMessage;
}

std::shared_ptr< XMLNode > XMLParser::readXMLNode( std::ifstream &file ) {
	std::shared_ptr< XMLNode > node( new XMLNode() );

	// Read opening tag
	if ( !readExactChar( file, '<' ) ) { return node; }
	ignoreFileWhitespace( file );
	node->name = readWord( file );
	ignoreFileWhitespace( file );

	while ( true ) {
		// Check if end-of-file or IO failure
		if ( file.eof() ) { // Check failure too
            _errorMessage = "Unexpected EOF while reading node \"" + node->name + "\"";
			return node;
		}
		// Check if tag is closing
		else if ( isNextChar( file, '>' ) ) {
			file.get();
			break;
		}
		// Check if self-closing tag
		else if ( isNextChar( file, '/' ) ) {
			file.get(); // Ignore '/'
			// Self-closing tag
			if ( !isNextChar( file, '>' ) ) {
                _errorMessage = "\"" + node->name + "\" node's closing tag is missing a \'>\'";
				return node; // Error
			}
			else {
				file.get(); // Ignore '>'
				node->error = false;
				return node;
			}
		}
		// Read XML attribute
		else {
			XMLAttribute attribute;
				
			// Read attribute name
			attribute.first = readWord( file );
			if ( attribute.first.empty() ) {
                _errorMessage = "Unnamed attribute in node <" + node->name + ">";
                return node;
            } // If name is empty then failed to read name
			if ( !readExactChar( file, '=' ) ) { return node; } // Check for '=' sign

			if ( !readExactChar( file, '\"' ) ) { return node; } // Check for double quotes
			while ( true ) {
				char nextChar;
				if ( file.eof() ) {
                    _errorMessage = "Unexpected EOF while reading attribute \"" + attribute.first + "\" from node <" + node->name + ">";
					return node;
				}

				nextChar = (char)file.get();
				if ( nextChar == '\"' ) {
					break;
				}
				attribute.second.push_back( nextChar );
			}

			node->attributes.push_back( attribute );

			// Ignore remaining whitespaces
			ignoreFileWhitespace( file );

		} // End of reading attribute

	}// End of reading element's opening tag

	ignoreFileWhitespace( file );

	// Read elements's body
	while ( true ) {
		// If reached end of file
		if ( file.eof() ) {
            _errorMessage = "Unexpected EOF while reading node \"" + node->name + "\"";
			return node;
		}
		// If < was found, it is either a new child node or the closing tag
		else if ( isNextChar( file, '<' ) ) {
			if ( peekNextChar( file, 2 ) == '/' ) {
				break; // Closing tag
			}
			else {
				std::shared_ptr< XMLNode > childNode = readXMLNode( file );
				if ( childNode->error ) {
                    // Error has been set before
					return node; // Error reading child node
				}
				node->subNodes.push_back( childNode );

				ignoreFileWhitespace( file );
			}
		}
		// Else read body's content
		else {
			while ( true ) {
				if ( file.eof() ) {
                    _errorMessage = "Unexpected EOF while reading node \"" + node->name + "\"";
					return node; // Error: Unexpected EOF
				}
				else if ( isNextChar( file, '<' ) ) {
					break;
				}
				else {
					node->content.push_back( (char)file.get() );
				}
			}
		}
	}

	// Read closing tag
    if ( !readExactString( file, "</" + node->name + ">" ) ) {
        _error = true;
        _errorMessage = "<" + node->name + "> node closing tag is missing or malformed.";
        return node;
    }

	node->error = false;
	return node;
}

// Read XML file
std::shared_ptr< XMLFileNode > XMLParser::readXMLFile( const std::string &fileName ) {
	std::ifstream xmlFile( fileName );

	std::shared_ptr< XMLFileNode > fileDescription( new XMLFileNode() );

    _error = false;
    _errorMessage = "";

	// If file was successfully opened
	if ( xmlFile.is_open() ) {

		fileDescription->rootNode = readXMLNode( xmlFile );
		if ( fileDescription->rootNode->error ) {
			fileDescription->rootNode.reset();
		}
		else {
			fileDescription->error = false;
		}

		xmlFile.close();
	}

	return fileDescription;
}

void printXMLNode( XMLNode *node, int depth ) {
	for ( int j = 0; j < depth; ++j ) {
		std::cout << '\t';
	}
	std::cout << "NodeName: " << node->name << std::endl;
	for ( int j = 0; j < depth; ++j ) {
		std::cout << '\t';
	}
	std::cout << "[C] " << node->content << std::endl;


	for ( int i = 0; i < (int)node->attributes.size(); ++i ) {
		for ( int j = 0; j < depth; ++j ) {
			std::cout << '\t';
		}
		std::cout << "[A] " << node->attributes[ i ].first << " = " << node->attributes[ i ].second << std::endl;
	}

	for ( int i = 0; i < (int)node->subNodes.size(); ++i ) {
		printXMLNode( node->subNodes[ i ].get(), depth + 1 );
	}
}

bool XMLParser::readFloatXML( float *f, XMLAttribute *attribute ) {
    try {
        *f = (float)atof( attribute->second.c_str() );
    }
    catch ( ... ) {
        _errorMessage =  "\"" + attribute->first + "\" not a float: " + attribute->second; return false;
    }

    return true;
}
bool XMLParser::readIntegerXML( int *i, XMLAttribute *attribute ) {
    try {
        *i = atoi( attribute->second.c_str() );
    }
    catch ( ... ) {
        _errorMessage = "\"" + attribute->first + "\" not an integer: " + attribute->second; return false;
    }

    return true;
}

void printXMLTree( XMLFileNode *node ) {
	printXMLNode( node->rootNode.get(), 0 );
}
