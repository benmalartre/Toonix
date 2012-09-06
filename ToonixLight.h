// Toonix Light
//-------------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixGeometry.h"
#include "ToonixLine.h"
#include <map>

#ifndef _TOONIX_MESH_H
#define _TOONIX_MESH_H_

// TXLight Class
//------------------------------------
class TXLight
{  
public:
	TXLight();
	~TXLight(){};

	TXGeometry* _geom;
	std::vector<CVector3f> _vertices;
	CLongArray _polygons;
	typedef std::map<int,int> VerticeMap;
	VerticeMap _litvertices;

	void Build();

	ULONG _nbv;							// Number Output Vertices
	ULONG _nbp;							// Length Output Polygonal Description
	ULONG _nbl;							// Number of lights
	std::vector<CVector3f> _lights;		// Lights Position
	std::vector<float> _bias;			// Lights Bias
	std::vector<float> _distance;		// Lights Distance
	CVector3f _view;					// Camera Position
	bool _reverse;
	float _push;

	void CutTriangle(TXGeometry* geom,ULONG a, ULONG b, ULONG c);
	void ClearDatas();
	bool SeeTriangle(TXTriangle* t, CVector3f& p, float& dist);

};

#endif