#pragma once

#include <string>

#include "API_Headers.h"

class WindowManagerGL {
public:
    WindowManagerGL();
    ~WindowManagerGL();

    bool createWindow( const std::string& title, int defaultWidth, int defaultHeight, int mode );
    void closeWindow();

    void setFullscreen();
    void setDesktopFullscreen();
    void setWindowedMode();
    void maximizeWindow();

    void forceResize( int width, int height );

    void eventResize( int width, int height );

    void setTitle( const std::string& windowTitle );

    void swapBuffer();

    int getWidth() const;
    int getHeight() const;

    // 0 = Windowed, SDL_WINDOW_FULLSCREEN, SDL_WINDOW_BORDERLESS
    int getWindowResolutionMode();

    //private:
public:
    void updateDimensions();

    int _defaultWidth;
    int _defaultHeight;

    int _width;
    int _height;

    bool _loaded;
    unsigned int _windowMode;
    SDL_Window* _window;
    SDL_GLContext _context;
};