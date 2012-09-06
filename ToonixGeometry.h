// ToonixGeometry.h
//-------------------------------------------------
#ifndef _TOONIX_GEOMETRY_H_
#define _TOONIX_GEOMETRY_H_

#include <vector>
#include "ToonixCamera.h"

// forward declaration
class TXVertex;
class TXEdge;
class TXTriangle;
class TXPolygon;
class TXCamera;

static LONG CLUSTER = -3;
static LONG BOUNDARY = -2;
static LONG CREASE = -1;
static LONG SILHOUETTE = 0;

static unsigned char ToonixDataR = 222;
static unsigned char ToonixDataG = 222;
static unsigned char ToonixDataB = 222;

static unsigned char ToonixLineR = 200;
static unsigned char ToonixLineG = 200;
static unsigned char ToonixLineB = 200;

static unsigned char ToonixNodeR = 188;
static unsigned char ToonixNodeG = 188;
static unsigned char ToonixNodeB = 188;

// TXVertex Class
//------------------------------------
class TXVertex
{	
public:	
	TXVertex(LONG id,float px, float py, float pz);
	~TXVertex();

	void SetVisible(bool visible){_visible = visible;};
	bool IsVisible(){return _visible;};
	bool IsInCell(const CVector3f& min, const CVector3f& max);

	LONG _id;
	std::vector<TXEdge*> _edges;
	std::vector<TXTriangle*> _triangles;
	bool _isboundary;

	float _dot;
	LONG _sign;

	CVector3f _pos;
	CVector3f _norm;

	//void IsVisible(TXCamera* cam);
	bool _visible;

	bool _lit;
};

// TXEdge Class
//------------------------------------
class TXEdge
{  
public:
	TXEdge();
	TXEdge(LONG id,LONG p1, LONG p2);
	~TXEdge();
	LONG _id;
	CLongArray _pid;
	bool _ischecked;
	float _weight;

	bool _isclusteredge;		// is this edge a cluster edge
	bool _isboundary;			// is this edge a boundary edge
	bool _iscrease;				// is this edge a crease edge
	bool _issilhouette;			// is this edge a silhouette edge
	bool _isvisible;			// is this edge in camera culling

	CFloatArray _angles;		// angles with adjacent edges

	TXVertex* _start;
	TXVertex* _end;

	std::vector<TXTriangle*> _triangles;
	std::vector<TXEdge*> _adjacents;		//adjacent edges
	std::vector<TXEdge*> _neighbors;		//neighboring edges
	std::vector<TXEdge*> _next;
	std::vector<TXEdge*> _previous;

	std::vector<TXPolygon*> _polygons;

	bool ShareTriangle(TXEdge*& other);
	bool IsInTriangle(TXTriangle*& t);
	LONG SharePoint(TXEdge*& other);
	CLongArray WhoseNext();
	ULONG GetIDInTriangle(TXTriangle*);
	
	void GetNeighborEdges();
	void GetAdjacentEdges();
	void GetNextEdges();

	float GetAngle(TXEdge* other);
	bool Check(LONG lineid);

	bool TouchCell(const CVector3f& minp, const CVector3f& maxp);

	bool _lit;
};

// TXTriangle Class
//------------------------------------
class TXTriangle
{  
public:
	TXTriangle(LONG id,LONG p1, LONG p2, LONG p3);
	std::vector<TXVertex*> _v;
	~TXTriangle(){};
	LONG _id;
	CLongArray _pid;
	CVector3f _norm;
	bool _lit;
	bool _facing;
};

// TXPolygon Class
//------------------------------------
class TXPolygon
{  
public:
	TXPolygon(LONG id,CLongArray pid);
	~TXPolygon();
	LONG _id;
	CLongArray _pid;
	CVector3f _norm;

	std::vector<TXEdge*> _edges;
	void PushEdgeData(TXEdge*& edge);
};



// TXGeometry Class
//------------------------------------
class TXGeometry
{  
public:
	TXGeometry();
	~TXGeometry();

	std::vector<TXGeometry*> _subgeometries;
	bool _isleaf;

	std::vector<TXVertex*> _vertices;
	std::vector<TXEdge*> _edges;
	std::vector<TXVertex*> _visiblevertices;
	std::vector<TXEdge*> _visibleedges;
	std::vector<TXTriangle*> _triangles;
	std::vector<TXPolygon*> _polygons;

	std::vector<TXEdge*> _boundaries;
	std::vector<TXEdge*> _creases;
	std::vector<TXEdge*> _clusteredges;
	std::vector<TXEdge*> _silhouettes;

	CVector3f _spherecenter;
	float _sphereradius;

	CDoubleArray _points;

	void GetPointPosition(CICEGeometry& geom);

	void GetDotAndSign(CVector3f& view, float bias);
	float GetAngleBetweenTwoEdges(ULONG id1, ULONG id2);

	void GetCachedMeshData(CICEGeometry& geom);
	void GetRuntimeMeshData(CICEGeometry& geom);

	bool GetBBoxVisibility(TXCamera* camera);
	void GetVisible(TXCamera* camera);
	void GetCreaseEdges(float crease);
	void ClearVisible();
	void SetAllVisible();

	void ClearDatas();
	void ClearVisibilityDatas();

	void GetSubGeometries(CICEGeometry& geom);

	bool IsSilhouette(TXEdge* e,CVector3f view, bool smooth);

	void Unlit();

	bool _init;				// is cached mesh data initialized
	bool _creasecached;		// is crease data cached
	ULONG _nbe;				// actual nb edges on geometry
	ULONG _nbv;				// actual nb vertices
	ULONG _nbve;			// nb visible edges
	ULONG _nbvv;			// nb visible vertices
	bool _dirty;
	CVector3f _min, _max;	// bounding box infos
	CVector3f _view;		// camera position

	bool _culledges;		// boolean to cull or not edges before building lines
	int _infrustrum;

};


#endif