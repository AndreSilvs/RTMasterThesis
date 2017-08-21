#include "Font.h"

/** GLOBAL	HEADERS	**/
#include FT_BITMAP_H

/** LOCAL	HEADERS	**/
#include "Bits.h"
#include "MathCalcs.h"
#include "MatrixCalcs.h"
#include "PixelArray.h"
#include "PixelManip.h"

#include "SystemErrors.h"
#include "Logging.h"

/** FORWARD DECLARATIONS **/

/** NOTES **/

namespace {
    const Color4C PIXEL_BLACK = Color4C( 0, 0, 0, 255 );
    const Color4C PIXEL_TRANSPARENT = Color4C( 0, 0, 0, 0 );

    GLint sVertexAttribute = -1;
    GLint sTextureAttribute = -1;

    GLint sPvmMatrix;
    GLint sTextureUnit;
    GLint sColorMultiplier;

    Matrix4D pvm;
    Matrix4D pv;

    GLuint fontVao;
    GLuint fontVbos[ 2 ]; // Position & Texture, Index
    GLuint vertexOffset = 0;
    GLuint textureOffset = 0;
}

FT_Library ttfLib;

bool initializeTTF() {
	FT_Error error = FT_Init_FreeType( &ttfLib );
	if( error ) {
        Logging::error.reportError( error, "FreeType: Failed to initialize FreeType lib" );
        return false;
    }
	return true;
}

ShaderProgram Font::shader;
MatrixStack Font::projection;
MatrixStack Font::view;
MatrixStack Font::model;
bool Font::isFontSystemLoaded = false;

void Font::setFontColor( Color4C color ) {
    glUniform4f( sColorMultiplier, (float)color.red / 255.f, (float)color.green / 255.f, (float)color.blue / 255.f, (float)color.alpha / 255.f );
}

void Font::setMatrixes() {
    pv = view.getMatrix() * projection.getMatrix();
    pvm = model.getMatrix() * pv;
    glUniformMatrix4fv( sPvmMatrix, 1, GL_FALSE, pvm.getMatrixAsArray() );
}
void Font::setModelMatrix() {
    pvm = model.getMatrix() * pv;
    glUniformMatrix4fv( sPvmMatrix, 1, GL_FALSE, pvm.getMatrixAsArray() );
}

bool Font::initializeFontSystem() {
    if ( !initializeTTF() ) {
        Logging::error.reportWarning( "FreeType Lib did not initialize." );
        return false;
    }

    if ( !shader.compileProgram( "Shaders/Font2DVS.vert", "Shaders/Font2DFS.frag" ) ) {
        Logging::error.reportWarning( 0, "Error compiling Shader for Font System." );
        return false;
    }

    bool success = true;
    if ( !shader.readAttributeLocation( "inVertex", sVertexAttribute ) ) { success = false; }
    if ( !shader.readAttributeLocation( "inTexture", sTextureAttribute ) ) { success = false; }

    if ( !shader.readUniformLocation( "pvmMatrix", sPvmMatrix ) ) { success = false; }

    if ( !shader.readUniformLocation( "texture0", sTextureUnit ) ) { success = false; }
    if ( !shader.readUniformLocation( "colorMultiplier", sColorMultiplier ) ) { success = false; }

    if ( !success ) {
        /*auto unif = shader.getUniformList();
        for ( auto name : unif ) {
        std::cout << name << std::endl;
        }
        getchar();*/
        return false;
    }

    if ( !createQuad() ) {
        Logging::error.reportError( 0, "Error creating VAO for Font System." );
        return false;
    }

    shader.useProgram();

    setFontColor( Color4C( 255, 255, 255, 255 ) );
    projection.getMatrix().setIdentity();
    view.getMatrix().setIdentity();
    model.getMatrix().setIdentity();

    setMatrixes();

    shader.disableProgram();

    isFontSystemLoaded = true;
    return true;
}

void Font::closeFontSystem() {
    if ( isFontSystemLoaded ) {
        isFontSystemLoaded = false;
        glDeleteVertexArrays( 1, &::fontVao );
        glDeleteVertexArrays( 2, ::fontVbos );
    }
}

