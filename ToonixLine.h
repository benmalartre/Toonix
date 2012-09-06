//Toonix Line
//-------------------------------------------

#ifndef _TOONIX_LINE_H_
#define _TOONIX_LINE_H_

#include "ToonixGeometry.h"

// Data structure holding data for each line particle
//------------------------------------
class TXPoint
{
public:
	TXPoint(){};
	TXPoint(TXPoint* p);
	~TXPoint(){};
	CVector3f _pos;
	CVector3f _dir;
	CVector3f _norm;
	float _length;
	float _radius;
	LONG _id;
	LONG _lineid;			// boundary = -2, crease = -1, viewpoint = 0,1,...,n
	LONG _sublineid;
	
	void Merge(TXPoint*& in_other);
};

// TXChain Class
//------------------------------------
class TXChain
{  
public:
	TXChain(){};
	~TXChain(){};

	std::vector<TXPoint*> _points;
	void AddPoint(TXPoint*& point);
	void Revert();
	void AddChain(TXChain*& chain);
	void Clear();
	void FilterPoints(float filter);

	LONG _firstid;
	LONG _lastid;
	LONG _lineid;				// boundary = -2, crease = -1, viewpoint = 0,1,...,n
	LONG _sublineid;			// id of the TXChain inside the TXLine
	bool _closed;
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
	void GetCreases(){_geom->GetCreaseEdges(_crease);};			// get all crease edges
	bool Break();

	TXGeometry* _geom;
	std::vector<TXChain*> _chains;

	float _width;			// width
	float _break;			// break line angle
	float _filterpoints;	// distance for filtering line points
	float _crease;			// crease angle
	float _extend;			// extend chain start and end
	ULONG _nbv;				// nb view point for silhouette detection
	ULONG _nbp;				// total output TXPoints
	bool _smoothsilhouette;	// smooth silhouette detection
	bool _flip;
	ULONG  _lastvertexid;
	bool _recalculate;		// recalculate this line(used for caching crease info)
	bool _breakline;

	std::vector<CVector3f> _eye;	// points used for silhouette detection
	CFloatArray _bias;				// for each point of view a bias value

	CVector3f _view;

};
#endif