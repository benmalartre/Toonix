
#ifndef TOONIX_GL_MANAGER_H
#define TOONIX_GL_MANAGER_H

#include "ToonixGLShape.h"
#include "ToonixGLLine.h"
#include "ToonixGLKDTree.h"

struct TXGLShapeMap
{
	TXGLShape* m_shape;
	TXGLShapeMap* m_next;
	long m_id;
};

class TXGLShapeManager
{
public:
	TXGLShapeManager(){};
	~TXGLShapeManager(){};

	ULONG GetUniqueID(){return m_cnt;};
	void AddShape(TXGLShape*);
	void RemoveShape(long id);

	void DrawAll();
	void DrawShape(long id);
	void FlushAll();

	TXGLShapeMap* m_map;
	ULONG m_nbshapes;
	ULONG m_cnt;
};

extern TXGLShapeManager G_TXGLShapeManager;

#endif