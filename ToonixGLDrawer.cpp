//--------------------------------------
// ToonixGLDrawer.cpp
//--------------------------------------
#include "ToonixGLDrawer.h"
#include "ToonixCamera.h"

TXGLDrawer _drawer;

void TXGLDrawer::SetCamera(TXCamera* cam)
{
	_camera = cam;
}

void TXGLDrawer::SetGeometry(TXGeometry* geom)
{
	_geom = geom;
}

void TXGLDrawer::DrawSphere(double r, int lats, int longs) 
{
	int i, j;
	for(i = 0; i <= lats; i++) 
	{
		double lat0 = PI * (-0.5 + (double) (i - 1) / lats);
		double z0  = sin(lat0);
		double zr0 =  cos(lat0);
	    
		double lat1 = PI * (-0.5 + (double) i / lats);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);
	    
		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= longs; j++) 
		{
			double lng = 2 * PI * (double) (j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);
	    
			glNormal3f(x * zr0*r, y * zr0*r, z0*r);
			glVertex3f(x * zr0*r, y * zr0*r, z0*r);
			glNormal3f(x * zr1*r, y * zr1*r, z1*r);
			glVertex3f(x * zr1*r, y * zr1*r, z1*r);
		}
		glEnd();
	}
}

void TXGLDrawer::DrawBoundingSpheres()
{
	std::vector<TXGeometry*>::iterator it = _geom->_subgeometries.begin();
	
	glEnable (GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for(;it<_geom->_subgeometries.end();it++)
	{
		Application().LogMessage(L"Sub Geometrie In Frustrum : "+(CString)(*it)->_infrustrum);
		//OUT
		if((*it)->_infrustrum == OUTSIDE)glColor4f(1,0,0,0.1);
		//INTERSECT
		else if((*it)->_infrustrum == INTERSECT)glColor4f(0,1,0,0.1);
		//IN
		else if((*it)->_infrustrum == INSIDE)glColor4f(0,0,1,0.1);
		//Error
		else glColor4f(0,0,0,1);

		glPushMatrix();
		glTranslatef((*it)->_spherecenter.GetX(),(*it)->_spherecenter.GetY(),(*it)->_spherecenter.GetZ());
		DrawSphere((*it)->_sphereradius,12, 12); 
		glPopMatrix();
	}
	glDisable(GL_BLEND);
}
void TXGLDrawer::DrawFrustrum()
{
	//Set Size
	glEnable(GL_POINT_SMOOTH);
	glPointSize(4.0f);

	// set current color
	glColor3f(1, 0, 0);

	//draw point
	glBegin(GL_POINTS);
		glVertex3f(_camera->_ntl.GetX(),_camera->_ntl.GetY(),_camera->_ntl.GetZ());
		glVertex3f(_camera->_ntr.GetX(),_camera->_ntr.GetY(),_camera->_ntr.GetZ());
		glVertex3f(_camera->_nbl.GetX(),_camera->_nbl.GetY(),_camera->_nbl.GetZ());
		glVertex3f(_camera->_nbr.GetX(),_camera->_nbr.GetY(),_camera->_nbr.GetZ());

		glVertex3f(_camera->_ftl.GetX(),_camera->_ftl.GetY(),_camera->_ftl.GetZ());
		glVertex3f(_camera->_ftr.GetX(),_camera->_ftr.GetY(),_camera->_ftr.GetZ());
		glVertex3f(_camera->_fbl.GetX(),_camera->_fbl.GetY(),_camera->_fbl.GetZ());
		glVertex3f(_camera->_fbr.GetX(),_camera->_fbr.GetY(),_camera->_fbr.GetZ());
	glEnd();

	DrawLines();
	DrawPlanes();
	DrawNormals();
}

void TXGLDrawer::DrawLines() 
{
	// set current color
	glColor3f(0.7, 0.3, 0.3);
	glBegin(GL_LINE_LOOP);
	//near plane
		glVertex3f(_camera->_ntl.GetX(),_camera->_ntl.GetY(),_camera->_ntl.GetZ());
		glVertex3f(_camera->_ntr.GetX(),_camera->_ntr.GetY(),_camera->_ntr.GetZ());
		glVertex3f(_camera->_nbr.GetX(),_camera->_nbr.GetY(),_camera->_nbr.GetZ());
		glVertex3f(_camera->_nbl.GetX(),_camera->_nbl.GetY(),_camera->_nbl.GetZ());
	glEnd();

	glBegin(GL_LINE_LOOP);
	//far plane
		glVertex3f(_camera->_ftr.GetX(),_camera->_ftr.GetY(),_camera->_ftr.GetZ());
		glVertex3f(_camera->_ftl.GetX(),_camera->_ftl.GetY(),_camera->_ftl.GetZ());
		glVertex3f(_camera->_fbl.GetX(),_camera->_fbl.GetY(),_camera->_fbl.GetZ());
		glVertex3f(_camera->_fbr.GetX(),_camera->_fbr.GetY(),_camera->_fbr.GetZ());
	glEnd();

	glBegin(GL_LINE_LOOP);
	//bottom plane
		glVertex3f(_camera->_nbl.GetX(),_camera->_nbl.GetY(),_camera->_nbl.GetZ());
		glVertex3f(_camera->_nbr.GetX(),_camera->_nbr.GetY(),_camera->_nbr.GetZ());
		glVertex3f(_camera->_fbr.GetX(),_camera->_fbr.GetY(),_camera->_fbr.GetZ());
		glVertex3f(_camera->_fbl.GetX(),_camera->_fbl.GetY(),_camera->_fbl.GetZ());
	glEnd();

	glBegin(GL_LINE_LOOP);
	//top plane
		glVertex3f(_camera->_ntr.GetX(),_camera->_ntr.GetY(),_camera->_ntr.GetZ());
		glVertex3f(_camera->_ntl.GetX(),_camera->_ntl.GetY(),_camera->_ntl.GetZ());
		glVertex3f(_camera->_ftl.GetX(),_camera->_ftl.GetY(),_camera->_ftl.GetZ());
		glVertex3f(_camera->_ftr.GetX(),_camera->_ftr.GetY(),_camera->_ftr.GetZ());
	glEnd();

	glBegin(GL_LINE_LOOP);
	//left plane
		glVertex3f(_camera->_ntl.GetX(),_camera->_ntl.GetY(),_camera->_ntl.GetZ());
		glVertex3f(_camera->_nbl.GetX(),_camera->_nbl.GetY(),_camera->_nbl.GetZ());
		glVertex3f(_camera->_fbl.GetX(),_camera->_fbl.GetY(),_camera->_fbl.GetZ());
		glVertex3f(_camera->_ftl.GetX(),_camera->_ftl.GetY(),_camera->_ftl.GetZ());
	glEnd();

	glBegin(GL_LINE_LOOP);
	// right plane
		glVertex3f(_camera->_nbr.GetX(),_camera->_nbr.GetY(),_camera->_nbr.GetZ());
		glVertex3f(_camera->_ntr.GetX(),_camera->_ntr.GetY(),_camera->_ntr.GetZ());
		glVertex3f(_camera->_ftr.GetX(),_camera->_ftr.GetY(),_camera->_ftr.GetZ());
		glVertex3f(_camera->_fbr.GetX(),_camera->_fbr.GetY(),_camera->_fbr.GetZ());
	glEnd();
}

void TXGLDrawer::DrawPlanes() 
{
	// set current color
	glEnable (GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.5, 0, .25,0.05);
	glBegin(GL_QUADS);

	//near plane
		glVertex3f(_camera->_ntl.GetX(),_camera->_ntl.GetY(),_camera->_ntl.GetZ());
		glVertex3f(_camera->_ntr.GetX(),_camera->_ntr.GetY(),_camera->_ntr.GetZ());
		glVertex3f(_camera->_nbr.GetX(),_camera->_nbr.GetY(),_camera->_nbr.GetZ());
		glVertex3f(_camera->_nbl.GetX(),_camera->_nbl.GetY(),_camera->_nbl.GetZ());

	//far plane
		glVertex3f(_camera->_ftr.GetX(),_camera->_ftr.GetY(),_camera->_ftr.GetZ());
		glVertex3f(_camera->_ftl.GetX(),_camera->_ftl.GetY(),_camera->_ftl.GetZ());
		glVertex3f(_camera->_fbl.GetX(),_camera->_fbl.GetY(),_camera->_fbl.GetZ());
		glVertex3f(_camera->_fbr.GetX(),_camera->_fbr.GetY(),_camera->_fbr.GetZ());

	//bottom plane
		glVertex3f(_camera->_nbl.GetX(),_camera->_nbl.GetY(),_camera->_nbl.GetZ());
		glVertex3f(_camera->_nbr.GetX(),_camera->_nbr.GetY(),_camera->_nbr.GetZ());
		glVertex3f(_camera->_fbr.GetX(),_camera->_fbr.GetY(),_camera->_fbr.GetZ());
		glVertex3f(_camera->_fbl.GetX(),_camera->_fbl.GetY(),_camera->_fbl.GetZ());

	//top plane
		glVertex3f(_camera->_ntr.GetX(),_camera->_ntr.GetY(),_camera->_ntr.GetZ());
		glVertex3f(_camera->_ntl.GetX(),_camera->_ntl.GetY(),_camera->_ntl.GetZ());
		glVertex3f(_camera->_ftl.GetX(),_camera->_ftl.GetY(),_camera->_ftl.GetZ());
		glVertex3f(_camera->_ftr.GetX(),_camera->_ftr.GetY(),_camera->_ftr.GetZ());

	//left plane
		glVertex3f(_camera->_ntl.GetX(),_camera->_ntl.GetY(),_camera->_ntl.GetZ());
		glVertex3f(_camera->_nbl.GetX(),_camera->_nbl.GetY(),_camera->_nbl.GetZ());
		glVertex3f(_camera->_fbl.GetX(),_camera->_fbl.GetY(),_camera->_fbl.GetZ());
		glVertex3f(_camera->_ftl.GetX(),_camera->_ftl.GetY(),_camera->_ftl.GetZ());

	// right plane
		glVertex3f(_camera->_nbr.GetX(),_camera->_nbr.GetY(),_camera->_nbr.GetZ());
		glVertex3f(_camera->_ntr.GetX(),_camera->_ntr.GetY(),_camera->_ntr.GetZ());
		glVertex3f(_camera->_ftr.GetX(),_camera->_ftr.GetY(),_camera->_ftr.GetZ());
		glVertex3f(_camera->_fbr.GetX(),_camera->_fbr.GetY(),_camera->_fbr.GetZ());

	glEnd();
	glDisable(GL_BLEND);

}

void TXGLDrawer::DrawNormals()
{
	CVector3f a,b;
	glColor3f(1,1,0);
	glBegin(GL_LINES);
		// near
		//a = ntr + ntl + nbr + nbl;
		a.Add(_camera->_ntr,_camera->_ntl);
		a.AddInPlace(_camera->_nbr);
		a.AddInPlace(_camera->_nbl);
		a.ScaleInPlace(0.25);
		b.Add(_camera->_planes[NEARP]._norm, a );
		glVertex3f(a.GetX(),a.GetY(),a.GetZ());
		glVertex3f(b.GetX(),b.GetY(),b.GetZ());

		// far
		a.Add(_camera->_ftr,_camera->_ftl);
		a.AddInPlace(_camera->_fbr);
		a.AddInPlace(_camera->_fbl);
		a.ScaleInPlace(0.25);
		b.Add(_camera->_planes[FARP]._norm, a );
		glVertex3f(a.GetX(),a.GetY(),a.GetZ());
		glVertex3f(b.GetX(),b.GetY(),b.GetZ());

		// left
		a.Add(_camera->_ftl,_camera->_fbl);
		a.AddInPlace(_camera->_nbl);
		a.AddInPlace(_camera->_ntl);
		a.ScaleInPlace(0.25);
		//b.Add( a , _camera->_planes[LEFT]._norm);
		b.Add(_camera->_planes[LEFT]._norm, a );
		glVertex3f(a.GetX(),a.GetY(),a.GetZ());
		glVertex3f(b.GetX(),b.GetY(),b.GetZ());
		
		// right
		a.Add(_camera->_ftr,_camera->_nbr);
		a.AddInPlace(_camera->_fbr);
		a.AddInPlace(_camera->_ntr);
		a.ScaleInPlace(0.25);
		//b.Add( a , _camera->_planes[RIGHT]._norm);
		b.Add(_camera->_planes[RIGHT]._norm, a );
		glVertex3f(a.GetX(),a.GetY(),a.GetZ());
		glVertex3f(b.GetX(),b.GetY(),b.GetZ());
		
		// top
		a.Add(_camera->_ftr,_camera->_ftl);
		a.AddInPlace(_camera->_ntr);
		a.AddInPlace(_camera->_ntl);
		a.ScaleInPlace(0.25);
		//b.Add( a , _camera->_planes[TOP]._norm);
		b.Add(_camera->_planes[TOP]._norm, a );
		glVertex3f(a.GetX(),a.GetY(),a.GetZ());
		glVertex3f(b.GetX(),b.GetY(),b.GetZ());
		
		// bottom
		a.Add(_camera->_fbr,_camera->_fbl);
		a.AddInPlace(_camera->_nbr);
		a.AddInPlace(_camera->_nbl);
		a.ScaleInPlace(0.25);
		//b.Add( a , _camera->_planes[BOTTOM]._norm);
		b.Add(_camera->_planes[BOTTOM]._norm, a );
		glVertex3f(a.GetX(),a.GetY(),a.GetZ());
		glVertex3f(b.GetX(),b.GetY(),b.GetZ());
	glEnd();

}

void ToonixGLDrawer_Init ( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
	Application().LogMessage(L"Init Toonix GL Drawer Called");

	// Cast the CRef into a GraphicSequencerContext SDK object
	XSI::GraphicSequencerContext l_vGraphicSequencerContext = in_pSequencerContext;

	// Get the sequencer from the context object
	XSI::CGraphicSequencer in_pSequencer = l_vGraphicSequencerContext.GetGraphicSequencer ();

	CStringArray viewModeList;
	viewModeList.Add(L"ToonixDebugPass");
	// Use the sequencer to register the callback
	in_pSequencer.RegisterViewportCallback(L"ToonixGLDrawer",0,XSI::siPreEndFrame,XSI::siOpenGL20,XSI::siCustom,viewModeList);
	//in_pSequencer.RegisterViewportMode(L"ToonixDebugPass",XSI::siOpenGL20,0);

}

void ToonixGLDrawer_Execute( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
	Application app;

	GraphicSequencerContext oGraphicSequencerContext = in_pSequencerContext;
	CGraphicSequencer oSequencer = oGraphicSequencerContext.GetGraphicSequencer();

	if(_drawer._camera == NULL)
		return;

	// Set alpha function (discard pixels with alpha == 0.0f)
	//glAlphaFunc( GL_GREATER, 0.0f );

	glPushMatrix();
	_drawer.DrawFrustrum();
	_drawer.DrawBoundingSpheres();
	glPopMatrix();
}

void ToonixGLDrawer_Term( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
	_drawer._camera = NULL;
	_drawer._geom = NULL;
	Application().LogMessage(L"ToonixGLDrawer_Term Called");
}

void ToonixGLDrawer_InitInstance( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
	//Application().LogMessage(L"ToonixGLDrawer_InitInstance Called");
}

void ToonixGLDrawer_TermInstance( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
	_drawer._camera = NULL;
	_drawer._geom = NULL;
	//Application().LogMessage(L"ToonixGLDrawer_TermInstance Called");
}
