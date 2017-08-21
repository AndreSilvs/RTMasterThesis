#include "XMLWriter.h"

#include <fstream>

namespace {
	void indentationSpace( std::ofstream &file, int times ) {
		for ( int i = 0; i < times; ++i ) {
			file << " ";
		}
	}
	
	void writeXMLNode( std::ofstream &file, XMLNode *node, int depth ) {
		indentationSpace( file, depth * 4 );
		file << "<" << node->name;
		if ( node->attributes.size() > 0 ) {
			for ( int i = 0, iEnd = (int)node->attributes.size(); i < iEnd; ++i ) {
				file  << " " << node->attributes[ i ].first << "=\"" << node->attributes[ i ].second << "\"";
			}
		}
		if ( node->content.empty() && node->subNodes.empty() ) {
			file << "/>" << std::endl;
		}
		else {
			file << ">";
			if ( !node->content.empty() ) {
				file << node->content;
				file << "</" << node->name << ">" << std::endl;
			}
			else {
				file << std::endl;
				for ( int i = 0, iEnd = (int)node->subNodes.size(); i < iEnd; ++i ) {
						writeXMLNode( file, node->subNodes[ i ].get(), depth + 1 );
				}
				indentationSpace( file, depth * 4 );
				file << "</" << node->name << ">" << std::endl;
			}
		}
	}
}

void writeXMLFile( XMLFileNode *node, const std::string &fileName ) {
	std::ofstream file( fileName );
	
	if ( !file.is_open() ) {
		return;
	}
	
	writeXMLNode( file, node->rootNode.get(), 0 );
	file.close();
}