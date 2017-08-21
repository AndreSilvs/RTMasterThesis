#include "Bits.h"

int nextPowerOfTwo(int num) {
	num--;
	num |= num >> 1;
	num |= num >> 2;
	num |= num >> 4;
	num |= num >> 8;
	num |= num >> 16;
	num++;

	return num;
}

int previousPowerOfTwo(int num) {
	num = num | (num >> 1);
	num = num | (num >> 2);
	num = num | (num >> 4);
	num = num | (num >> 8);
	num = num | (num >> 16);

	return num - (num >> 1);
}

// ABGR color bits
unsigned int ABGRtoUint( unsigned char alpha, unsigned char blue, unsigned char green, unsigned char red ) {
    return (unsigned int)( ( (unsigned int)alpha << 24 ) +
        ( (unsigned int)blue << 16 ) +
        ( (unsigned int)green << 8 ) +
        (unsigned int)red );
}
int redABGR(unsigned int abgrColor) {
	abgrColor &= 0x000000FF;
	return (int)abgrColor;
}
int greenABGR(unsigned int abgrColor) {
	abgrColor &= 0x0000FF00;
	abgrColor >>= 8;
	return (int)abgrColor;
}
int blueABGR(unsigned int abgrColor) {
	abgrColor &= 0x00FF0000;
	abgrColor >>= 16;
	return (int)abgrColor;
}
int alphaABGR(unsigned int abgrColor) {
	abgrColor &= 0xFF000000;
	abgrColor >>= 24;
	return (int)abgrColor;
}

// RGBA color bits
unsigned int RGBAtoUint( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha ) {
    return ( ( (unsigned int)red << 24 ) +
        ( (unsigned int)green << 16 ) +
        ( (unsigned int)blue << 8 ) +
        (unsigned int)alpha );
}
int redRGBA(unsigned int abgrColor) {
	abgrColor &= 0xFF000000;
	abgrColor >>= 24;
	return (int)abgrColor;
}
int greenRGBA(unsigned int abgrColor) {
	abgrColor &= 0x00FF0000;
	abgrColor >>= 16;
	return (int)abgrColor;
}
int blueRGBA(unsigned int abgrColor) {
	abgrColor &= 0x0000FF00;
	abgrColor >>= 8;
	return (int)abgrColor;
}
int alphaRGBA(unsigned int abgrColor) {
	abgrColor &= 0x000000FF;
	return (int)abgrColor;
}

unsigned int RGBAtoABGR(unsigned int rgba) {
	return ((rgba & 0xFF000000) >> 24) |
			((rgba & 0x00FF0000) >> 8) |
			((rgba & 0x0000FF00) << 8) |
			((rgba & 0x000000FF) << 24);
}
unsigned int ABGRtoRGBA(unsigned int abgr) {
	return ((abgr & 0xFF000000) >> 24) |
			((abgr & 0x00FF0000) >> 8) |
			((abgr & 0x0000FF00) << 8) |
			((abgr & 0x000000FF) << 24);
}
