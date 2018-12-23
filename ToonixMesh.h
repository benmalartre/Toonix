// Toonix Mesh
//-------------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixGeometry.h"
#include "ToonixLine.h"

#ifndef _TOONIX_MESH_H
#define _TOONIX_MESH_H_

// TXMesh Class
//------------------------------------
class TXMesh
{  
public:
	TXMesh(){};
	TXMesh(TXLine* line);
	~TXMesh(){};

	std::vector<TXPoint*>* m_points;
	TXLine* m_line;

	std::vector<CVector3f> m_vertices;
	CLongArray m_polygons;

	CVector3Array m_section;
	ULONG m_subdiv;
	float m_factor;

	CTransformation m_xf;
	CVector3f m_a;
	CVector3 m_p;
	CVector3f m_offset, m_noffset;
	CMatrix3 m_matrix;

	CVector3f m_view;

	void AddPoint(LONG id, bool firstcap, bool hasnext, bool closed, ULONG chainsize);
	void DirectionToRotation(CVector3f& in_dir, CVector3f& in_up);

	void Build();

	ULONG m_nbv;
	ULONG m_nbp;
	bool m_dirty;
	bool m_valid;
};

#endif