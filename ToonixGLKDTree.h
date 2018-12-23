//-------------------------------------------------
// ToonixGLKDTree.h
//-------------------------------------------------
#ifndef TOONIX_GLKDTREE_H
#define TOONIX_GLKDTREE_H

#include "ToonixGLShape.h"
#include "ToonixKDTree.h"

class TXGeometry;

class TXGLKDTree : public TXGLShape
{
public:
	TXGLKDTree(){m_geom=NULL;};
	TXGLKDTree(TXKDTree* tree){m_tree = tree;};
	~TXGLKDTree(){Delete();};

	void Draw();
	void DrawNode(const TXKDTree* t,const TXGeometry* geom);

	void Set(TXGeometry* geom){m_geom=geom;};

private:
	TXGeometry* m_geom;
	TXKDTree* m_tree;

};

#endif