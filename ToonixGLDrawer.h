// ToonixGLDrawer.h
#ifndef TOONIX_GLDRAWER_H
#define TOONIX_GLDRAWER_H

#include "ToonixGLManager.h"

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

	TOONIXGLDRAWERCALLBACK_API XSI::CStatus XSILoadPlugin( XSI::PluginRegistrar& in_reg );
	TOONIXGLDRAWERCALLBACK_API XSI::CStatus XSIUnloadPlugin( const XSI::PluginRegistrar& in_reg );

	// ToonixGLDrawer
	TOONIXGLDRAWERCALLBACK_API void	ToonixGLDrawer_Execute		( XSI::CRef, LPVOID * );
	TOONIXGLDRAWERCALLBACK_API void	ToonixGLDrawer_Init			( XSI::CRef, LPVOID * );
	TOONIXGLDRAWERCALLBACK_API void	ToonixGLDrawer_Term			( XSI::CRef, LPVOID * );
	TOONIXGLDRAWERCALLBACK_API void	ToonixGLDrawer_InitInstance	( XSI::CRef, LPVOID * );
	TOONIXGLDRAWERCALLBACK_API void	ToonixGLDrawer_TermInstance	( XSI::CRef, LPVOID * );

	// ToonixPass
	TOONIXGLDRAWERCALLBACK_API void	ToonixPass_Execute			( XSI::CRef, LPVOID * );
	TOONIXGLDRAWERCALLBACK_API void	ToonixPass_Init				( XSI::CRef, LPVOID * );
	TOONIXGLDRAWERCALLBACK_API void	ToonixPass_Term				( XSI::CRef, LPVOID * );
	TOONIXGLDRAWERCALLBACK_API void	ToonixPass_InitInstance		( XSI::CRef, LPVOID * );
	TOONIXGLDRAWERCALLBACK_API void	ToonixPass_TermInstance		( XSI::CRef, LPVOID * );
	};

#endif

bool IsGLExtensionSupported(const char* extName);

#endif //_TOONIX_GLDRAWER_H_


