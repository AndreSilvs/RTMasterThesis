#include "API_Debug.h"

void printSDL_Event( const SDL_Event * event ) {
    if ( event->type == SDL_WINDOWEVENT ) {
        switch ( event->window.event ) {
            case SDL_WINDOWEVENT_SHOWN:
                SDL_Log( "Window %d shown", event->window.windowID );
                break;
            case SDL_WINDOWEVENT_HIDDEN:
                SDL_Log( "Window %d hidden", event->window.windowID );
                break;
            case SDL_WINDOWEVENT_EXPOSED:
                SDL_Log( "Window %d exposed", event->window.windowID );
                break;
            case SDL_WINDOWEVENT_MOVED:
                SDL_Log( "Window %d moved to %d,%d",
                    event->window.windowID, event->window.data1,
                    event->window.data2 );
                break;
            case SDL_WINDOWEVENT_RESIZED:
                SDL_Log( "Window %d resized to %dx%d",
                    event->window.windowID, event->window.data1,
                    event->window.data2 );
                break;
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                SDL_Log( "Window %d size changed to %dx%d",
                    event->window.windowID, event->window.data1,
                    event->window.data2 );
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                SDL_Log( "Window %d minimized", event->window.windowID );
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
                SDL_Log( "Window %d maximized", event->window.windowID );
                break;
            case SDL_WINDOWEVENT_RESTORED:
                SDL_Log( "Window %d restored", event->window.windowID );
                break;
            case SDL_WINDOWEVENT_ENTER:
                SDL_Log( "Mouse entered window %d",
                    event->window.windowID );
                break;
            case SDL_WINDOWEVENT_LEAVE:
                SDL_Log( "Mouse left window %d", event->window.windowID );
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                SDL_Log( "Window %d gained keyboard focus",
                    event->window.windowID );
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                SDL_Log( "Window %d lost keyboard focus",
                    event->window.windowID );
                break;
            case SDL_WINDOWEVENT_CLOSE:
                SDL_Log( "Window %d closed", event->window.windowID );
                break;
            default:
                SDL_Log( "Window %d got unknown event %d",
                    event->window.windowID, event->window.event );
                break;
        }
    }
}

std::string getGlPrimitiveName( GLenum primitive ) {
    switch ( primitive ) {
        case GL_POINTS: return "GL_POINTS";
        case GL_LINES: return "GL_LINES";
        case GL_LINE_STRIP: return "GL_LINE_STRIP";
        case GL_LINE_LOOP: return "GL_LINE_LOOP";
        case GL_LINES_ADJACENCY: return "GL_LINES_ADJACENCY";
        case GL_TRIANGLES: return "GL_LINES";
        case GL_TRIANGLES_ADJACENCY: return "GL_TRIANGLES_ADJACENCY";
        case GL_TRIANGLE_FAN: return "GL_TRIANGLES_FAN";
        case GL_TRIANGLE_STRIP: return "GL_TRIANGLES_STRIP";
        case GL_TRIANGLE_STRIP_ADJACENCY: return "GL_TRIANGLE_STRIP_ADJACENCY";
        case GL_PATCHES: return "GL_PATCHES";
    }
    return "InvalidEnum";
}

void reportWindowsError( const std::string& title, const std::string& error ) {
    MessageBoxA( 0, error.c_str(), title.c_str(), MB_OK );
}

GLint getGLIntegerValue( GLenum valueID ) {
    GLint v;
    glGetIntegerv( valueID, &v );
    return v;
}