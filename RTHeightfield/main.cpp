#include "API_Headers.h"
#include "API_Debug.h"

#include <iostream>
#include <string>
#include <ios>

//#include "AudioSystem.h"

#include "DebugPrint.h"
#include "Logging.h"
#include "SystemErrors.h"

#include "MainShader2D.h"
#include "MainShader3D.h"
#include "PostProcShader.h"
#include "FrameBufferObject.h"

#include "TimeCalcs.h"
#include "StringManipulation.h"

#include "MathCalcs.h"
#include "VectorCalcs.h"

#include "DynamicFloatMap.h"

#include "GlobalSystems.h"

#include "RTInitialization.h"
#include "RTHFScene.h"
#include "RTHFShader.h"


#include "Heightfield.h"

#include "RTConfig.h"
#include "DebugTimer.h"

#include "PathScript.h"

namespace {
    const int FPS = 60; // Frames per second
    const int MSPF = 1000 / FPS; // Miliseconds per frame

    // Max performance test time
    const int recordSeconds = 300;

    //Screen dimension constants
    const int SCREEN_WIDTH = 1024;
    const int SCREEN_HEIGHT = 768;

    //Main loop flag
    bool quit = false;
    bool showHUD = true;

    //Event handler
    SDL_Event events;

    FrameBufferObject gMyFBO;

    Vector2D mouseCoords{ 0, 0 };
    Vector2D mouseWCoords;

    int measuredFrameRate = 0;
    int rayAccumulator = 0;
    int measureRayPerSecond = 0;
    std::vector<int> fpsRecord;
    std::vector<unsigned int> rayRecord;
    bool recordFPS = false;
    std::string hfName;

    // GameEngine
    RTHFShader rtShader;
    RTHFScene rtScene;
    Heightfield rtHeightfield;
	RTConfig configData;
    float _camAngleH = 0.0f;
    float _camAngleV = 0.0f;
    float _camDistance = 360.0f;

    bool checkMaxMipmap = false;
    bool mouseRelative = false;

    ShaderProgram debugShader;

    // Variable debugging
    std::vector< std::pair< std::string, DynamicFloatWrapper* > > debugList;
    int currentDebugVar = 0;

    bool isMultisamplingOn = false;

    bool autoLightCycle = false;
    bool autoLightSide = false;


    CameraPathScript pathScript;
    int camScriptMode = 0; // 0 = Nothing, 1 = Play, 2 = Record
    float camScriptCounter = 0.0f;
};

// Main aux functions
// ==================
void storeFPSrecord();

bool initialize();

bool initializeSDL();
bool initializeGlad();
bool initializeOpenGL();
bool initializeApplication();

void handleEvents();
void handleKeys( unsigned char key, int x, int y );
void update( int ms );
void render();

void close();
// ==================

int main( int argc, char* args[] ) {
    //Start up SDL and create window
    if ( !initialize() ) {
        close();
        return 1;
    }

    // Start main loop
    //Enable text input
    SDL_StartTextInput();

    int counter = 0;
    int lastTime = 0;
    int currentTime = 0;

    int accum = 0;
    int frames = 0;
    int delta = 0;

    lastTime = SDL_GetTicks();

    while ( !quit ) {
        // Update elapsed time
        currentTime = SDL_GetTicks();
        delta = currentTime - lastTime;
        //counter += delta;
        accum += delta;
        lastTime = currentTime;

        if ( accum > 1000 ) {
            accum = 0;
            measuredFrameRate = frames;
            frames = 0;
            measureRayPerSecond = rayAccumulator;
            rayAccumulator = 0;
            if ( recordFPS ) {////
                fpsRecord.push_back( measuredFrameRate );
                rayRecord.push_back( measureRayPerSecond );
                if ( fpsRecord.size() == ::recordSeconds ) {
                    recordFPS = false;
                    storeFPSrecord();
                }
            }////
        }

        // Events
        handleEvents();

        // Update
        update( delta );

        render();
        frames++;
    }

    //Disable text input
    SDL_StopTextInput();

    //Free resources and close SDL
    close();

    return 0;
}

bool initialize() {
	if ( !configData.readConfigFile( "Assets/config.xml" ) ) {
		std::cout << "Error reading config file." << std::endl;
		getchar();
		return false;
	}

    if ( !initializeSDL() ) {
        return false;
    }

    if ( !initializeGlad() ) {
        return false;
    }

    //Use Vsync
    if ( SDL_GL_SetSwapInterval( 1 ) < 0 ) {
        getSDLError( "Warning: Unable to set VSync!" );
    }

    //Initialize OpenGL
    if ( !initializeOpenGL() ) {
        return false;
    }

    // Initialize Application
    if ( !initializeApplication() ) {
        return false;
    }

    return true;
}

