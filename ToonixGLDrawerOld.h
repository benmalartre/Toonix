//-----------------------------------------------
//  Toonix GL Drawer
//-----------------------------------------------
#include "ToonixGLShape.h"

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
	void DrawCreaseVertices();
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