bool Font::createQuad() {
    float vertexData[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    float quadTexCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    //IBO data
    GLuint indexData[] = { 0, 1, 2, 0, 2, 3 };

    GLuint vao = 0;
    GLuint vbos[ 2 ] = { 0, 0 };

    glGenVertexArrays( 1, &vao ); // Generate VAO handler
    glBindVertexArray( vao ); // Bind the VAO

    unsigned int vertexSize = sizeof( vertexData );
    unsigned int texCoordSize = sizeof( quadTexCoords );
    ::vertexOffset = 0;
    ::textureOffset = vertexSize;

    glGenBuffers( 2, vbos ); // Generate two VBOs, one for data and the other for indices

    // Vertex Positions
    glBindBuffer( GL_ARRAY_BUFFER, vbos[ 0 ] );
    glBufferData( GL_ARRAY_BUFFER, vertexSize + texCoordSize, NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, vertexSize, vertexData );
    glBufferSubData( GL_ARRAY_BUFFER, vertexSize, texCoordSize, quadTexCoords );

    glEnableVertexAttribArray( sVertexAttribute );
    glVertexAttribPointer( sVertexAttribute, 2, GL_FLOAT, 0, 0, 0 );

    glEnableVertexAttribArray( sTextureAttribute );
    glVertexAttribPointer( sTextureAttribute, 2, GL_FLOAT, 0, 0, (const void*)( vertexSize ) );

    // Indices
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbos[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indexData ), indexData, GL_STATIC_DRAW );

    if ( getOpenGLError() ) {
        glDeleteVertexArrays( 1, &vao );
        glDeleteVertexArrays( 2, vbos );

        isFontSystemLoaded = false;
        return false;
    }

    /*VAOData2D data;
    data.vertexAttribute = sVertexAttribute;
    data.textureAttribute = sTextureAttribute;

    data.vertices.resize( 8 );
    data.texCoords.resize( 8 );
    for ( int i = 0; i < 8; ++i ) {
        data.vertices[ i ] = vertexData[ i ];
        data.texCoords[ i ] = quadTexCoords[ i ];
    }

    data.indices.resize( 6 );
    for ( int i = 0; i < 6; ++i ) {
        data.indices[ i ] = indexData[ i ];
    }*/

    //vaoQuad.loadVAO( data );

    isFontSystemLoaded = true;

    ::fontVao = vao;
    ::fontVbos[ 0 ] = vbos[ 0 ];
    ::fontVbos[ 1 ] = vbos[ 1 ];

    return true;
}

void Font::uploadQuadData( float *position, float *texCoords ) {
    glBufferSubData( GL_ARRAY_BUFFER, vertexOffset, 8 * sizeof( float ), position );
    glBufferSubData( GL_ARRAY_BUFFER, textureOffset, 8 * sizeof( float ), texCoords );
}

// ============================================================================
// ============================================================================
// Bitmap Font
Font::Font() :
    _isLoaded( 0 )
{

}

Font::~Font() {
	freeFont();
}

bool Font::isLoaded() const {
    return _isLoaded;
}

void Font::freeFont() {
	if( isLoaded() ) {
        _fontTexture.closeTexture();

		_fontWidth = 0;
		_fontHeight = 0;
		_maxCharWidth = 0;
		_maxCharHeight = 0;
		_spaceBetweenChars = 0;
		_spaceBetweenLines = 0;
		_spaceWidth = 0;
		_tabWidth = 0;
	}
}