bool initializeSDL() {
    //Initialize SDL
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        getSDLError( "SDL could not initialize!" );
        reportWindowsError( "SDL Error", "Failed to initialize video." );
        return false;
    }

    SDL_GL_LoadLibrary( NULL );

    //Use OpenGL 4.5 core
    SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    // Allow multisampling to be used
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 4 );


    if ( SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ) < 0 ) { return false; }
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
    //SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

    //Create window
    if ( !globalSystem::window.createWindow( "RTHF-MaximumMipmaps", ::configData._screenResolutions[0].first, ::configData._screenResolutions[ 0 ].second,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE ) ) {
        getSDLError( "Window creation failed." );
        reportWindowsError( "SDL Error", "Failed to initialize window." );
        return false;
    }

    //SDL_SetRelativeMouseMode( SDL_TRUE );

    return true;
}

bool initializeGlad() {
    if ( !gladLoadGLLoader( SDL_GL_GetProcAddress ) ) {
        reportWindowsError( "GLAD Error", "Unsupported OpenGL version." );
    }

    return true;
}

bool initializeOpenGL() {
    const GLubyte *renderer = glGetString( GL_RENDERER );
    const GLubyte *vendor = glGetString( GL_VENDOR );
    const GLubyte *version = glGetString( GL_VERSION );
    const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

    GLint major, minor;
    glGetIntegerv( GL_MAJOR_VERSION, &major );
    glGetIntegerv( GL_MINOR_VERSION, &minor );

    std::cout << "OpenGL Version: " << major << "." << minor << "\n";
    std::cout << "=============================================\n";
    std::cout << "Vendor:    " << vendor << "\n";
    std::cout << "Renderer:  " << renderer << "\n";
    std::cout << "Version:   " << version << "\n";
    std::cout << "GLSL:   " << glslVersion << "\n";
    std::cout << "=============================================\n";
    std::cout << "Other stats:\n";
    std::cout << "Max Texture Size: " << getGLIntegerValue( GL_MAX_TEXTURE_SIZE ) << "\n";
    std::cout << "=============================================\n";

    glClearColor( 0x1F / 255.f, 0x1F / 255.f, 0x1F / 255.f, 1.f );
    
    glEnable( GL_DEPTH_TEST );
    glDepthMask( GL_TRUE );
    glDepthFunc( GL_LEQUAL );
    glDepthRange( 0.0f, 1.0f );
    glClearDepth( 1.0f );

    //glEnable( GL_STENCIL_TEST );

    glEnable( GL_CULL_FACE );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glActiveTexture( GL_TEXTURE0 );

    if ( getOpenGLError( "Error enabling OpenGL systems." ) ) {
        return false;
    }
    
    rtShader.loadShader();

    if ( !debugShader.compileProgram( "Shaders/debugVS.vert", "Shaders/debugFS.frag" ) ) {
        return false;
    }

    if ( !initializeDevIL() ) {
        return false;
    }

    if ( !globalSystem::textures.initialize() ) {
        return false;
    }

    if ( !Font::initializeFontSystem() ) {
        return false;
    }
    setOrthoMatrix( Font::projection.getMatrix(), 0.f, (float)SCREEN_WIDTH, 0.f, (float)SCREEN_HEIGHT, 1.f, -1.f );
    
    if ( !gMyFBO.initializeFBO( SCREEN_WIDTH, SCREEN_HEIGHT ) ) {
        return false;
    }

    return true;
}

