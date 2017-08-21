#pragma once

int nextPowerOfTwo( int num );
int previousPowerOfTwo( int num );

// Takes color in ABGR format (0xAABBGGRR)
unsigned int ABGRtoUint( unsigned char alpha, unsigned char blue, unsigned char green, unsigned char red );
int redABGR( unsigned int abgrColor );
int greenABGR( unsigned int abgrColor );
int blueABGR( unsigned int abgrColor );
int alphaABGR( unsigned int abgrColor );

unsigned int RGBAtoUint( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha );
int redRGBA( unsigned int rbgaColor );
int greenRGBA( unsigned int rgbaColor );
int blueRGBA( unsigned int rgbaColor );
int alphaRGBA( unsigned int rbgaColor );

unsigned int RGBAtoABGR( unsigned int rgba );
unsigned int ABGRtoRGBA( unsigned int abgr );