bool Font::loadTTF( std::string filename, int fontSize ) {
	//Get cell dimensions
    GLuint cellW = 0;
    GLuint cellH = 0;
    int maxBearing = 0;
    int minHang = 0;

    //Character data
	PixelArray< GLubyte > pixels[ 256 ];
    FT_Glyph_Metrics metrics[ 256 ];
    
	//Load face
    FT_Face face = NULL;
    FT_Error error = FT_New_Face( ttfLib, filename.c_str(), 0, &face );
    if( !error )  {
		//Set face size
        error = FT_Set_Pixel_Sizes( face, 0, fontSize );
        if( !error ) {
			//Go through extended ASCII to get glyph data
            for( int i = 0; i < 256; ++i ) {
                //Load and render glyph
                error = FT_Load_Char( face, i, FT_LOAD_RENDER );
                if( !error ) {
					//Get metrics
                    metrics[ i ] = face->glyph->metrics;

                    _horizontalAdvancement[ i ] = metrics[ i ].horiAdvance / 64;

                    _bearingX[ i ] = metrics[ i ].horiBearingX / 64;

                    //Copy glyph bitmap
                    pixels[ i ].copyPixelArray( face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows );

					//Calculate max bearing
                    if( metrics[ i ].horiBearingY / 64 > maxBearing ) {
                        maxBearing = metrics[ i ].horiBearingY / 64;
                    }

                    //Calculate max width
                    if( metrics[ i ].width / 64 > cellW ) {
                        cellW = metrics[ i ].width / 64;
                    }

                    //Calculate gylph hang
                    int glyphHang = ( metrics[ i ].horiBearingY - metrics[ i ].height ) / 64;
                    if( glyphHang < minHang ) {
                        minHang = glyphHang;
                    }
                }
				else {
                    Logging::error.reportError( "FreeType: Unable to load glyph [" + std::to_string( i ) + "]  " );
                    return false;
                }
            }

			//Create bitmap font
            cellH = maxBearing - minHang;
            PixelArray< GLubyte > fontPixels;
			int width = cellW * 16;
			int height = cellH * 16;
			fontPixels.createPixelArray( width, height );

            //Begin creating bitmap font
            GLuint currentChar = 0;
            //Clip nextClip { 0.f, 0.f, cellW, cellH };

            //Blitting coordinates
            int bX = 0;
            int bY = 0;

			//Go through cell rows
            for( unsigned int rows = 0; rows < 16; rows++ ) {
                //Go through each cell column in the row
                for( unsigned int cols = 0; cols < 16; cols++ ) {
                    //Set base offsets
                    bX = cellW * cols;
                    bY = cellH * rows;

                    //Initialize clip
					_charactersWidth[ currentChar ] = 1 + metrics[ currentChar ].width / 64;

					_clips[ currentChar ].left = (float)bX / (float)width;
					_clips[ currentChar ].top = 1.0f - (float)bY / (float)height;
					_clips[ currentChar ].right = ( 1 + bX + metrics[ currentChar ].width / 64 ) / (float)width;
					_clips[ currentChar ].bottom = 1.0f - ( bY + cellH ) / (float)height;
					int halfWidth = _charactersWidth[ currentChar ] / 2;

					/*_clips[ currentChar ].left = bX / (float)width;
					_clips[ currentChar ].top = 1.0f - (float)bY / (float)height;
					_clips[ currentChar ].right = ( bX + )  / (float)width;
					_clips[ currentChar ].bottom = 1.0f - ( bY + cellH ) / (float)height;*/

					//Blit character
                    fontPixels.overwritePixelArray( pixels[ currentChar ], bX, bY + maxBearing - metrics[ currentChar ].horiBearingY / 64 );

                    //Go to the next character
                    currentChar++;
                }
            }

			//Make texture power of two
			//fontPixels pad to power of two

			invertImageY(fontPixels.getPixels(), fontPixels.getWidth(), fontPixels.getHeight());

			//Generate texture
            PixelArray< Color4C > optimizedPixels;
            optimizedPixels.createPixelArrayEmpty( fontPixels.getWidth(), fontPixels.getHeight() );
            for ( int i = 0; i < optimizedPixels.getSize(); ++i ) {
                optimizedPixels[ i ] = Color4C( 255, 255, 255, fontPixels[ i ] );
            }

            _fontTexture.loadTexture( optimizedPixels.getPixels(), optimizedPixels.getWidth(), optimizedPixels.getHeight() );

            //Set spacing variables
			_fontWidth = cellW;
			_fontHeight = fontSize;

			_maxCharWidth = cellW;
			_maxCharHeight = cellH;
            //mNewLine = maxBearing;
			
			_spaceBetweenChars = 0.0f; // Default spacing
			_spaceBetweenLines = (float)( _fontHeight - maxBearing ); // totalSpacingY = _maxCharHeight + _spaceBetweenLines = _fontHeight
			
			_spaceWidth = ((float)_maxCharWidth)/2.0f;
			_tabWidth = (float)( _maxCharWidth * 4 );
			_halfHeight = (float)_maxCharHeight/2.0f;

        }
        else {
            Logging::error.reportError( error, "FreeType: Unable to set font size." );
            return false;
        }

        //Free face
        FT_Done_Face( face );
    }
	else {
        Logging::error.reportError( error, "FreeType: Unable to load load font face." );
        return false;
    }

	return true;
}

