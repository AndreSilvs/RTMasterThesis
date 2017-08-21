#pragma once

#define NOMINMAX

#include <SDL.h>
//#include <SDL_opengl.h>

#include <IL/il.h>
#include <IL/ilu.h>

#define ALLOW_GL_DEBUG true

#ifdef IN_IDE_PARSER
# define GL_GLEXT_PROTOTYPES
# include "GL/gl.h"
#else
# include "glad.h"

#endif