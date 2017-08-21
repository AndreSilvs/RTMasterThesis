#include "SystemErrors.h"

#include "Logging.h"

namespace {
    GLenum lastErrorGL = GL_NO_ERROR;
}

void getSDLError( std::string optional ) {
    std::string error = SDL_GetError();
    Logging::error.reportError( 0, "SDL error. " + optional + "\n\t" + error );
}

/*bool getGlewError( GLenum errorCode, std::string optional ) {
    if ( errorCode == GLEW_OK ) {
        return false;
    }
    std::string error = reinterpret_cast< const char*>( glewGetErrorString( errorCode ) );
    Logging::error.reportError( 0, "Glew error. " + optional + "\n\t" + error );
    return true;
}*/

bool getOpenGLError( std::string optional ) {
    lastErrorGL = glGetError();
    if ( lastErrorGL != GL_NO_ERROR ) {
        Logging::error.reportError( lastErrorGL, "OpenGL error " + optional );
        return true;
    }
    return false;
}

int getOpenGLLastError() {
    return lastErrorGL;
}

bool getDevILError( ILenum errorCode, std::string optional ) {
    if ( errorCode == IL_NO_ERROR ) {
        return false;
    }
    //std::string error = iluErrorString( errorCode );
    Logging::error.reportError( errorCode, "DevIL Error: " + optional + "\n" /*+ error*/ );
    return true;
}