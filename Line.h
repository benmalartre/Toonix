//Toonix Line
//-------------------------------------------

#ifndef TOONIX_LINE_H
#define TOONIX_LINE_H

#include "Register.h"
#include "Common.h"
#include "Geometry.h"
#include "Data.h"
//#include "DualMesh.h"

// Data structure holding data for each line particle
//------------------------------------
class TXPoint
{
public:
	TXPoint(){};
	TXPoint(TXPoint* p);
	~TXPoint(){};
	CVector3f m_pos;
	CVector3f m_dir;
	CVector3f m_norm;
	float m_length;
	float m_radius;
	LONG m_id;
	LONG m_lineid;			// boundary = -2, crease = -1, viewpoint = 0,1,...,n
	LONG m_sublineid;
	CColor4f m_color;
	
	void Merge(TXPoint*& in_other);
};

// TXChain Class
//------------------------------------
class TXChain
{  
public:
	TXChain(){};
	~TXChain(){};

	std::vector<TXPoint*> m_points;
	void AddPoint(TXPoint*& point);
	void Revert();
	void AddChain(TXChain*& chain);
	void Clear();
	void FilterPoints(float filter);

	LONG m_firstid;
	LONG m_lastid;
	LONG m_lineid;				// boundary = -2, crease = -1, viewpoint = 0,1,...,n
	LONG m_sublineid;			// id of the TXChain inside the TXLine
	bool m_closed;
};
// TXLine Class
//------------------------------------
class TXLine
{  
public:
	TXLine();
	~TXLine();

	void ClearDatas();
	void ClearChains();
	void EmptyData();
	void GetPoint(LONG edgeid, LONG nextedgeid,ULONG lastid, TXChain* chain);
	ULONG GetNbPoints();
	LONG GetNextID(LONG edgeid, LONG lastid, LONG lineid);
	bool BreakLine(LONG edgeid, LONG nextid);
	bool IsSilhouetteEdge(LONG edgeid,LONG lineid);
	void UncheckEdges(std::vector<TXEdge*>& inedges);
	void Build(LONG lineid);
	void Build(std::vector<TXEdge*>& inedges, LONG lineid);
	void Append(TXLine* line);
	void GetSilhouettes(ULONG v);								// get all silhouette edges for _eye[v]
	void ClearSilhouettes();									// clear silhouette datas
	void GetBoundaries();										// get all boundaries edges
	void GetCreases(){m_geom->GetCreaseEdges(m_crease);};			// get all crease edges
	void GetIntersections();									// get all intersection edges
	bool Break();

	TXGeometry* m_geom;
	std::vector<TXChain*> m_chains;

	float m_width;			// width
	float m_break;			// break line angle
	float m_filterpoints;	// distance for filtering line points
	float m_crease;			// crease angle
	float m_extend;			// extend chain start and end
	ULONG m_nbv;				// nb view point for silhouette detection
	ULONG m_nbp;				// total output TXPoints
	bool m_smoothsilhouette;	// smooth silhouette detection
	bool m_flip;
	ULONG  m_lastvertexid;
	bool m_recalculate;		// recalculate this line(used for caching crease info)
	bool m_breakline;

	std::vector<CVector3f> m_eye;	// points used for silhouette detection
	CFloatArray m_bias;				// for each point of view a bias value

	CVector3f m_view;

};
#endif