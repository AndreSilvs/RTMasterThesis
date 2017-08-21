#pragma once

#include "API_Headers.h"

#include <string>
#include <vector>

struct VaoAttributeInfo {
    GLuint bufferID;
    int size;
    int offset;
    int stride;
    GLenum usage;

    void set( GLuint bufferID, int size, int offset, int stride, GLenum usage );
};

struct VaoAttribute {
    std::vector<float> attributeData;
    std::string name;
    int attributeSize;
    int instanceDivisor;
    GLenum usageHint;

    int getNumVerticesUsed() const;
    bool checkSizeConsistency() const;

    VaoAttribute();
    VaoAttribute( int attSize, int divisor, GLenum usage );

    void set( int size, int divisor, GLenum usage );
};

struct VaoInitializer {
    std::vector< VaoAttribute > attributes;
    std::vector< unsigned int > indices;
    bool useIndices;
    GLenum primitiveType;

    bool checkSizeConsistency() const;

    VaoInitializer();
    VaoInitializer( int numAttributes, bool useIndex = true );
};

// Wrapper class for Vertex-Array-Object operations
// Destructor automatically clears VAO buffers, as such no copies are allowed
class Vao {
public:
    Vao();    
    Vao( const Vao& v ) = delete;
    ~Vao();

    bool isCreated();

    bool createVao( const VaoInitializer& vInit );
    void destroy();
    void render();
    void bind();
    static void unbind();

    // Modify the attribute with index #attributeID assuming it has dynamic draw usage.
    // Attempting to modify a static atribute will corrupt other static data.
    bool modifyAttribute( int attributeID, const float* data, int vertices );

    GLuint getID();

    Vao& operator=( const Vao& v ) = delete;

private:
    GLuint _vaoID;
    GLenum _primitive;

    GLuint _staticBufferID;
    GLuint _dynamicBufferID;
    GLuint _indexBufferID;

    bool _useIndex;
    int _numIndices;

    bool _isValidObject;

    std::vector< VaoAttributeInfo > _attribInfo;
};