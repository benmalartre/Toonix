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

	std::vector<TXPoint*>* _points;
	TXLine* _line;

	std::vector<CVector3f> _vertices;
	CLongArray _polygons;

	CVector3Array _section;
	ULONG _subdiv;
	float _factor;

	CTransformation _xf;
	CVector3f _a;
	CVector3 _p;
	CVector3f _offset, _noffset;
	CMatrix3 _matrix;

	CVector3f _view;

	void AddPoint(LONG id, bool firstcap, bool hasnext, bool closed, ULONG chainsize);
	void DirectionToRotation(CVector3f& in_dir, CVector3f& in_up);

	void Build();

	ULONG _nbv;
	ULONG _nbp;
	bool _dirty;
	bool _valid;
};

#endif