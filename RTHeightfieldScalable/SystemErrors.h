#pragma once

#include "API_Headers.h"

#include <string>

void getSDLError( std::string optional = "" );
//bool getGlewError( GLenum errorCode, std::string optional = "" );
bool getOpenGLError( std::string optional = "" );
int getOpenGLLastError();
bool getDevILError( ILenum errorCode, std::string optional = "" );