bool Font::loadBitmap( const std::string &fileName ) {
    if ( isLoaded() ) {
        Logging::error.reportWarning( 0, "Another font is already loaded. Cannot load new font without closing current one." );
        return false;
    }

    if ( !_fontTexture.loadTexture( fileName ) ) {
        Logging::error.reportWarning( 0, "Failed to load bitmap font texture." );
        return false;
    }

    _fontTexture.downloadPixels();

    Color4C *pixels = _fontTexture.getPixels();
    int width = _fontTexture.getWidth();
    int height = _fontTexture.getHeight();

	// Initialize font data
	float cellSizeX = (float)width / 16.0f; // 16 = number of characters per row
	float cellSizeY = (float)height / 16.0f; // 16 = number of characters per column

    _maxCharHeight = cellSizeY;

	_fontWidth = (int)cellSizeX;
	_fontHeight = (int)cellSizeY;

	_spaceBetweenChars = 1.0f; // Default spacing
	_spaceBetweenLines = (float)( _fontHeight-_maxCharHeight ); // totalSpacingY = _maxCharHeight + _spaceBetweenLines = _fontHeight

	int nCharsLine = 16;

	// Iterate each character
	for( int i = 0 ; i < 256 ; ++i ) {
		// Initializations
		int leftLimit = i%nCharsLine;
        int bottomLimit = ( 15 - i / nCharsLine ); // Pixel origin is the lower-left corner
        int startIndex = leftLimit * (int)cellSizeX + bottomLimit * (int)cellSizeX * (int)width;
		int offsetLeft = 0;
		int offsetRight = 0;
		_clips[i].right = 0;
		_clips[i].left = 0;

		// Find left bound
		for ( int x = 0 ; x < (int)cellSizeX; ++x ) {
			for ( int y = 0; y < (int)cellSizeY; ++y ) {
                if ( pixels[ startIndex + x + y*(int)width ] != PIXEL_BLACK ) {
                    _clips[ i ].left = ( (float)leftLimit*cellSizeX + (float)x ) / (float)width;
					offsetLeft = x;

					// Break (horrible)
                    x = (int)cellSizeX + 1;
                    y = (int)cellSizeY + 1;
				}
			}
		}

		// Find right bound
		for ( int x = (int)cellSizeX-1 ; x >= 0 ; --x ) {
			for ( int y = 0; y < (int)cellSizeY ; ++y ) {
                if ( pixels[ startIndex + x + y*(int)width ] != PIXEL_BLACK ) {
                    _clips[ i ].right = ( (float)leftLimit*cellSizeX + (float)( x + 1 ) ) / (float)width;
                    offsetRight = x + 1;

					// Break (horrible)
					x = -1;
					y = (int)cellSizeY+1;
				}
			}
		}

		// Assume height is the max cellsize
        _clips[ i ].top = ( bottomLimit + 1 ) * cellSizeY / (float)height;
        _clips[ i ].bottom = ( bottomLimit * cellSizeY ) / (float)height;

		// Set character width
        _charactersWidth[ i ] = offsetRight - offsetLeft;
        _horizontalAdvancement[ i ] = _charactersWidth[ i ];
        _bearingX[ i ] = 0; // Bitmap has no bearing info
	}
	// End parsing bitmap

	// Default spacing
	_maxCharWidth = (int)cellSizeX;
	_maxCharHeight = (int)cellSizeY;
	_spaceWidth = ((float)_maxCharWidth)/2.0f;
	_tabWidth = (float)(_maxCharWidth*4);
	_halfHeight = (float)_maxCharHeight/2.0f;

	// Set black bits to transparent
	int pixel_size = width * height;
	for( int i = 0; i < pixel_size; ++i ) {
        if ( pixels[ i ] == PIXEL_BLACK ) {
            pixels[ i ] = PIXEL_TRANSPARENT;
        }
	}

    _fontTexture.uploadPixels();

    return true;
}

float Font::getTextLength( const std::string &text ) const {
	float length = 0;
	for ( unsigned int i = 0 ; i < text.size() ; ++i ) {
		if( text[i] == ' ') {
			length += _spaceWidth;
		} else {
			if ( text[i] == '\t' ) {
				float tabSpace = mathCalcs::nextMultiple(length, _tabWidth) - length;
				length += tabSpace;
			} else {
				if(text[i] == '\n') {
					break;
				} else {
					length += _charactersWidth[text[i]] + _spaceBetweenChars;
				}
			}
		}
	}
	return length;
}

int Font::getTextNewlines( const std::string &text, float maxlength ) const {
	int newlines = 0;
	float linelength = 0.0f;

	if( maxlength != 0.0f ) { // MaxLength was specified
		for ( unsigned int i = 0 ; i < text.size() ; ++i ) {
			if(linelength> (maxlength-_maxCharWidth)) {
				linelength = 0.0f;
				newlines++;
			}

			switch( text[i] ) {
				case '\n' : {
					linelength = 0.0f;
					newlines++;
					break;
				}
				case ' ' : {
					linelength += _spaceWidth;
					break;
				}
				case '\t' : {
					linelength += mathCalcs::nextMultiple(linelength, _tabWidth) - linelength;
					newlines++;
					break;
				}
				default : {
					linelength += _charactersWidth[i] + _spaceBetweenChars;
					break;
				}
			}
		}
		
	} else { // MaxLength was not specified
		for ( unsigned int i = 0 ; i < text.size() ; ++i ) {
			newlines += (text[i] == '\n' ? 1 : 0);
		}
	}
	return newlines;
}

float Font::getTextHeight( const std::string &text , float maxlength ) const {
	return (float)getTextNewlines(text,maxlength)*(_maxCharHeight+_spaceBetweenLines);
}

Vector2D Font::getTextOffset( const std::string &text , float maxlength ) const {
	float length = 0;
	int newlines = 0;

	for ( unsigned int i = 0 ; i < text.size() ; ++i ) {
		if(maxlength != 0.0f && length> (maxlength-_maxCharWidth)) {
			length = 0.0f;
			newlines++;
		}

		if( text[i] == ' ') {
			length += _spaceWidth;
		} else {
			if ( text[i] == '\t' ) {
				float tabSpace = mathCalcs::nextMultiple(length, _tabWidth) - length;
				length += tabSpace;
			} else {
				if( text[i] == '\n' ) {
					length = 0.0f;
					newlines++;
				} else {
					length += (float)_charactersWidth[text[i]] + _spaceBetweenChars;
				}
			}
		}
	}

	return Vector2D( length, -(float)newlines*(_maxCharHeight+_spaceBetweenLines) );
}

