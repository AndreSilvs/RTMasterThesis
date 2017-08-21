#include "VAO.h"

#include <iostream>

#include <memory>

// TODO: If debug enabled then allow prints


void VaoAttributeInfo::set( GLuint bID, int sz, int offs, int strd, GLenum use ) {
    bufferID = bID;
    size = sz;
    offset = offs;
    stride = strd;
    usage = use;
}

VaoAttribute::VaoAttribute() :
    instanceDivisor( 0 ), attributeSize( 3 ), usageHint( GL_STATIC_DRAW )
{
}
VaoAttribute::VaoAttribute( int attSize, int divisor, GLenum usage ) :
    instanceDivisor( divisor ), attributeSize( attSize ), usageHint( usage )
{
}
void VaoAttribute::set( int size, int divisor, GLenum usage ) {
    attributeSize = size;
    instanceDivisor = divisor;
    usageHint = usage;
}
int VaoAttribute::getNumVerticesUsed() const {
    return attributeData.size() / attributeSize;
}
bool VaoAttribute::checkSizeConsistency() const {
    return ( attributeData.size() % attributeSize ) == 0;
}

VaoInitializer::VaoInitializer() :
    useIndices( true )
{

}
VaoInitializer::VaoInitializer( int numAttributes, bool useIndex ) :
    attributes( numAttributes, {} ), useIndices( useIndex )
{

}
bool VaoInitializer::checkSizeConsistency() const {
    if ( attributes.size() == 0 ) {
        //std::cout << "No attributes" << std::endl; return false;
    }
    if ( useIndices && indices.size() == 0 ) {
        //std::cout << "useIndices = true; but index array is empty." << std::endl;
        return false;
    }

    if ( !attributes[ 0 ].checkSizeConsistency() ) {
        return false;
    }

    int expectedVertices = attributes[ 0 ].getNumVerticesUsed();
    for ( int i = 1; i < attributes.size(); ++i ) {
        if ( !attributes[ i ].checkSizeConsistency() || ( attributes[ i ].getNumVerticesUsed() != expectedVertices ) ) {
            return false;
        }
    }

    // Debug version
    /*std::vector<int> counts( attributes.size(), 0 );
    std::vector<bool> checks( attributes.size(), true );
    for ( int i = 0; i < attributes.size(); ++i ) {
        checks[ i ] = attributes[ i ].checkSizeConsistency();
        counts[ i ] = attributes[ i ].getNumVerticesUsed();
    }

    std::cout << "VAO - Num Vertices per attribute( ";
    bool success = true;
    int count = counts[ 0 ];
    for ( int i = 0; i < attributes.size(); ++i ) {
        std::cout << i << "=";
        if ( !checks[ i ] ) {
            std::cout << "INC ";
            success = false;
        }
        else {
            std::cout << counts[ i ] << " ";
            if ( counts[ i ] != count ) {
                success = false;
            }
        }
    }
    std::cout << ") ";
    if ( !success ) {
        std::cout << "FAILED" << std::endl;
        return false;
    }
    std::cout << "OK" << std::endl;*/

    // check num vertices against primitive type

    return true;
}

Vao::Vao() :
    _vaoID( 0 ), _useIndex( false ), _numIndices( 0 ), _primitive( GL_TRIANGLES ), _isValidObject( false ),
    _staticBufferID( 0 ), _dynamicBufferID( 0 ), _indexBufferID( 0 )
{

}
Vao::~Vao() {
    destroy();
}

bool Vao::isCreated() {
    return _isValidObject;
}
GLuint Vao::getID() {
    return _vaoID;
}

void Vao::destroy() {
    if ( _isValidObject ) {
        if ( _staticBufferID != 0 ) {
            glDeleteBuffers( 1, &_staticBufferID );
        }
        if ( _dynamicBufferID != 0 ) {
            glDeleteBuffers( 1, &_dynamicBufferID );
        }
        if ( _indexBufferID != 0 ) {
            glDeleteBuffers( 1, &_indexBufferID );
        }

        glDeleteVertexArrays( 1, &_vaoID );

        _isValidObject =false;
        _vaoID = _staticBufferID = _dynamicBufferID = _indexBufferID = 0;
    }
}

