#ifndef BITMAP_FONT_HEADER
#define BITMAP_FONT_HEADER

/** GLOBAL	HEADERS	**/
#include "API_Headers.h"

#include <iostream>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H


/** LOCAL	HEADERS	**/
#include "MatrixStack.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "Vector.h"

/** FORWARD DECLARATIONS **/

/** NOTES **/
extern FT_Library ttfLib;

/*struct FontGlyphInfo {
    Clip textureClip;
    int bearingX;
    int horizontalAdvance;
    int width;
};*/

enum TextAlignment {
	TEXT_ALIGNED_CENTER,
	TEXT_ALIGNED_LEFT,
	TEXT_ALIGNED_RIGHT
};


bool initializeTTF();

class Font {
public:
    static ShaderProgram shader;
    static bool initializeFontSystem();
    static void closeFontSystem();

    static void setFontColor( Color4C color );
    static void setMatrixes();
    static void setModelMatrix();

    static MatrixStack projection;
    static MatrixStack view;
    static MatrixStack model;

public:
	Font();
	virtual ~Font();

	bool isLoaded() const;

	void freeFont();

	virtual bool loadBitmap( const std::string &fileName );

	bool loadTTF( std::string filename, int fontSize );

	// Stops at newline or end
	float getTextLength( const std::string &text ) const;
	// Count newlines
	int getTextNewlines( const std::string &text, float maxlength = 0.0f ) const;
	float getTextHeight( const std::string &text, float maxlength = 0.0f ) const;
	// Translation offset to after last character, assuming left alignment
	Vector2D getTextOffset( const std::string &text, float maxlength = 0.0f ) const;

	std::string getTextFromLine( const std::string &text, int line, float maxlength = 0.0f ) const;

	std::string getFirstLine( const std::string &text, float maxlength ) const;

	int getFontWidth() const;
	int getFontHeight() const;
	float getSpaceBetweenChars() const;
	float getSpaceBetweenLines() const;
	int getCharWidth(unsigned int c) const;
	int getCharMaxWidth() const;
	int getCharMaxHeight() const;

	void renderTextLeft(const std::string &text, float maxlength = 0.0f, float startlength = 0.0f) const;
	void renderTextCentered(const std::string &text, float maxlength = 0.0f) const;
	void renderTextRight( const std::string &text, float maxlength = 0.0f ) const;

	void renderTextCircle( const std::string &text, float radius, float angle );

	// Values from 1.0f to 4.0f are usually enough
	virtual void setSpaceBetweenChars( float space );
	virtual void setSpaceBetweenLines( float space );
	// GL_LINEAR, GL_NEAREST, ...
	void setMinMagFilter( GLenum minFilter, GLenum magFilter );

    Texture *getFontTexture();

protected:
    static bool isFontSystemLoaded;
    static bool createQuad();
    static void uploadQuadData( float *position, float *texCoords );

    Texture _fontTexture;
    bool _isLoaded;

    Clip _clips[ 256 ];
    int _charactersWidth[ 256 ];
    int _horizontalAdvancement[ 256 ];
    int _bearingX[ 256 ];

	int _fontWidth;
	int _fontHeight;
	int _maxCharHeight;
	int _maxCharWidth;

	float _spaceBetweenChars;
	float _spaceBetweenLines;

	float _spaceWidth;
	float _tabWidth;
	float _halfHeight;

};

/*class MonoFont : public Font {
public:
	MonoFont();
	~MonoFont();

	void loadBitmap(std::string filename );

	void setCharSpacingX(float space);
	void setCharSpacingY(float space);
};*/

#endif