std::string Font::getTextFromLine( const std::string &text, int line, float maxlength ) const {
	if( line == 0 ) return text;

	float length = 0;
	int newlines = 0;

	unsigned int i = 0; // cicle iterator
	unsigned int pos = 0; // line first character position

	while (newlines < line && i < text.size()) {
		if (maxlength != 0.0f && length> (maxlength-_maxCharWidth)) {
			length = 0.0f;
			newlines++;
			pos = i;
		} else {

			if( text[i] == ' ') {
				length += _spaceWidth;
			}
			else if ( text[i] == '\t' ) {
				float tabSpace = mathCalcs::nextMultiple(length, _tabWidth) - length;
				length += tabSpace;
			}
			else if ( text[i] == '\n' ) {
				length = 0.0f;
				newlines++;
				pos = i + 1;
			}
			else {
				length += (float)_charactersWidth[text[i]] + _spaceBetweenChars;
			}

			++i;
		}
	}

	if ( pos >= text.size() || i >= text.size() ) return "";

	return text.substr(pos);
}

std::string Font::getFirstLine( const std::string &text, float maxlength ) const {
	float length = 0;
	int newlines = 0;

	unsigned int i = 0; // cicle iterator

	while (( newlines < 1 ) && ( i < text.size() )) {
		if (( maxlength != 0.0f ) && ( length > ( maxlength - _maxCharWidth ) )) {
			length = 0.0f;
			newlines++;
		} else {
			int character = (unsigned char)text[i];
			
			if( character == ' ' ) {
				length += _spaceWidth;
			}
			else if ( character == '\t' ) {
				float tabSpace = mathCalcs::nextMultiple( length, _tabWidth ) - length;
				length += tabSpace;
			}
			else if ( character == '\n' ) {
				length = 0.0f;
				newlines++;
			}
			else {
				length += (float)_charactersWidth[ character ] + _spaceBetweenChars;
			}

			++i;
		}
	}

	if ( i >= text.size() ) return text;

	return text.substr( 0, i + 1 );
}

int Font::getFontWidth() const {
	return _fontWidth;
}
int Font::getFontHeight() const {
	return _fontHeight;
}
float Font::getSpaceBetweenChars() const {
	return _spaceBetweenChars;
}
float Font::getSpaceBetweenLines() const {
	return _spaceBetweenLines;
}
int Font::getCharWidth(unsigned int c) const {
	return _charactersWidth[c];
}
int Font::getCharMaxWidth() const {
	return _maxCharWidth;
}
int Font::getCharMaxHeight() const {
	return _maxCharHeight;
}

void Font::renderTextLeft( const std::string &text, float maxlength, float startlength) const {
    _fontTexture.bindTexture();

	float linelength = startlength;

    glDisable( GL_DEPTH_TEST );

    //GLboolean depthMask = GL_TRUE;
    //glGetBooleanv( GL_DEPTH_WRITEMASK, &depthMask );

    Matrix4D &modelMatrix = Font::model.getMatrix();

    Font::model.pushMatrix();

    glBindVertexArray( ::fontVao );

    glBindBuffer( GL_ARRAY_BUFFER, ::fontVbos[ 0 ] );

    float positions[ 8 ];
    float texCoords[ 8 ];
		
		for ( unsigned int i = 0 ; i < text.size(); ++i ) {
			int c = text[i];

            if ( maxlength != 0.0f && linelength > ( maxlength - _maxCharWidth ) ) {
                translateMatrix( modelMatrix, -linelength, -_maxCharHeight - _spaceBetweenLines, 0.0f );
				linelength = 0.0f;
			}

			switch( c ) {
				case ' ' : {
                    translateMatrix( modelMatrix, _spaceWidth, 0.0f, 0.0f );
					linelength += _spaceWidth;
					break;
				}
				case '\t' : {
                    float tabSpace = mathCalcs::nextMultiple( linelength, _tabWidth ) - linelength;
                    translateMatrix( modelMatrix, tabSpace, 0.0f, 0.0f );
					linelength += tabSpace;
					break;
				}
				case '\n' : {
                    translateMatrix( modelMatrix, -linelength, -_maxCharHeight - _spaceBetweenLines, 0.0f );
					linelength = 0.0f;
					break;
				}
				default : {
                    float charHalfWidth = _charactersWidth[ c ] / 2.0f;
                    float charHalfAdvancement = _horizontalAdvancement[ c ] / 2.0f;

                    translateMatrix( modelMatrix, _bearingX[ c ], 0.0f, 0.0f );

                    positions[ 0 ] = 0;                     positions[ 1 ] = -_halfHeight;
                    positions[ 2 ] = _charactersWidth[ c ]; positions[ 3 ] = -_halfHeight;
                    positions[ 4 ] = _charactersWidth[ c ]; positions[ 5 ] = _halfHeight;
                    positions[ 6 ] = 0;                     positions[ 7 ] = _halfHeight;

                    texCoords[ 0 ] = _clips[ c ].left; texCoords[ 1 ] = _clips[ c ].bottom;
                    texCoords[ 2 ] = _clips[ c ].right; texCoords[ 3 ] = _clips[ c ].bottom;
                    texCoords[ 4 ] = _clips[ c ].right; texCoords[ 5 ] = _clips[ c ].top;
                    texCoords[ 6 ] = _clips[ c ].left; texCoords[ 7 ] = _clips[ c ].top;

                    uploadQuadData( positions, texCoords );
                    setModelMatrix();

                    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
		
                    translateMatrix( modelMatrix, -_bearingX[ c ] + _horizontalAdvancement[ c ] + _spaceBetweenChars, 0.0f, 0.0f );

					linelength += _horizontalAdvancement[ c ] + _spaceBetweenChars;
				}
			}	
		}

    Font::model.popMatrix();
    //glDepthMask( depthMask );
    glEnable( GL_DEPTH_TEST );
    _fontTexture.unbindTexture();
}