bool Vao::createVao( const VaoInitializer& vInit ) {
    // Check if attributes are consistent
    if ( !vInit.checkSizeConsistency() ) {
        return false;
    }

    _attribInfo.resize( vInit.attributes.size() );

    glGenVertexArrays( 1, &_vaoID );
    glBindVertexArray( _vaoID );

    int numVertices = vInit.attributes[ 0 ].getNumVerticesUsed();

    // Determine which attributes are static and which are dynamic
    std::vector<int> staticAtts;
    std::vector<int> dynamicAtts;
    for ( int i = 0; i < vInit.attributes.size(); ++i ) {
        if ( vInit.attributes[ i ].usageHint == GL_STATIC_DRAW ) {
            staticAtts.push_back( i );
        }
        else /*if ( vInit.attributes[ i ].usageHint == GL_DYNAMIC_DRAW )*/ {
            dynamicAtts.push_back( i );
        }
    }

    // Create intermediary array with data interleaved
    if ( staticAtts.size() > 0 ) {
        // Calculate the total size (number of floats) to hold the vertex data
        int staticSize = 0;
        int stride = 0;
        for ( int i = 0; i < staticAtts.size(); ++i ) {
            staticSize += vInit.attributes[ staticAtts[ i ] ].attributeData.size();
            stride += vInit.attributes[ staticAtts[ i ] ].attributeSize;
        }
        std::unique_ptr<float[]> interleavedData( new float[ staticSize ] );

        int staticSizeBytes = staticSize * sizeof( float );

        unsigned int offset = 0;
        for ( int i = 0; i < staticAtts.size(); ++i ) {
            int attSize = vInit.attributes[ staticAtts[ i ] ].attributeSize;
            for ( int j = 0; j < numVertices * attSize; ++j ) {
                int vIndex = j / attSize;
                int vShift = j % attSize;
                interleavedData[ offset + vIndex * stride + vShift ] = vInit.attributes[ staticAtts[ i ] ].attributeData[ j ];
            }
            offset += attSize;
        }

        int strideBytes = stride * sizeof( float );

        glGenBuffers( 1, &_staticBufferID );
        glBindBuffer( GL_ARRAY_BUFFER, _staticBufferID );
        glBufferData( GL_ARRAY_BUFFER, staticSizeBytes, interleavedData.get(), GL_STATIC_DRAW );

        offset = 0;
        for ( int i = 0; i < staticAtts.size(); ++i ) {
            int attributeID = staticAtts[ i ];
            int attSize = vInit.attributes[ attributeID ].attributeSize;
            glEnableVertexAttribArray( attributeID );
            //std::cout << "( " << vInit.attributes[ attributeID ].name << " " << attributeID << ":    " << attSize << " " << strideBytes << " Offset: " << offset << " )" << std::endl;
            glVertexAttribPointer( attributeID, attSize, GL_FLOAT, GL_FALSE, strideBytes, (void*)offset );
            if ( vInit.attributes[ attributeID ].instanceDivisor > 0 ) {
                glVertexAttribDivisor( attributeID, vInit.attributes[ attributeID ].instanceDivisor );
            }

            _attribInfo[ attributeID ].set( _staticBufferID, attSize, offset, stride, GL_STATIC_DRAW );

            offset += (unsigned int)attSize * sizeof( float );
        }

        /*std::cout << "Data: ";
        for ( int i = 0; i < staticSize; ++i ) {
            std::cout << interleavedData[ i ] << " ";
        }
        std::cout << std::endl;*/
    }

    // Upload dynamic data into a single buffer, not interleaved
    if ( dynamicAtts.size() > 0 ) {
        // Calculate the total size (number of floats) to hold the vertex data
        int dynamicSize = 0;
        for ( int i = 0; i < dynamicAtts.size(); ++i ) {
            dynamicSize += vInit.attributes[ dynamicAtts[ i ] ].attributeData.size();
        }
        int dynamicSizeBytes = dynamicSize * sizeof( float );

        // Create buffer
        glGenBuffers( 1, &_dynamicBufferID );
        glBindBuffer( GL_ARRAY_BUFFER, _dynamicBufferID );
        glBufferData( GL_ARRAY_BUFFER, dynamicSizeBytes, nullptr, GL_DYNAMIC_DRAW );

        unsigned int offset = 0;

        // Upload each attribute's data
        for ( int i = 0; i < dynamicAtts.size(); ++i ) {
            int attributeID = dynamicAtts[ i ];
            int sizeBytes = vInit.attributes[ attributeID ].attributeData.size() * sizeof( float );
            glBufferSubData( GL_ARRAY_BUFFER, offset, sizeBytes, &vInit.attributes[ attributeID ].attributeData[ 0 ] );
            
            _attribInfo[ attributeID ].set( _dynamicBufferID,
                vInit.attributes[ attributeID ].attributeSize, offset, 0, GL_DYNAMIC_DRAW );

            offset += sizeBytes;
        }

        // Reuse offset
        offset = 0;

        // Enable attribute arrays
        for ( int i = 0; i < dynamicAtts.size(); ++i ) {
            int attributeID = dynamicAtts[ i ];
            int attSize = vInit.attributes[ attributeID ].attributeSize;
            int sizeBytes = vInit.attributes[ attributeID ].attributeData.size() * sizeof( float );

            glEnableVertexAttribArray( attributeID );
            glVertexAttribPointer( attributeID, attSize, GL_FLOAT, GL_FALSE, 0, (void*)offset );
            if ( vInit.attributes[ attributeID ].instanceDivisor > 0 ) {
                glVertexAttribDivisor( attributeID, vInit.attributes[ attributeID ].instanceDivisor );
            }

            // DEBUG
            //std::cout << "( " << vInit.attributes[ attributeID ].name << " " << attributeID << ":    " << attSize << " " << strideBytes << " Offset: " << offset << " )" << std::endl;

            offset += sizeBytes;
        }

        /*std::cout << "Data: ";
        for ( int i = 0; i < dynamicSize; ++i ) {
            std::cout << interleavedData[ i ] << " ";
        }
        std::cout << std::endl;*/
    }

    // Set Indices
    if ( vInit.useIndices ) {
        glGenBuffers( 1, &_indexBufferID );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _indexBufferID );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, vInit.indices.size() * sizeof( unsigned int ), &vInit.indices[0], GL_STATIC_DRAW );
        _useIndex = true;
        _numIndices = vInit.indices.size();
    }

    _primitive = vInit.primitiveType;
    
    glBindVertexArray( 0 );

    _isValidObject = true;

    return true;
}

void Vao::render() {
    glBindVertexArray( _vaoID );

    //if ( _useIndex ) {
    //std::cout << "Rendering: " << ( _primitive == GL_LINES ? "Lines" : "Other" ) << " " << _numIndices << std::endl;
        glDrawElements( _primitive, _numIndices, GL_UNSIGNED_INT, 0 );
    //}
}

bool Vao::modifyAttribute( int attributeID, const float* data, int vertices ) {
    unbind();

    glBindBuffer( GL_ARRAY_BUFFER, _attribInfo[ attributeID ].bufferID );
    glBufferSubData( GL_ARRAY_BUFFER, _attribInfo[ attributeID ].offset, _attribInfo[ attributeID ].size * vertices * sizeof( float ), data );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    return true;
}

void Vao::bind() {
    glBindVertexArray( _vaoID );
}

/*STATIC*/ void Vao::unbind() {
    glBindVertexArray( 0 );
}