bool initializeApplication() {
    //globalSystem::window.maximizeWindow();

    /*if ( !AudioSystem::instance.initializeAudio() ) {
        return false;
    }*/

    if ( !globalSystem::gameFont.loadTTF( "Assets/Fonts/arial.ttf", 16 ) ) {
        return false;
    }
    /*if ( !globalSystem::gameFont.loadBitmap( "Assets/Fonts/MonoOutlineFont.png" ) ) {
        return false;
    }*/
    globalSystem::gameFont.setMinMagFilter( GL_NEAREST, GL_NEAREST );
    if ( !globalSystem::gameFontLarge.loadTTF( "Assets/Fonts/arial.ttf", 32 ) ) {
        return false;
    }

    // RT Configuration
    DebugTimer loadTimer;
    loadTimer.start();

    std::string hmapFileName = "Assets/Textures/testhmap1k-perlin.png";
    std::string ptexFileName = "Assets/Textures/phototex1k.png";
    if ( ::configData.successfulRead ) {
        hmapFileName = ::configData._heightmapFileName;
        ptexFileName = ::configData._photoTexName;
        std::cout << "Config File Found" << std::endl;
        std::cout << "Loading heightmap: " << hmapFileName << std::endl;
        if ( ptexFileName.empty() ) { std::cout << "No photo texture included" << std::endl; }
        else { std::cout << "Loading photo texture: " << ptexFileName << std::endl; }

        std::cout << "Heightfield BPC: " << ::configData._hfBpp << std::endl;
    }
    else {
        std::cout << "Config File Not Found - Loading default files" << std::endl;
        std::cout << hmapFileName << " + " << ptexFileName << std::endl;
    }
    std::cout << "Config Height: " << ::configData._mapHeight << std::endl;

    if ( ::configData._hfBpp != 8 && ::configData._hfBpp != 16 ) {
        std::cout << "Unsupported BPC: " << ::configData._hfBpp << std::endl;
        return false;
    }

    // Initialize ray tracing shaders
    if ( !myrt::initialize() ) {
        return false;
    }

    rtHeightfield.setMapHeight( ::configData._mapHeight );
    rtHeightfield.setPatchSize( ::configData._patchSize );
    ::hfName = hmapFileName;

    if ( ::configData._hfBpp == 8 ) {
        // Load heightmap file
        if ( !::configData._isHeightmapCompact ) {
            if ( !rtHeightfield.loadHeightmap_8b( hmapFileName ) ) {
                std::cout << "Failed to load heightmap" << std::endl;
                return false;
            }
            else {
                std::cout << "Successfully loaded compact heightfield format." << std::endl;
            }
        }
        else {
            if ( !rtHeightfield.loadCompactHeightmap_8b( hmapFileName ) ) {
                std::cout << "Failed to load pre-compacted heightmap" << std::endl;
                return false;
            }
            else {
                std::cout << "Successfully loaded pre-compacted heightfield format." << std::endl;
            }
        }

        if ( getOpenGLError() ) {
            std::cout << "Something went wrong here after loading hf." << std::endl;
        }

        // Load phototexture file
        if ( !::configData._photoTexName.empty() && rtHeightfield.loadPhotoTexture( ::configData._photoTexName ) ) {
            std::cout << "Successfully loaded photo texture" << std::endl;
            rtHeightfield.getPhotoTexture()->setMinMagFilter( GL_LINEAR, GL_LINEAR );
            rtHeightfield.getPhotoTexture()->setWrapMode( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            rtHeightfield.getPhotoTexture()->bindTexture();
            /*GLint wasCompressed = 0;
            glGetTexLevelParameteriv( GL_TEXTURE_2D, 0,
                GL_TEXTURE_COMPRESSED_ARB, &wasCompressed );*/


            GLint intFormat = 0;
            glGetTexLevelParameteriv( GL_TEXTURE_2D, 0,
                GL_TEXTURE_INTERNAL_FORMAT, &intFormat );
            std::ios::fmtflags f( std::cout.flags() );
            std::cout << "Photo Texture internal format: " << "0x" << std::hex << intFormat << std::endl;
            std::cout.flags( f );

            GLint texSize = 0;
            glGetTexLevelParameteriv( GL_TEXTURE_2D, 0,
                GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &texSize );
            std::cout << "Photo Texture compressed size: " << texSize << " bytes" << std::endl;
            rtHeightfield.getPhotoTexture()->unbindTexture();
        }
        else {
            rtHeightfield.loadDefaultPhotoTexture();
            std::cout << "No photo texture provided" << std::endl;
        }

        if ( getOpenGLError() ) {
            std::cout << "Something went wrong here while loading phototex" << std::endl;
            return false;
        }

        std::cout << "Loading normals (8): ";
        rtHeightfield.preComputeNormals2();
        std::cout << "Complete!" << std::endl;

    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////// 16 BPP
    else if ( ::configData._hfBpp == 16 ) {

        if ( !::configData._isHeightmapCompact ) {
            if ( !rtHeightfield.loadHeightmap_16b( hmapFileName ) ) {
                std::cout << "Failed to load heightmap" << std::endl;
                getchar();
                return false;
            }
            else {
                std::cout << "Successfully loaded 16bit per point heightfield." << std::endl;
            }
        }
        else {
            if ( !rtHeightfield.loadCompactHeightmap_16b( hmapFileName ) ) {
                std::cout << "Failed to load compact 16bit heightmap" << std::endl;
                getchar();
                return false;
            }
            std::cout << "Successfully loaded 16bit per patch heightfield." << std::endl;
        }

        if ( getOpenGLError() ) {
            std::cout << "Something went wrong here after loading 16 bit hf." << std::endl;
        }

        // Load phototexture file
        if ( !::configData._photoTexName.empty() && rtHeightfield.loadPhotoTexture( ::configData._photoTexName ) ) {
            std::cout << "Successfully loaded photo texture" << std::endl;
            rtHeightfield.getPhotoTexture()->setMinMagFilter( GL_LINEAR, GL_LINEAR );
            rtHeightfield.getPhotoTexture()->setWrapMode( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            rtHeightfield.getPhotoTexture()->bindTexture();
            /*GLint wasCompressed = 0;
            glGetTexLevelParameteriv( GL_TEXTURE_2D, 0,
            GL_TEXTURE_COMPRESSED_ARB, &wasCompressed );*/


            GLint intFormat = 0;
            glGetTexLevelParameteriv( GL_TEXTURE_2D, 0,
                GL_TEXTURE_INTERNAL_FORMAT, &intFormat );
            std::ios::fmtflags f( std::cout.flags() );
            std::cout << "Photo Texture internal format: " << "0x" << std::hex << intFormat << std::endl;
            std::cout.flags( f );

            GLint texSize = 0;
            glGetTexLevelParameteriv( GL_TEXTURE_2D, 0,
                GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &texSize );
            std::cout << "Photo Texture compressed size: " << texSize << " bytes" << std::endl;
            rtHeightfield.getPhotoTexture()->unbindTexture();
        }
        else {
            rtHeightfield.loadDefaultPhotoTexture();
            std::cout << "No photo texture provided" << std::endl;
        }

        if ( getOpenGLError() ) {
            std::cout << "An error occurred while loading the photo texture." << std::endl;
        }

        std::cout << "Loading normals (16): ";
        rtHeightfield.preComputeNormals_16b();
        std::cout << "Complete!" << std::endl;
    }

	if ( ::configData._startPosition.first ) { rtScene.cameraEye = ::configData._startPosition.second; }
	else { rtScene.cameraEye = { 0.0f, 0.0f, 384.0f }; }
	if ( ::configData._startDirection.first ) {
		::_camAngleH = ::configData._startDirection.second.x;
		::_camAngleV = ::configData._startDirection.second.y;
	}
	else {
		::_camAngleH = -45.0f;
		::_camAngleV = -24.0f;
	}
    rtScene.cameraUp = { 0.0f, 0.0f, 1.0f };
    rtScene.fovAngle = 60.0f;
    rtScene.farZ = 2048.0f;
    rtScene.screenResolution.x = (float)globalSystem::window.getWidth();
    rtScene.screenResolution.y = (float)globalSystem::window.getHeight();
    rtScene.heightfield = &rtHeightfield;
    rtScene.sceneHeight = ::configData._mapHeight;
    rtScene.renderMode = RT_RENDERMODE_PHOTO;
    //rtScene.precomputeUVN();

    loadTimer.stop();

    std::cout << "Scene loaded in: " << ( (double)loadTimer.getTimeElapsed() ) / 1000.0 << "s" << std::endl;

    // Start Value, Minimum Value, Maximum Value, Step
    globalSystem::runtimeData.addData( "Scene-far", DynamicFloatWrapper( rtScene.farZ, 512.0f, 65536.0f, 256.0f ) );
    globalSystem::runtimeData.addData( "CameraSpeed", DynamicFloatWrapper( 160.0f, 1.0f, 4096.0f, 15.0f ) );
    globalSystem::runtimeData.addData( "Rendermode", DynamicFloatWrapper( 0.0f, 0.0f, RT_RENDERMODE_TOTAL-1, 1.0f ) );
    globalSystem::runtimeData.addData( "Lod-Z-param", DynamicFloatWrapper( 256.0f, 128.0f, 4096.0f, 2.0f ) );
    globalSystem::runtimeData.addData( "Light-angle", DynamicFloatWrapper( -45.0f, -85.0f, 85.0f, 0.5f ) );
    globalSystem::runtimeData.addData( "Scene-height", DynamicFloatWrapper( ::configData._mapHeight, 128.0f, 2048.0f, 32.0f ) );
    globalSystem::runtimeData.addData( "FOV", DynamicFloatWrapper( 60.0f, 40.0f, 120.0f, 0.1f ) );
    //globalSystem::runtimeData.addData( "LOD", DynamicFloatWrapper( 0.0f, 0.0f, 10.0f, 1.0f ) );

    ::debugList = globalSystem::runtimeData.getVariablesArray();
    ::currentDebugVar = 0;

    //SDL_SetRelativeMouseMode( SDL_TRUE );


    std::cout << "=============================================\n";
    std::cout << "SCENE SETUP INFO" << std::endl;
    std::cout << "---------------------------------------------\n";
    std::cout << "Fov Angle: " << rtScene.fovAngle << std::endl;
    std::cout << "Screen Resolution: " << rtScene.screenResolution.x << " x " << rtScene.screenResolution.y << std::endl;
    std::cout << "Last mip level: " << rtScene.heightfield->getLastMipLevel() << std::endl;
    std::cout << "Compact Heightmap size: " << rtHeightfield.getCompactHeightmap()->getWidth() << " x " << rtScene.heightfield->getCompactHeightmap()->getHeight() << std::endl;
    std::cout << "Real dims: " << rtHeightfield.getRealHeightmapDimensions().x << " x " << rtHeightfield.getRealHeightmapDimensions().y << std::endl;
    std::cout << "Hmap dims: " << rtHeightfield.getHeightmapDimensions().x << " x " << rtHeightfield.getHeightmapDimensions().y << std::endl;

    std::cout << "Last LOD: " << rtScene.heightfield->getLastMipLevel() << std::endl;

    std::cout << "Screen Ratios: "; printArray( &rtScene.getScreenRatios()[ 0 ], 2 );

    std::cout << "Patch Size: " << rtScene.heightfield->getPatchSize() << std::endl;

    //globalSystem::window.maximizeWindow();

    if ( !::configData._camPathFileName.empty() ) {
        std::cout << "Camera Path File: " << ::configData._camPathFileName << " ";
        if ( !pathScript.readPathFromFile( ::configData._camPathFileName ) ) {
            std::cout << "FAIL" << std::endl;
            pathScript.clear();
        }
        else {
            std::cout << "OK" << std::endl;
        }
    }

    std::cout << "Num Frames: " << pathScript.getNumberOfFrames() << std::endl;

    return true;
}

void handleEvents() {
    globalSystem::keys.clear();

    while ( SDL_PollEvent( &events ) != 0 ) {
        //User requests quit
        if ( events.type == SDL_QUIT ) {
            quit = true;
        }
        else if ( events.type == SDL_KEYUP ) {
            globalSystem::keys.releaseKey( events.key.keysym.scancode );
        }
        else if ( events.type == SDL_KEYDOWN ) {
            // If first press, then register that key was pressed
            if ( events.key.repeat == 0 ) {
                globalSystem::keys.pressKey( events.key.keysym.scancode );
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_F1 ) {
                mouseRelative = !mouseRelative;
                if ( mouseRelative ) {
                    SDL_SetRelativeMouseMode( SDL_TRUE );
                }
                else {
                    SDL_SetRelativeMouseMode( SDL_FALSE );
                }
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_F2 ) {
                ::currentDebugVar = ( ::currentDebugVar + 1 ) % ::debugList.size();
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_F4 ) {
                if ( globalSystem::window.getWindowResolutionMode() == 0 ) {
                    //globalSystem::window.forceResize( selectedRes.first, selectedRes.second );
                    globalSystem::window.setFullscreen();
                }
                else {
                    //globalSystem::window.forceResize( selectedRes.first, selectedRes.second );
                    globalSystem::window.setWindowedMode();
                }
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_F5 ) {
                rtShader.closeShader();
                rtShader.loadShader();
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_F6 ) {
                ::showHUD = !::showHUD;
            }
			if ( events.key.keysym.scancode == SDL_SCANCODE_F7 ) {
				::configData._currentRes = ( ::configData._currentRes + 1 ) % ::configData._screenResolutions.size();
				std::pair< int, int > selectedRes = ::configData._screenResolutions[ ::configData._currentRes ];
				globalSystem::window.forceResize( selectedRes.first, selectedRes.second );
			}
            if ( events.key.keysym.scancode == SDL_SCANCODE_ESCAPE ) {
                quit = true;
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_L ) {
                if ( autoLightCycle ) {
                    autoLightCycle = !autoLightSide;
                }
                else {
                    autoLightCycle = true;
                }
                autoLightSide = true;
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_K ) {
                if ( autoLightCycle ) {
                    autoLightCycle = autoLightSide;
                }
                else {
                    autoLightCycle = true;
                }
                autoLightSide = false;
            }
            // Camera Path Record Mode ( 5 FPS )
            if ( events.key.keysym.scancode == SDL_SCANCODE_B ) {
                if ( camScriptMode == 0 ) {
                    camScriptMode = 1;
                    camScriptCounter = 0.0f;
                    pathScript.clear();
                    std::cout << "Camera Path record mode enabled" << std::endl;
                }
                else {
                    camScriptMode = 0;
                    std::string fileName = "path" + getCurrentTimeString();
                    replaceCharacters( fileName, " :", '-' );
                    pathScript.savePathToFile( fileName + ".cps" );
                    pathScript.clear();
                }
            }
            // Camera Replay Mode
            if ( events.key.keysym.scancode == SDL_SCANCODE_N ) {
                if ( camScriptMode == 0 ) {
                    camScriptMode = 2;
                    camScriptCounter = 0.0f;
                    std::cout << "Camera Path replay mode enabled" << std::endl;
                    recordFPS = true;
                    fpsRecord.clear();
                    rayRecord.clear();
                }
                else {
                    camScriptMode = 0;
                }
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_1 ) {
                globalSystem::runtimeData[ "Rendermode" ].setValue( 0.0 );
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_2 ) {
                globalSystem::runtimeData[ "Rendermode" ].setValue( 1.0 );
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_3 ) {
                globalSystem::runtimeData[ "Rendermode" ].setValue( 2.0 );
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_4  ) {
                globalSystem::runtimeData[ "Rendermode" ].setValue( 3.0 );
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_5 ) {
                globalSystem::runtimeData[ "Rendermode" ].setValue( 4.0 );
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_6 ) {
                globalSystem::runtimeData[ "Rendermode" ].setValue( 5.0 );
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_7 ) {
                globalSystem::runtimeData[ "Rendermode" ].setValue( 6.0 );
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_8 ) {
                globalSystem::runtimeData[ "Rendermode" ].setValue( 7.0 );
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_9 ) {
                globalSystem::runtimeData[ "Rendermode" ].setValue( 8.0 );
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_0 ) {
                globalSystem::runtimeData[ "Rendermode" ].setValue( 9.0 );
            }
            if ( events.key.keysym.scancode == SDL_SCANCODE_M ) {
                isMultisamplingOn = !isMultisamplingOn;
                if ( isMultisamplingOn ) {
                    std::cout << "Enabled multisampling" << std::endl;
                    glEnable( GL_MULTISAMPLE );

                    if ( glIsEnabled( GL_MULTISAMPLE ) == GL_TRUE ) {
                        std::cout << "It is indeed enabled" << std::endl;
                    }
                    else {
                        std::cout << "As expected, it did not work." << std::endl;
                    }
                }
                else {
                    std::cout << "Disabled multisampling" << std::endl;
                    glDisable( GL_MULTISAMPLE );
                }
            }
        }
        else if ( events.type == SDL_MOUSEWHEEL ) {
            if ( events.wheel.y > 0 ) {
                debugList[ ::currentDebugVar ].second->addDelta();
            }
            else if ( events.wheel.y < 0 ) {
                debugList[ ::currentDebugVar ].second->subDelta();
            }
        }
        else if ( events.type == SDL_MOUSEBUTTONDOWN ) {
            if ( events.button.button == SDL_BUTTON_MIDDLE ) {
                ::currentDebugVar = ( ::currentDebugVar + 1 ) % debugList.size();
            }
        }
        else if ( events.type == SDL_MOUSEMOTION ) {
            globalSystem::mouse.mouseCoords.setVector( (float)events.motion.x, (float)events.motion.y );
            if ( mouseRelative ) {
                ::_camAngleH = mathCalcs::loopInRange( ::_camAngleH - 0.1f * (float)events.motion.xrel, 0, 360 );
                ::_camAngleV = mathCalcs::clampToRange( ::_camAngleV - 0.1f * (float)events.motion.yrel, -89.99f, 89.99f );
            }
        }
        //Handle keypress with current mouse position
        else if ( events.type == SDL_TEXTINPUT ) {
            int x = 0, y = 0;
            SDL_GetMouseState( &x, &y );
            handleKeys( events.text.text[ 0 ], x, y );
        }
        else if ( events.type == SDL_WINDOWEVENT ) {
            if ( events.window.event == SDL_WINDOWEVENT_RESIZED ) {
                //globalSystem::window.eventResize( events.window.data1, events.window.data2 );
                /*globalSystem::window.eventResize( events.window.data1, events.window.data2 );

                rtScene.screenResolution.x = (float)globalSystem::window.getWidth();
                rtScene.screenResolution.y = (float)globalSystem::window.getHeight();*/
            }
            else if ( events.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ) {
                globalSystem::window.eventResize( events.window.data1, events.window.data2 );
                //std::cout << globalSystem::window.getWidth() << " " << globalSystem::window.getHeight() << std::endl;
                rtScene.screenResolution.x = (float)globalSystem::window.getWidth();
                rtScene.screenResolution.y = (float)globalSystem::window.getHeight();
                
                gMyFBO.destroy();
                gMyFBO.initializeFBO( events.window.data1, events.window.data2 );
            }
        }
    }
}

int qtAmount = 8;
void handleKeys( unsigned char key, int x, int y ) {
    if ( key == 'p' || key == 'P' ) {
        std::string fileName = "img" + getCurrentTimeString();
        replaceCharacters( fileName, " :", '-' );

        Texture::saveScreenshot( fileName, globalSystem::window.getWidth(), globalSystem::window.getHeight() );
    }
    else if ( toupper(key) == 'R' ) {
        if ( recordFPS ) {
            recordFPS = false;
            fpsRecord.clear();
            rayRecord.clear();
            std::cout << "FPS record was interrupted" << std::endl;
        }
        else {
            recordFPS = true;
            fpsRecord.clear();
            rayRecord.clear();
        }
    }
    else if ( toupper( key ) == 'T' ) {
        recordFPS = false;
        storeFPSrecord();
    }
    /*else if ( toupper( key ) == 'T' ) {
        glBindBuffer
    }*/
}

float tElapsed = 0.0f;
void update( int deltaTime ) {
    rtScene.sceneHeight = globalSystem::runtimeData.getData( "Scene-height" )->getValue();
    rtScene.farZ = globalSystem::runtimeData.getData( "Scene-far" )->getValue();
    auto vU = rtScene.getU();
    auto vV = rtScene.getV();
    auto vN = rtScene.getN();

    /*std::cout << " (" << vU.x << " " << vU.y << " " << vU.z << ") ";
    std::cout << " (" << vV.x << " " << vV.y << " " << vV.z << ") ";
    std::cout << " (" << vN.x << " " << vN.y << " " << vN.z << ") "  << std::endl;*/
    //std::cout << rtScene.getScreenRatios().x << " " << rtScene.getScreenRatios().y << std::endl;

    float fdtime = deltaTime / 1000.f;
    globalSystem::time.deltaTime = fdtime;
    tElapsed += fdtime;

    rtScene.cameraDirection = createDirectionVectorZup( ::_camAngleH, ::_camAngleV );

    auto sdlkeyboard = SDL_GetKeyboardState(NULL);
    Vector3D camMovement{ 0.0f };
    bool moved = false;
    if ( sdlkeyboard[ SDL_SCANCODE_A ] || sdlkeyboard[ SDL_SCANCODE_LEFT ] ) {
        moved = true;
        camMovement.x -= 1.0f;
    }
    if ( sdlkeyboard[ SDL_SCANCODE_D ] || sdlkeyboard[ SDL_SCANCODE_RIGHT ] ) {
        moved = true;
        camMovement.x += 1.0f;
    }
    if ( sdlkeyboard[ SDL_SCANCODE_S ] || sdlkeyboard[ SDL_SCANCODE_DOWN ] ) {
        moved = true;
        camMovement.y -= 1.0f;
    }
    if ( sdlkeyboard[ SDL_SCANCODE_W ] || sdlkeyboard[ SDL_SCANCODE_UP ] ) {
        moved = true;
        camMovement.y += 1.0f;
    }
    if ( sdlkeyboard[ SDL_SCANCODE_SPACE ] ) {
        moved = true;
        camMovement.z += 1.0f;
    }
    if ( sdlkeyboard[ SDL_SCANCODE_LCTRL ] ) {
        moved = true;
        camMovement.z -= 1.0f;
    }

    if ( moved ) {
        camMovement.normalize();
        camMovement *= globalSystem::runtimeData.getData( "CameraSpeed" )->getValue() * (float)deltaTime / 1000.f;

        rtScene.cameraEye += ( rtScene.cameraDirection * camMovement.y ) +
            ( createDirectionVectorZup( ::_camAngleH - 90.0f, 0.0f ) * camMovement.x ) +
            ( createDirectionVectorZup( 0.0f, 90.0f ) * camMovement.z );
    }

    if ( sdlkeyboard[ SDL_SCANCODE_KP_PLUS ] ) {
        debugList[ ::currentDebugVar ].second->addDelta();
    }
    if ( sdlkeyboard[ SDL_SCANCODE_KP_MINUS ] ) {
        debugList[ ::currentDebugVar ].second->subDelta();
    }

    if ( camScriptMode == 1 ) {
        camScriptCounter += fdtime;
        if ( camScriptCounter >= 0.2f ) {
            camScriptCounter -= 0.2f;
            pathScript.addFrame( rtScene.cameraEye, rtScene.cameraDirection );
            std::cout << "Frame " << pathScript.getNumberOfFrames() << std::endl;
            if ( pathScript.getNumberOfFrames() >= 500 ) {
                camScriptMode = 0;
                std::cout << "Path Script 500 frame limit was reached! Disabling path recording." << std::endl;
            }
        }
    }
    else if ( ::camScriptMode == 2 ) {
        camScriptCounter += fdtime * 5.0;

        rtScene.cameraEye = pathScript.getPosition( camScriptCounter );
        rtScene.cameraDirection = pathScript.getCameraDirection( camScriptCounter );

        if ( camScriptCounter >= (pathScript.getNumberOfFrames()-1) ) {
            camScriptMode = 0;
            storeFPSrecord();
            recordFPS = false;
        }
    }

    rtScene.fovAngle = globalSystem::runtimeData.getData( "FOV" )->getValue();
    rtScene.precomputeUVN();


    if ( autoLightCycle ) {
        auto lAng = globalSystem::runtimeData.getData( "Light-angle" );
        if ( autoLightSide ) {
            lAng->addDelta();
            if ( lAng->getValue() >= 85.0 ) {
                autoLightCycle = false;
            }
        }
        else {
            lAng->subDelta();
            if ( lAng->getValue() <= -85.0 ) {
                autoLightCycle = false;
            }
        }
    }

    // Custom light
    float lightAngle = mathCalcs::degreesToRadians( globalSystem::runtimeData.getData( "Light-angle" )->getValue() );
    Vector3D lightDirection = Vector3D( sin( lightAngle ), -0.2f, -cos( lightAngle ) );
    rtScene.lightDirection = lightDirection;
    //std::cout << "Light Vector: " << lightDirection.x << " " << lightDirection.y << " " << lightDirection.z << std::endl;

    // Updatable parameters
    rtScene.lodZParam = globalSystem::runtimeData.getData( "Lod-Z-param" )->getValue();

    rtHeightfield._userPosition = rtScene.cameraEye;
    rtHeightfield.update( fdtime );
    rtScene.cameraEye = rtHeightfield._userPosition;
}

void render() {
    int windowW = globalSystem::window.getWidth();
    int windowH = globalSystem::window.getHeight();

    glViewport( 0, 0, windowW, windowH );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if ( checkMaxMipmap ) {
        glDisable( GL_DEPTH_TEST );
        debugShader.useProgram();

        GLuint ulTex = debugShader.getUniformLocation( "tex" );
        GLuint ulLod = debugShader.getUniformLocation( "lod" );

        glUniform1i( ulTex, 0 );
        glUniform1f( ulLod, ::debugList[1].second->getValue() );

        rtScene.heightfield->getMaximumMipmap()->bindTexture( 0 );
        myrt::screenQuad.render();
        Texture::unbindTexture( 0 );

        debugShader.disableProgram();
        glEnable( GL_DEPTH_TEST );
    }
    else if ( rtShader.isLoaded() ) {
        rtScene.renderMode = (RTRenderMode)( (int)globalSystem::runtimeData.getData( "Rendermode" )->getValue() );
        rtShader.renderScene( rtScene );
        ::rayAccumulator += rtShader.getNumRays();

        if ( ::showHUD ) {
            // Render debug text
            Font::shader.useProgram();
            setOrthoMatrix( Font::projection.getMatrix(), 0.f, (float)windowW, 0.f, (float)windowH, -1.f, 1.f );
            setTranslationMatrix( Font::model.getMatrix(), 16.f, (float)windowH - 20.f, 0.f );
            Font::setMatrixes();
            Font::setFontColor( Color4C( 255, 255, 255, 255 ) );
            globalSystem::gameFont.renderTextLeft( "FPS:" + ( recordFPS ? "[" + std::to_string( measuredFrameRate ) + "]" : std::to_string( measuredFrameRate ) ) + "\n" +
                ::debugList[ ::currentDebugVar ].first + " = " + floatToString( ::debugList[ ::currentDebugVar ].second->getValue() ) + "\n" +
                "Position: " + floatToString( rtScene.cameraEye.x ) + ", " + floatToString( rtScene.cameraEye.y ) + ", " + floatToString( rtScene.cameraEye.z ) + "\n" +
                "Rays Per Second: " + intToString( measureRayPerSecond ) );

            Font::shader.disableProgram();
        }
    }
    else {
        // Render debug text
        Font::shader.useProgram();
        setOrthoMatrix( Font::projection.getMatrix(), 0.f, (float)windowW, 0.f, (float)windowH, -1.f, 1.f );
        setTranslationMatrix( Font::model.getMatrix(), 16.f, (float)windowH - 20.f, 0.f );
        Font::setMatrixes();
        Font::setFontColor( Color4C( 255, 255, 255, 255 ) );
        globalSystem::gameFont.renderTextLeft( "FPS:" + std::to_string( measuredFrameRate ) + "\n" +
            ::debugList[ ::currentDebugVar ].first + " = " + floatToString( ::debugList[ ::currentDebugVar ].second->getValue() ) );

        setTranslationMatrix( Font::model.getMatrix(), 32.0f, 192.0f, 0.f );
        Font::setMatrixes();
        globalSystem::gameFont.renderTextLeft( "Failed to load shaders\n" + rtShader.getLastError(), windowW - 64.0f );

        Font::shader.disableProgram();
    }

    // Finish rendering
    globalSystem::window.swapBuffer();
}

void close() {
    /*if ( getOpenGLError( "Error enabling OpenGL systems." ) ) {
        Logging::error.reportError( "Epic error undetected." );
        exit( 0 );
    }*/

    //AudioSystem::instance.cleanAudio();

    globalSystem::textures.clear();

    globalSystem::gameFont.freeFont();

    Font::closeFontSystem();

    gMyFBO.destroy();

    //Destroy window	
    globalSystem::window.closeWindow();

    //Quit SDL subsystems
    SDL_Quit();
}

void storeFPSrecord() {
    std::string fileName = "perfTest" + getCurrentTimeString() + ".txt";
    replaceCharacters( fileName, " :", '-' );
    
    std::ofstream rFile{ fileName };
    int average, minimum, maximum;
    average = 0;
    minimum = 999999999;
    maximum = 0;
    //rFile << 
    
    rFile << "Terrain: " << ::hfName << "\n";
    rFile << "Screen Resolution: " << globalSystem::window.getWidth() << " x " << globalSystem::window.getHeight() << "\n";
    rFile << "Heightmap Resolution: " << rtHeightfield.getRealHeightmapDimensions().x << " x " << rtHeightfield.getRealHeightmapDimensions().y << std::endl;
    rFile << "Heightmap max altitude: " << rtHeightfield.getMapHeight() << "\n";
    rFile << "RenderMode: " << getRenderModeString( rtScene.renderMode ) << "\n";
    rFile << "Scene-far: " << rtScene.farZ << "\n";
    rFile << "FOV: " << rtScene.fovAngle << "\n";
    rFile << "\n";
    rFile << "FPS Test" << std::endl;
    for ( int i = 0; i < fpsRecord.size(); ++i ) {
        rFile << fpsRecord[ i ] << "\n";
        average += fpsRecord[ i ];
        maximum = mathCalcs::max( maximum, fpsRecord[ i ] );
        minimum = mathCalcs::min( minimum, fpsRecord[ i ] );
    }
    average /= fpsRecord.size();
    rFile << "Average FPS: " << average << "\n";
    rFile << "Minimum FPS: " << minimum << "\n";
    rFile << "Maximum FPS: " << maximum << "\n";

    rFile << "\n\nRays/Second\n";
    unsigned int averageR, minimumR, maximumR;
    averageR = 0;
    minimumR = 99999999999;
    maximumR = 0;
    for ( int i = 0; i < rayRecord.size(); ++i ) {
        rFile << rayRecord[ i ] << "\n";
        float ratio = ( 1.0 / float( i + 1 ) );
        averageR = unsigned int( float( averageR ) * ratio + (float)rayRecord[ i ] * ( 1.0 - ratio ) );
        maximumR = mathCalcs::max( maximumR, rayRecord[ i ] );
        minimumR = mathCalcs::min( minimumR, rayRecord[ i ] );
    }
    rFile << "Average RPS: " << averageR << "\n";
    rFile << "Minimum RPS: " << minimumR << "\n";
    rFile << "Maximum RPS: " << maximumR << "\n";
    rFile.close();
}