void Font::renderTextRight( const std::string &text, float maxlength ) const {
	/*if( maxlength != 0.0f && maxlength < _maxCharWidth ) return;

    _fontTexture.bindTexture();

	float linelength = 0.0f;

	unsigned int first = 0;
	unsigned int next = 0;

	glPushMatrix();
		
		for ( unsigned int i = 0 ; i < text.size(); ++i ) {
			next = i;
			int character = (unsigned char)text[i];

			if (( maxlength != 0.0f ) && ( linelength > ( maxlength - _maxCharWidth ))) {
				glPushMatrix();
					glTranslatef( -linelength, 0.0f, 0.0f );
					renderTextLeft( text.substr( first, next - first ), maxlength );
				glPopMatrix();

				glTranslatef( 0.0f, -_maxCharHeight - _spaceBetweenLines, 0.0f );
				linelength = 0.0f;
				first = i;
			}

			if ( character == ' ' ) {
				linelength += _spaceWidth;
			}
			else if ( character == '\t' ) {
				float tabSpace = mathCalcs::nextMultiple( linelength, _tabWidth ) - linelength;
				linelength += tabSpace;
			}
			else if ( character == '\n' ) {
				glPushMatrix();
					glTranslatef( -linelength, 0.0f, 0.0f );
					renderTextLeft( text.substr( first, next - first ), maxlength );
				glPopMatrix();

				glTranslatef( 0.0f, -_maxCharHeight - _spaceBetweenLines, 0.0f );
				linelength = 0.0f;
				first = i+1;
			}
			else {
				float halfspacex = _charactersWidth[ character ]/2.0f;
				linelength += halfspacex*2 + _spaceBetweenChars;
			}	
		}
		
		glTranslatef( -linelength, 0.0f, 0.0f );
		renderTextLeft( text.substr( first, next - first + 1 ) );

	glPopMatrix();

    _fontTexture.bindTexture();*/
}

void Font::renderTextCentered( const std::string &text, float maxlength) const {
	if( maxlength != 0.0f && maxlength < _maxCharWidth ) return;

    _fontTexture.bindTexture();

	float linelength = 0.0f;

	unsigned int first = 0;
	unsigned int next = 0;

    Matrix4D &modelMatrix = Font::model.getMatrix();

	Font::model.pushMatrix();
		
		for ( unsigned int i = 0 ; i < text.size(); ++i ) {
			next = i;
			int c = text[i];

			if(maxlength != 0.0f && linelength> (maxlength-_maxCharWidth)) {
				model.pushMatrix();
                    translateMatrix( modelMatrix, -linelength/2.0f, 0.0f, 0.0f);
					renderTextLeft(text.substr(first,next-first), maxlength);
				model.popMatrix();

                translateMatrix( modelMatrix, 0.0f, -_maxCharHeight-_spaceBetweenLines, 0.0f);
				linelength = 0.0f;
				first = i;
			}

			switch(c) {
				case ' ' : {
					linelength += _spaceWidth;
					break;
				}
				case '\t' : {
					float tabSpace = mathCalcs::nextMultiple(linelength, _tabWidth) - linelength;
					linelength += tabSpace;
					break;
				}
				case '\n' : {
                    model.pushMatrix();
                        translateMatrix( model.getMatrix(), -linelength / 2.0f, 0.0f, 0.0f );
						renderTextLeft(text.substr(first,next-first), maxlength);
                    model.popMatrix();

                    translateMatrix( modelMatrix, 0.0f, -_maxCharHeight-_spaceBetweenLines, 0.0f);
					linelength = 0.0f;
					first = i+1;
					break;
				}
				default : {
                    linelength += _horizontalAdvancement[ c ] + _spaceBetweenChars;
					//float halfspacex = _charactersWidth[c]/2.0f;
					//linelength += halfspacex*2 + _spaceBetweenChars;
				}
			}	
		}
        translateMatrix( modelMatrix, -linelength / 2.0f, 0.0f, 0.0f );
		renderTextLeft(text.substr(first,next - first + 1));

    Font::model.popMatrix();

    _fontTexture.unbindTexture();
}

