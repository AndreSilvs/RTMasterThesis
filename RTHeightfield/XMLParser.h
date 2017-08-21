#pragma once

#include <memory>
#include <string>
#include <list>
#include <vector>

// TODO: Enforce naming rules such as node names must not start with numbers

/* Features:
    - Parse XML files into a recursive node structure
    - Multiple child-nodes may have the same name - Calling 'getChild' will only return the first node found
    - Node/attribute order is irrelevant
*/

// XML file structures
typedef std::pair< std::string, std::string > XMLAttribute;

struct XMLNode {
	std::string name;
	std::string content;

	std::vector< XMLAttribute > attributes;
	std::vector< std::shared_ptr< XMLNode > > subNodes;
	
	bool error;

	XMLNode();
	XMLAttribute* getAttribute( const std::string &name );
	XMLNode* getChild( const std::string &name );
    std::list< XMLNode* > getAllChildren( const std::string& name );

	void createAttribute( const std::string &name, const std::string &content );
	XMLNode *createChild( const std::string &name );

};
struct XMLFileNode {
	std::shared_ptr< XMLNode > rootNode;

	bool error;

    // Read a string in the same format as a directory path and return the corresponding XMLNode, if any exists
    XMLNode* findNode( const std::string &path );
    // Read a string in the same format as a directory path and return the corresponding XMLAttribute, if any exists
    XMLAttribute* findAttribute( const std::string &path );

	XMLFileNode();
};

class XMLParser {
public:
    XMLParser();

    std::shared_ptr< XMLFileNode > readXMLFile( const std::string &fileName );
    const std::string& getErrorMessage() const;

    bool readFloatXML( float *f, XMLAttribute *attribute );
    bool readIntegerXML( int *i, XMLAttribute *attribute );

private:
    bool _error;
    std::string _errorMessage;

    std::shared_ptr< XMLNode > readXMLNode( std::ifstream &file );
};

void printXMLTree( XMLFileNode *node );

