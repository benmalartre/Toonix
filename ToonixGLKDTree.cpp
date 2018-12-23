//-------------------------------------------------
// TXGLKDTree.cpp
//-------------------------------------------------
#include "ToonixGL.h"
#include "ToonixGLKDTree.h"
#include <GL/GL.h>

//kd-tree debugging method

void TXGLKDTree::DrawNode(const TXKDTree* t,const TXGeometry* geom)
{
	CRandom rnd;

	Application().LogMessage(L"Drawing GL KDTree...");
	
	if(!t->m_left && !t->m_right)
	{
		Application().LogMessage(L"Leaf Node have "+(CString)(ULONG)t->m_points.size()+L" Points Inside");
		//glBegin(GL_POINTS);
		//glColor3f(rnd.GetNormalizedValue(),rnd.GetNormalizedValue(),rnd.GetNormalizedValue());
		
		//for(int i=0;i<geom->_nbv;i++)
			//glVertex3f((GLfloat)geom->_vertices[i]->_pos.GetX(),(GLfloat)geom->_vertices[i]->_pos.GetY(),(GLfloat)geom->_vertices[i]->_pos.GetZ());

		//glEnd();
		
		/*
		glDisable(GL_CULL_FACE);
		glBegin(GL_QUADS);
		glColor4f(1.0,0.0,0.0,0.1);

		// Left Face
		glVertex3f(t->_min.GetX(),t->_min.GetY(),t->_min.GetZ());
		glVertex3f(t->_min.GetX(),t->_min.GetY(),t->_max.GetZ());
		glVertex3f(t->_min.GetX(),t->_max.GetY(),t->_max.GetZ());
		glVertex3f(t->_min.GetX(),t->_max.GetY(),t->_min.GetZ());
		
		// Right Face
		glVertex3f(t->_max.GetX(),t->_min.GetY(),t->_min.GetZ());
		glVertex3f(t->_max.GetX(),t->_min.GetY(),t->_max.GetZ());
		glVertex3f(t->_max.GetX(),t->_max.GetY(),t->_max.GetZ());
		glVertex3f(t->_max.GetX(),t->_max.GetY(),t->_min.GetZ());
		
		// Front Face
		glVertex3f(t->_min.GetX(),t->_max.GetY(),t->_max.GetZ());
		glVertex3f(t->_min.GetX(),t->_min.GetY(),t->_max.GetZ());
		glVertex3f(t->_max.GetX(),t->_min.GetY(),t->_max.GetZ());
		glVertex3f(t->_max.GetX(),t->_max.GetY(),t->_max.GetZ());

		// Back Face
		glVertex3f(t->_min.GetX(),t->_max.GetY(),t->_min.GetZ());
		glVertex3f(t->_min.GetX(),t->_min.GetY(),t->_min.GetZ());
		glVertex3f(t->_max.GetX(),t->_min.GetY(),t->_min.GetZ());
		glVertex3f(t->_max.GetX(),t->_max.GetY(),t->_min.GetZ());

		// Right Face
		glVertex3f(t->_max.GetX(),t->_min.GetY(),t->_min.GetZ());
		glVertex3f(t->_max.GetX(),t->_min.GetY(),t->_max.GetZ());
		glVertex3f(t->_max.GetX(),t->_max.GetY(),t->_max.GetZ());
		glVertex3f(t->_max.GetX(),t->_max.GetY(),t->_min.GetZ());
		
		// Top Face
		glVertex3f(t->_min.GetX(),t->_max.GetY(),t->_min.GetZ());
		glVertex3f(t->_min.GetX(),t->_max.GetY(),t->_max.GetZ());
		glVertex3f(t->_max.GetX(),t->_max.GetY(),t->_max.GetZ());
		glVertex3f(t->_max.GetX(),t->_max.GetY(),t->_min.GetZ());

		// Bottom Face
		glVertex3f(t->_min.GetX(),t->_min.GetY(),t->_min.GetZ());
		glVertex3f(t->_min.GetX(),t->_min.GetY(),t->_max.GetZ());
		glVertex3f(t->_max.GetX(),t->_min.GetY(),t->_max.GetZ());
		glVertex3f(t->_max.GetX(),t->_min.GetY(),t->_min.GetZ());
		*/
	}

	if(t->m_left)DrawNode(t->m_left,geom);
	if(t->m_right)DrawNode(t->m_right,geom);
}

void TXGLKDTree::Draw()
{
	Application().LogMessage(L"TXGLKDTree Draw Called...");
	if(m_geom!=NULL)
	{
		DrawNode(m_geom->m_kdtree,m_geom);
	}
	/*
	if(ctr->points!=NULL)
	{
		glVertex3f(ctr->x0,ctr->y0,ctr->z0);
		 glVertex3f(ctr->x1,ctr->y0,ctr->z0);
		glVertex3f(ctr->x1,ctr->y0,ctr->z0);
		 glVertex3f(ctr->x1,ctr->y1,ctr->z0);
		glVertex3f(ctr->x1,ctr->y1,ctr->z0);
		 glVertex3f(ctr->x0,ctr->y1,ctr->z0);
		glVertex3f(ctr->x0,ctr->y1,ctr->z0);
		 glVertex3f(ctr->x0,ctr->y0,ctr->z0);
		
		glVertex3f(ctr->x0,ctr->y0,ctr->z1);
		 glVertex3f(ctr->x1,ctr->y0,ctr->z1);
		glVertex3f(ctr->x1,ctr->y0,ctr->z1);
		 glVertex3f(ctr->x1,ctr->y1,ctr->z1);
		glVertex3f(ctr->x1,ctr->y1,ctr->z1);
		 glVertex3f(ctr->x0,ctr->y1,ctr->z1);
		glVertex3f(ctr->x0,ctr->y1,ctr->z1);
		 glVertex3f(ctr->x0,ctr->y0,ctr->z1);
		 
		glVertex3f(ctr->x0,ctr->y0,ctr->z0);
		 glVertex3f(ctr->x0,ctr->y0,ctr->z1);
		glVertex3f(ctr->x1,ctr->y0,ctr->z0);
		 glVertex3f(ctr->x1,ctr->y0,ctr->z1);
		glVertex3f(ctr->x1,ctr->y1,ctr->z0);
		 glVertex3f(ctr->x1,ctr->y1,ctr->z1);
		glVertex3f(ctr->x0,ctr->y1,ctr->z0);
		 glVertex3f(ctr->x0,ctr->y1,ctr->z1);
	}
	if(ctr->spx==0)
		glColor3f(0.1,0,0.1);
	else if(ctr->spx==1)
		glColor3f(0,0.1,0.1);
	else
		glColor3f(0.1,0.1,0);
	*/
	//render left and right node
	/*
	if(_kdtree->_left)_kdtree->_left
	Draw(tree->_left);
	Draw(tree->_right);
	*/
	

}