void Font::renderTextCircle( const std::string &text, float radius, float angle ) {
    _fontTexture.bindTexture();

	float perimeter = 2 * radius * (float)CALC_PI;

	float len = getTextLength( text );

	float currentAngle = mathCalcs::loopInRange( angle + 180.f * ( len / perimeter ), 0, 360 );

	for ( unsigned int i = 0 ; i < text.size(); ++i ) {
		int c = text[i];

		switch(c) {
			case ' ' : {
				currentAngle = mathCalcs::loopInRange( currentAngle - 360.f * ( _spaceWidth / perimeter ), 0, 360 );
				//glTranslatef( _spaceWidth , 0.0f, 0.0f);
				break;
			}
			/*case '\t' : {
				float tabSpace = nextMultiple(linelength, _tabWidth) - linelength;
				glTranslatef( tabSpace , 0.0f, 0.0f);
				linelength += tabSpace;
				break;
			}*/
			/*case '\n' : {
				glTranslatef(-linelength, -_maxCharHeight-_spaceBetweenLines, 0.0f);
				linelength = 0.0f;
				break;
			}*/
			default : {
				/*float halfspacex = _charactersWidth[c]/2.0f;

				glPushMatrix();

					//glTranslatef( halfspacex, 0.0f, 0.0f);
					currentAngle = mathCalcs::loopInRange( currentAngle - 360.f * ( halfspacex / perimeter ), 0, 360 );

					glRotatef( currentAngle, 0, 0, 1 );
					glTranslatef( 0, radius, 0 );

					//renderQuad( _charactersWidth[ c ], _halfHeight * 2 );
					glBegin(GL_QUADS);                          
						glTexCoord2d( _clips[c].left,_clips[c].bottom );	glVertex2f( -halfspacex, -_halfHeight);
						glTexCoord2d( _clips[c].right,_clips[c].bottom );	glVertex2f( halfspacex, -_halfHeight);
						glTexCoord2d( _clips[c].right,_clips[c].top );		glVertex2f( halfspacex, _halfHeight);
						glTexCoord2d( _clips[c].left,_clips[c].top );		glVertex2f( -halfspacex, _halfHeight);
					glEnd();
		
					//glTranslatef( halfspacex + _spaceBetweenChars, 0.0f, 0.0f );
					currentAngle = mathCalcs::loopInRange( currentAngle - 360.f * ( ( halfspacex + _spaceBetweenChars ) / perimeter ), 0, 360 );

				glPopMatrix();*/
			}
		}	
	}

    _fontTexture.unbindTexture();
}

void Font::setSpaceBetweenChars(float space) {
	_spaceBetweenChars = space;
}

void Font::setSpaceBetweenLines(float space) {
	_spaceBetweenLines = space;
}

void Font::setMinMagFilter(GLenum minFilter, GLenum magFilter) {
    _fontTexture.setMinMagFilter( minFilter, magFilter );
}

Texture *Font::getFontTexture() {
    return &_fontTexture;
}

// ============================================================================
// ============================================================================
// MonoFont

