#pragma once

#include "API_Headers.h"

#include <string>

void printSDL_Event( const SDL_Event * event );

std::string getGlPrimitiveName( GLenum primitive );

void reportWindowsError( const std::string& title, const std::string& error );

GLint getGLIntegerValue( GLenum valueID );
