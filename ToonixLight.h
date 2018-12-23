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

	TXGeometry* m_geom;
	std::vector<CVector3f> m_vertices;
	CLongArray m_polygons;
	typedef std::map<int,int> VerticeMap;
	VerticeMap m_litvertices;

	void Build();

	ULONG m_nbv;							// Number Output Vertices
	ULONG m_nbp;							// Length Output Polygonal Description
	ULONG m_nbl;							// Number of lights
	std::vector<CVector3f> m_lights;		// Lights Position
	std::vector<float> m_bias;			// Lights Bias
	std::vector<float> m_distance;		// Lights Distance
	CVector3f m_view;					// Camera Position
	bool m_reverse;
	float m_push;

	void CutTriangle(TXGeometry* geom,ULONG a, ULONG b, ULONG c);
	void ClearDatas();
	bool SeeTriangle(TXTriangle* t, CVector3f& p, float& dist);

};

#endif