#ifndef TOONIX_GL_H
#define TOONIX_GL_H

// OpenGL includes
//----------------------------------
#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <GL/gl3w.h>

#ifdef _MSC_VER
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"user32.lib")
#endif

bool IsGLExtensionSupported(const char* extname);

#endif
