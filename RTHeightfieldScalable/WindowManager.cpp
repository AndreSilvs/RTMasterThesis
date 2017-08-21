#include "WindowManager.h"

WindowManagerGL::WindowManagerGL() : _window( NULL ), _context( NULL ) {}
WindowManagerGL::~WindowManagerGL() {
    closeWindow();
}

bool WindowManagerGL::createWindow( const std::string& title, int defaultWidth, int defaultHeight, int mode ) {
    _defaultWidth = defaultWidth;
    _defaultHeight = defaultHeight;

    // Create application window
    _window = SDL_CreateWindow( title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, defaultWidth, defaultHeight, mode );
    if ( _window == NULL ) {
        return false;
    }
    _windowMode = ( mode & SDL_WINDOW_FULLSCREEN ? SDL_WINDOW_FULLSCREEN :
        ( mode & SDL_WINDOW_FULLSCREEN_DESKTOP ? SDL_WINDOW_FULLSCREEN_DESKTOP :
        ( mode & SDL_WINDOW_BORDERLESS ? SDL_WINDOW_BORDERLESS : 0 ) ) );
    //setIcon();

    // Create OpenGL context
    _context = SDL_GL_CreateContext( _window );
    if ( _context == NULL ) {
        SDL_DestroyWindow( _window ); _window = NULL;
        return false;
    }

    _width = defaultWidth;
    _height = defaultHeight;

    _loaded = true;

    return true;
}
void WindowManagerGL::closeWindow() {
    if ( _loaded ) {
        SDL_GL_DeleteContext( _context );
        SDL_DestroyWindow( _window );
        _window = NULL; _context = NULL;
    }
}

void WindowManagerGL::setFullscreen() {
    SDL_SetWindowFullscreen( _window, SDL_WINDOW_FULLSCREEN );
    _windowMode = SDL_WINDOW_FULLSCREEN;
    updateDimensions();
}
void WindowManagerGL::setDesktopFullscreen() {
    SDL_SetWindowFullscreen( _window, SDL_WINDOW_FULLSCREEN_DESKTOP );
    _windowMode = SDL_WINDOW_FULLSCREEN_DESKTOP;
    updateDimensions();
}
void WindowManagerGL::setWindowedMode() {
    SDL_SetWindowFullscreen( _window, 0 );
    _windowMode = 0;
    updateDimensions();
}

void WindowManagerGL::maximizeWindow() {
    SDL_MaximizeWindow( _window );
    updateDimensions();
}

void WindowManagerGL::forceResize( int width, int height ) {
    SDL_SetWindowSize( _window, width, height );
    updateDimensions();
}

void WindowManagerGL::eventResize( int width, int height ) {
    _width = width; _height = height;
    updateDimensions();
}

void WindowManagerGL::setTitle( const std::string& windowCaptcha ) {
    SDL_SetWindowTitle( _window, windowCaptcha.c_str() );
}

int WindowManagerGL::getWidth() const {
    return _width;
}
int WindowManagerGL::getHeight() const {
    return _height;
}

void WindowManagerGL::swapBuffer() {
    SDL_GL_SwapWindow( _window );
}

void WindowManagerGL::updateDimensions() {
    SDL_GetWindowSize( _window, &_width, &_height );
    glViewport( 0, 0, _width, _height );
}

int WindowManagerGL::getWindowResolutionMode() {
    return _windowMode;
}