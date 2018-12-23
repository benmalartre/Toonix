// ToonixGLDrawerSequencer
//------------------------------------
#include "ToonixGLDrawer.h"

// VBO extension function pointers

TXGLShapeManager G_TXGLShapeManager;


void TXGLTest()
{
	int lats = 12;
	int longs = 12;
	double r = 7;

	int i, j;
	for(i = 0; i <= lats; i++) 
	{
		double lat0 = PI * (-0.5 + (double) (i - 1) / lats);
		double z0  = sin(lat0);
		double zr0 =  cos(lat0);
	    
		double lat1 = PI * (-0.5 + (double) i / lats);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);
	    

		for(j = 0; j <= longs; j++) 
		{
			double lng = 2 * PI * (double) (j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);
	    
			
			//glNormal3f(x * zr0*r, y * zr0*r, z0*r);
			//glVertex3f(x * zr0*r, y * zr0*r, z0*r);
			//glNormal3f(x * zr1*r, y * zr1*r, z1*r);
			//glVertex3f(x * zr1*r, y * zr1*r, z1*r);
		}

	}
}


void ToonixGLDrawer_Init ( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{

	int initGL3W = gl3wInit();

	// Cast the CRef into a GraphicSequencerContext SDK object
	XSI::GraphicSequencerContext l_vGraphicSequencerContext = in_pSequencerContext;
	
	// make sure the cast succeeded
	assert ( l_vGraphicSequencerContext.IsValid() );

	// get the sequencer from the context object
	XSI::CGraphicSequencer in_pSequencer = l_vGraphicSequencerContext.GetGraphicSequencer ();

	// use the sequencer to register the callback
	in_pSequencer.RegisterDisplayCallback(L"ToonixGLDrawer",0,XSI::siPostPass,XSI::siCustom,L"ToonixPass");
	//in_pSequencer.RegisterDisplayCallback ( L"ToonixGLDrawer", 0, XSI::siPostEndFrame, XSI::siRealtimePortMaterial, L"ToonixPass");

}

void ToonixGLDrawer_Execute( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
	Application app;

	GraphicSequencerContext oGraphicSequencerContext = in_pSequencerContext;
	CGraphicSequencer oSequencer = oGraphicSequencerContext.GetGraphicSequencer();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	// Set alpha function (discard pixels with alpha == 0.0f)
	//glAlphaFunc( GL_GREATER, 0.0f );

	//glPushMatrix();

	G_TXGLShapeManager.DrawAll();

	//glPopMatrix();
	glFlush();
}

void ToonixGLDrawer_Term( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
	G_TXGLShapeManager.FlushAll();
}

void ToonixGLDrawer_InitInstance( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
}

void ToonixGLDrawer_TermInstance( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
}