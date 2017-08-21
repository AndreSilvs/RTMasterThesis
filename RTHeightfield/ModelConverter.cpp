#include "ModelConverter.h"

#include "Logging.h"

#include <iostream>

namespace ModelConverter {

    bool convertOBJtoVAO( const OBJModel& model, Vao *outData ) {
        if ( !model.isLoaded() ) { return false; }
        if ( outData->isCreated() ) { return false; }

        // Index checking
        const std::vector< float > &modelPositions = model.getVertices();
        const std::vector< float > &modelTexCoords = model.getTextureCoords();
        const std::vector< float > &modelNormals = model.getNormals();
        const std::vector< OBJFace > &modelFaces = model.getFaces();

        const int numVertices = modelFaces.size() * 3;

        VaoInitializer vObjData{ 3 };
        vObjData.primitiveType = GL_TRIANGLES;
        vObjData.useIndices = true;

        vObjData.indices.resize( numVertices );

        vObjData.attributes[ 0 ].name = "position";
        vObjData.attributes[ 0 ].attributeSize = 3;
        vObjData.attributes[ 0 ].instanceDivisor = 0;
        vObjData.attributes[ 0 ].usageHint = GL_STATIC_DRAW;
        vObjData.attributes[ 0 ].attributeData.resize( numVertices * 3 );

        vObjData.attributes[ 1 ].name = "normals";
        vObjData.attributes[ 1 ].attributeSize = 3;
        vObjData.attributes[ 1 ].instanceDivisor = 0;
        vObjData.attributes[ 1 ].usageHint = GL_STATIC_DRAW;
        vObjData.attributes[ 1 ].attributeData.resize( numVertices * 3 );

        vObjData.attributes[ 2 ].name = "texCoords";
        vObjData.attributes[ 2 ].attributeSize = 2;
        vObjData.attributes[ 2 ].instanceDivisor = 0;
        vObjData.attributes[ 2 ].usageHint = GL_STATIC_DRAW;
        vObjData.attributes[ 2 ].attributeData.resize( numVertices * 2 );

        for ( int i = 0, iEnd = modelFaces.size(); i < iEnd; ++i ) {
            vObjData.indices[ i * 3 + 0 ] = (unsigned int)( i * 3 + 0 );
            vObjData.indices[ i * 3 + 1 ] = (unsigned int)( i * 3 + 1 );
            vObjData.indices[ i * 3 + 2 ] = (unsigned int)( i * 3 + 2 );

            //v/t/n v/t/n v/t/n
            for ( int j = 0; j < 3; ++j ) {
                vObjData.attributes[ 0 ].attributeData[ i * 9 + 0 + j ] = modelPositions[ modelFaces[ i ].v1[ 0 ] * 3 + j ];
                vObjData.attributes[ 0 ].attributeData[ i * 9 + 3 + j ] = modelPositions[ modelFaces[ i ].v2[ 0 ] * 3 + j ];
                vObjData.attributes[ 0 ].attributeData[ i * 9 + 6 + j ] = modelPositions[ modelFaces[ i ].v3[ 0 ] * 3 + j ];
            }

            for ( int j = 0; j < 3; ++j ) {
                vObjData.attributes[ 1 ].attributeData[ i * 9 + 0 + j ] = modelNormals[ modelFaces[ i ].v1[ 2 ] * 3 + j ];
                vObjData.attributes[ 1 ].attributeData[ i * 9 + 3 + j ] = modelNormals[ modelFaces[ i ].v2[ 2 ] * 3 + j ];
                vObjData.attributes[ 1 ].attributeData[ i * 9 + 6 + j ] = modelNormals[ modelFaces[ i ].v3[ 2 ] * 3 + j ];
            }

            for ( int j = 0; j < 2; ++j ) {
                vObjData.attributes[ 2 ].attributeData[ i * 6 + 0 + j ] = modelTexCoords[ modelFaces[ i ].v1[ 1 ] * 2 + j ];
                vObjData.attributes[ 2 ].attributeData[ i * 6 + 2 + j ] = modelTexCoords[ modelFaces[ i ].v2[ 1 ] * 2 + j ];
                vObjData.attributes[ 2 ].attributeData[ i * 6 + 4 + j ] = modelTexCoords[ modelFaces[ i ].v3[ 1 ] * 2 + j ];
            }
        }

        return outData->createVao( vObjData );
    }