/*MonoFont::MonoFont() {}
MonoFont::~MonoFont() {}

void MonoFont::loadBitmap( std::string filename ) {
	GLuint textureId=0;

	SDL_Surface *surf = IMG_Load(filename.c_str());
	SDL_Surface* optimized = NULL;

	if( surf != NULL) {
		optimized = SDL_DisplayFormatAlpha( surf);
		SDL_FreeSurface(surf);
	}

	if( optimized != NULL) {

		int mode=GL_RGB;
		if(optimized->format->BytesPerPixel == 4) {
			if (optimized->format->Rmask == 0x000000FF)
					mode = GL_RGBA;
                else
                    mode = GL_BGRA;
		} else {
				std::cout << "Error loading bitmap font: image has 3 Channels, could not convert to 4 Channels." << std::endl;
				SDL_FreeSurface(optimized);
				return;
		}

		GLuint * pixels = (GLuint*)optimized->pixels;
		int width = optimized->w;
		int height = optimized->h;

		// Parse bitmap
		float cellSizeX = (float)width / 16.0f;
		float cellSizeY = (float)height / 16.0f;
		_fontWidth = (int)cellSizeX;
		_fontHeight = (int)cellSizeY;

		int leftLimit = _fontWidth/2;
		int rightLimit = _fontWidth/2;
		int topLimit = _fontHeight/2;
		int botLimit = _fontHeight/2;

		int nCharsLine = 16;

		// Iterate each character
		for( int i = 0 ; i < 256 ; ++i ) {
			// Initializations
			int lowx = i%nCharsLine;
			int lowy = i/nCharsLine;
			int startindex = lowx*(int)cellSizeX + lowy*(int)cellSizeX*(int)width;

			// Find left bound
			for ( int x = 0 ; x < (int)cellSizeX ; ++x ) {
				for ( int y = 0; y < (int)cellSizeY ; ++y ) {
					if( pixels[startindex+x+y*(int)width] != PIXEL_BLACK ) {
						leftLimit = std::min(x, leftLimit);

						// Break (horrible)
						x = (int)cellSizeX+1;
						y = (int)cellSizeY+1;
					}
				}
			}

			// Find right bound
			for ( int x = (int)cellSizeX-1 ; x >= 0 ; --x ) {
				for ( int y = 0; y < (int)cellSizeY ; ++y ) {
					if( pixels[startindex+x+y*(int)width] != PIXEL_BLACK ) {
						rightLimit = std::max(x, rightLimit);

						// Break (horrible)
						x = -1;
						y = (int)cellSizeY+1;
					}
				}
			}

			// Find lower bound
			for ( int y = 0; y < (int)cellSizeY ; ++y ) {
				for ( int x = 0 ; x < (int)cellSizeX ; ++x ) {
					if( pixels[startindex+x+y*(int)width] != PIXEL_BLACK ) {
						botLimit = std::min(y, botLimit);

						// Break (horrible)
						x = (int)cellSizeX+1;
						y = (int)cellSizeY+1;
					}
				}
			}

			// Find upper bound
			for ( int y = (int)cellSizeY-1; y >= 0 ; --y ) {
				for ( int x = 0 ; x < (int)cellSizeX ; ++x ) {
					if( pixels[startindex+x+y*(int)width] != PIXEL_BLACK ) {
						topLimit = std::max(y, topLimit);

						// Break (horrible)
						x = (int)cellSizeX+1;
						y = -1;
					}
				}
			}
		}
		// End parsing bitmap

		// Iterate each character again to create clips
		int monoSpaceX = rightLimit - leftLimit + 1;
		int monoSpaceY = topLimit - botLimit + 1;
		for( int i = 0 ; i < 256 ; ++i ) {
			// Initializations
			int lowx = i%nCharsLine;
			int lowy = i/nCharsLine;

			_clips[i].left = ((float)lowx*cellSizeX+(float)leftLimit)/(float)width;
			_clips[i].right = ((float)lowx*cellSizeX+(float)(rightLimit+1))/(float)width;
			_clips[i].top = 1.0f-((float)lowy*cellSizeY+(float)(botLimit))/(float)height;
			_clips[i].bottom = 1.0f-((float)lowy*cellSizeY+(float)(topLimit+1))/(float)height;

			/*_clips[i].top = 1.0f-(lowy*cellSizeY)/(float)height;
			_clips[i].bottom = 1.0f-(lowy+1)*cellSizeY/(float)height;*/

		/*	_charactersWidth[i] = monoSpaceX;
		}

		// Default values and spacing
		_maxCharWidth = monoSpaceX;
		_maxCharHeight = monoSpaceY;
		_spaceBetweenChars = 1.0f;
		_spaceBetweenLines = (float)(_fontHeight-_maxCharHeight);
		_halfHeight = (float)_maxCharHeight/2.0f;

		_spaceWidth = _maxCharWidth+_spaceBetweenChars;
		_tabWidth = (_maxCharWidth+_spaceBetweenChars)*4;

		// Set black bits to transparent
		int pixel_size = width * height;
		for( int i = 0; i < pixel_size; ++i ) {
			if( pixels[i] == PIXEL_BLACK ) 
				pixels[i] = PIXEL_TRANSPARENT;
		}

		// Because OpenGl inverts the pixels Y, invert comes after parsing
		invertImageY((GLuint*)optimized->pixels, optimized->w, optimized->h);

		// Create GL texture, wont call makeTexture due to GL_NEAREST which should be
		// the default min/mag option for fonts
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexImage2D(GL_TEXTURE_2D, 0, 4, optimized->w, optimized->h, 0, mode, GL_UNSIGNED_BYTE, optimized->pixels);

		// For fonts default is GL_NEAREST
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		SDL_FreeSurface(optimized);

		_bitmap = textureId;
	}
}*/

/*void MonoFont::setCharSpacingX(float space) {
	_spaceBetweenChars = space;
	_spaceWidth = _maxCharWidth+_spaceBetweenChars;
	_tabWidth = (_maxCharWidth+_spaceBetweenChars)*4;
}
void MonoFont::setCharSpacingY(float space) {
	_spaceBetweenLines = space;
}*/