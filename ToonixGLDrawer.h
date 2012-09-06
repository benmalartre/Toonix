//-----------------------------------------------
//  Toonix GL Drawer
//-----------------------------------------------
#include "ToonixRegister.h"
#include "ToonixCamera.h"

#ifndef unix
#if _MSC_VER>1000
#pragma once
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

//#include <GL/glaux.h>
//#include <GL/glext.h>

#ifdef _MSC_VER
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"user32.lib")
#endif


void GetWindowSize(int& width, int& height);
bool IsExtensionSupported(char* extensionName);

class TXCamera;

class TXGLDrawer
{
public:
	TXGLDrawer(){_camera=NULL;};
	~TXGLDrawer(){};
	TXCamera* _camera;
	TXGeometry* _geom;

	void SetGeometry(TXGeometry* geom);
	void SetCamera(TXCamera* cam);
	void DrawFrustrum();
	void DrawLines(); 
	void DrawPlanes();
	void DrawNormals();
	void DrawBoundingSpheres();
	void DrawSphere(double radius,int lats,int longs);
};

extern TXGLDrawer _drawer;

#ifndef unix
#ifdef TOONIXGLDRAWER_EXPORTS
#define TOONIXGLDRAWERCALLBACK_API __declspec(dllexport)
#else
#define TOONIXGLDRAWERCALLBACK_API __declspec(dllimport)
#endif
#else
#define TOONIXGLDRAWERCALLBACK_API
#define LPVOID void*
#endif

// we can safely ignore this warning, since we know that the 
// implementation of these functions will be done in C++
#ifdef _WIN32
#pragma warning(disable:4190)
#endif

#ifdef __cplusplus
extern "C" 
{

// ToonixGLDrawer
TOONIXGLDRAWERCALLBACK_API void	ToonixGLDrawer_Execute		( XSI::CRef, LPVOID * );
TOONIXGLDRAWERCALLBACK_API void	ToonixGLDrawer_Init			( XSI::CRef, LPVOID * );
TOONIXGLDRAWERCALLBACK_API void	ToonixGLDrawer_Term			( XSI::CRef, LPVOID * );
TOONIXGLDRAWERCALLBACK_API void	ToonixGLDrawer_InitInstance	( XSI::CRef, LPVOID * );
TOONIXGLDRAWERCALLBACK_API void	ToonixGLDrawer_TermInstance	( XSI::CRef, LPVOID * );

// ToonixDebugPass
TOONIXGLDRAWERCALLBACK_API void	ToonixDebugPass_Execute			( XSI::CRef, LPVOID * );
TOONIXGLDRAWERCALLBACK_API void	ToonixDebugPass_Init			( XSI::CRef, LPVOID * );
TOONIXGLDRAWERCALLBACK_API void	ToonixDebugPass_Term			( XSI::CRef, LPVOID * );
TOONIXGLDRAWERCALLBACK_API void	ToonixDebugPass_InitInstance	( XSI::CRef, LPVOID * );
TOONIXGLDRAWERCALLBACK_API void	ToonixDebugPass_TermInstance	( XSI::CRef, LPVOID * );

};
#endif