    bool convertOBJtoVAOinitializer( const OBJModel& model, VaoInitializer& vObjData ) {
        if ( !model.isLoaded() ) { return false; }

        // Index checking
        const std::vector< float > &modelPositions = model.getVertices();
        const std::vector< float > &modelTexCoords = model.getTextureCoords();
        const std::vector< float > &modelNormals = model.getNormals();
        const std::vector< OBJFace > &modelFaces = model.getFaces();

        const int numVertices = modelFaces.size() * 3;
        
        vObjData = VaoInitializer( 3 );
        vObjData.primitiveType = GL_TRIANGLES;
        vObjData.useIndices = true;

        vObjData.indices.resize( numVertices );

        //vObjData.attributes[ 0 ].name = "position";
        vObjData.attributes[ 0 ].attributeSize = 3;
        vObjData.attributes[ 0 ].instanceDivisor = 0;
        vObjData.attributes[ 0 ].usageHint = GL_STATIC_DRAW;
        vObjData.attributes[ 0 ].attributeData.resize( numVertices * 3 );

        //vObjData.attributes[ 1 ].name = "normals";
        vObjData.attributes[ 1 ].attributeSize = 3;
        vObjData.attributes[ 1 ].instanceDivisor = 0;
        vObjData.attributes[ 1 ].usageHint = GL_STATIC_DRAW;
        vObjData.attributes[ 1 ].attributeData.resize( numVertices * 3 );

        //vObjData.attributes[ 2 ].name = "texCoords";
        vObjData.attributes[ 2 ].attributeSize = 2;
        vObjData.attributes[ 2 ].instanceDivisor = 0;
        vObjData.attributes[ 2 ].usageHint = GL_STATIC_DRAW;
        vObjData.attributes[ 2 ].attributeData.resize( numVertices * 2 );

        for ( int i = 0, iEnd = modelFaces.size(); i < iEnd; ++i ) {
            vObjData.indices[ i * 3 + 0 ] = (unsigned int)( i * 3 + 0 );
            vObjData.indices[ i * 3 + 1 ] = (unsigned int)( i * 3 + 1 );
            vObjData.indices[ i * 3 + 2 ] = (unsigned int)( i * 3 + 2 );

            //v/t/n v/t/n v/t/n
            for ( int j = 0; j < 3; ++j ) {
                vObjData.attributes[ 0 ].attributeData[ i * 9 + 0 + j ] = modelPositions[ modelFaces[ i ].v1[ 0 ] * 3 + j ];
                vObjData.attributes[ 0 ].attributeData[ i * 9 + 3 + j ] = modelPositions[ modelFaces[ i ].v2[ 0 ] * 3 + j ];
                vObjData.attributes[ 0 ].attributeData[ i * 9 + 6 + j ] = modelPositions[ modelFaces[ i ].v3[ 0 ] * 3 + j ];
            }

            for ( int j = 0; j < 3; ++j ) {
                vObjData.attributes[ 1 ].attributeData[ i * 9 + 0 + j ] = modelNormals[ modelFaces[ i ].v1[ 2 ] * 3 + j ];
                vObjData.attributes[ 1 ].attributeData[ i * 9 + 3 + j ] = modelNormals[ modelFaces[ i ].v2[ 2 ] * 3 + j ];
                vObjData.attributes[ 1 ].attributeData[ i * 9 + 6 + j ] = modelNormals[ modelFaces[ i ].v3[ 2 ] * 3 + j ];
            }

            for ( int j = 0; j < 2; ++j ) {
                vObjData.attributes[ 2 ].attributeData[ i * 6 + 0 + j ] = modelTexCoords[ modelFaces[ i ].v1[ 1 ] * 2 + j ];
                vObjData.attributes[ 2 ].attributeData[ i * 6 + 2 + j ] = modelTexCoords[ modelFaces[ i ].v2[ 1 ] * 2 + j ];
                vObjData.attributes[ 2 ].attributeData[ i * 6 + 4 + j ] = modelTexCoords[ modelFaces[ i ].v3[ 1 ] * 2 + j ];
            }
        }

        return true;
    }
}