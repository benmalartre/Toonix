// ToonixGeometry.h
//-------------------------------------------------
#ifndef _TOONIX_GEOMETRY_H_
#define _TOONIX_GEOMETRY_H_

#include <vector>
#include "ToonixCamera.h"
#include "ToonixBBox.h"
#include "ToonixKDTree.h"

// forward declaration
class TXVertex;
class TXEdge;
class TXTriangle;
class TXPolygon;
class TXCamera;

static LONG INTERSECTION = -4;
static LONG CLUSTER = -3;
static LONG BOUNDARY = -2;
static LONG CREASE = -1;
static LONG SILHOUETTE = 0;

// TXVertex Class
//------------------------------------
class TXVertex
{	
public:	
	TXVertex(LONG id,float px, float py, float pz);
	~TXVertex();

	void SetVisible(bool visible){m_visible = visible;};
	bool IsVisible(){return m_visible;};
	bool IsInCell(const CVector3f& min, const CVector3f& max);
	void IsCrease();

	LONG m_id;
	std::vector<TXEdge*> m_edges;
	std::vector<TXTriangle*> m_triangles;
	bool m_isboundary;
	bool m_iscrease;

	float m_dot;
	LONG m_sign;

	CVector3f m_pos;
	CVector3f m_norm;

	//void IsVisible(TXCamera* cam);
	bool m_visible;

	bool m_lit;
};

// TXEdge Class
//------------------------------------
class TXEdge
{  
public:
	TXEdge();
	TXEdge(LONG id,LONG p1, LONG p2);
	~TXEdge();
	LONG m_id;
	CLongArray m_pid;
	bool m_ischecked;
	float m_weight;

	bool m_isintersection;		// is this edge a intersection edge
	bool m_isclusteredge;		// is this edge a cluster edge
	bool m_isboundary;			// is this edge a boundary edge
	bool m_iscrease;				// is this edge a crease edge
	bool m_issilhouette;			// is this edge a silhouette edge
	bool m_isvisible;			// is this edge in camera culling

	CFloatArray m_angles;		// angles with adjacent edges

	TXVertex* m_start;
	TXVertex* m_end;

	std::vector<TXTriangle*> m_triangles;
	std::vector<TXEdge*> m_adjacents;		//adjacent edges
	std::vector<TXEdge*> m_neighbors;		//neighboring edges
	std::vector<TXEdge*> m_next;
	std::vector<TXEdge*> m_previous;

	std::vector<TXPolygon*> m_polygons;

	bool ShareTriangle(TXEdge* other);
	bool IsInTriangle(TXTriangle* t);
	bool IsConnectedToTriangle(TXTriangle* t);
	LONG SharePoint(TXEdge*& other);
	CLongArray WhoseNext();
	ULONG GetIDInTriangle(TXTriangle*);
	
	void GetNeighborEdges();
	void GetAdjacentEdges();
	void GetNextEdges();

	float GetAngle(TXEdge* other);
	float GetAngle(TXVertex* v);
	float GetDot(TXVertex* v);
	bool Check(LONG lineid);

	bool TouchCell(const CVector3f& minp, const CVector3f& maxp);
	bool IntersectWithBBox(TXBBox* in_bbox);
	bool IntersectWithTriangle(TXTriangle* in_triangle);

	bool _lit;
};

// TXTriangle Class
//------------------------------------
class TXTriangle
{  
public:
	TXTriangle(LONG id,LONG p1, LONG p2, LONG p3);
	std::vector<TXVertex*> m_v;
	~TXTriangle(){};
	LONG m_id;
	CLongArray m_pid;
	CVector3f m_norm;
	bool m_lit;
	bool m_facing;
	bool IsVisible();

	/// A struct representing a 2-dimensional uv coordinate
	struct uvCoord
	{
		float m_u;
		float m_v;

		uvCoord(float in_u = 0,float in_v = 0)
		{
			m_u = in_u;
			m_v = in_v;
		}

		bool IsOnTriangle()
		{
			return (m_u >= 0.0f) && (m_v >= 0.0f) && (m_u + m_v <= 1.0f);
		}
		bool IsInsideTriangle()
		{
			return (m_u > 0.0f) && (m_v > 0.0f) && (m_u + m_v < 1.0f);
		}
	};

	virtual uvCoord GetUVFromPoint(const CVector3f & in_pos);
};

// TXPolygon Class
//------------------------------------
class TXPolygon
{  
public:
	TXPolygon(LONG id,CLongArray pid);
	~TXPolygon();
	LONG m_id;
	CLongArray m_pid;
	CVector3f m_norm;

	std::vector<TXEdge*> m_edges;
	void PushEdgeData(TXEdge*& edge);
};


class TXKDTree;

// TXGeometry Class
//------------------------------------
class TXGeometry
{  
public:
	TXGeometry();
	~TXGeometry();

	TXKDTree* m_kdtree;
	std::vector<TXGeometry*> m_subgeometries;
	bool m_isleaf;
	bool m_isroot;
	ULONG m_id;
	ULONG m_basevertexID;
	ULONG m_basetriangleID;
	ULONG m_baseedgeID;

	std::vector<TXVertex*> m_vertices;
	std::vector<TXEdge*> m_edges;
	std::vector<TXVertex*> m_visiblevertices;
	std::vector<TXEdge*> m_visibleedges;
	std::vector<TXTriangle*> m_triangles;
	std::vector<TXPolygon*> m_polygons;

	std::vector<TXEdge*> m_boundaries;
	std::vector<TXEdge*> m_creases;
	std::vector<TXEdge*> m_clusteredges;
	std::vector<TXEdge*> m_silhouettes;
	std::vector<TXEdge*> m_intersections;

	CVector3f m_spherecenter;
	float m_sphereradius;

	CDoubleArray m_points;

	void GetPointPosition(CICEGeometry& geom);

	void GetDotAndSign(CVector3f& view, float bias);
	float GetAngleBetweenTwoEdges(ULONG id1, ULONG id2);

	void BuildKDTree();

	void GetCachedMeshData(CICEGeometry& geom);
	void GetRuntimeMeshData(CICEGeometry& geom);

	bool GetBBoxVisibility(TXCamera* camera);
	void GetVisible(TXCamera* camera);
	void GetCreaseEdges(float crease);
	void GetIntersectionEdges();
	void ClearVisible();
	void SetAllVisible();

	void ClearDatas();
	void ClearVisibilityDatas();

	void GetSubGeometries(CICEGeometry& geom);

	bool IsSilhouette(TXEdge* e,CVector3f view, bool smooth);
	bool GetDirtyState(CICEGeometry& geom);

	void Unlit();

	bool m_init;				// is cached mesh data initialized
	bool m_creasecached;		// is crease data cached
	ULONG m_nbv;				// actual nb vertices in geometry
	ULONG m_nbe;				// actual nb edges in geometry
	ULONG m_nbt;				// actual nb triangles in geometry

	ULONG m_nbve;			// nb visible edges
	ULONG m_nbvv;			// nb visible vertices

	ULONG m_dirty;			// is the geometry dirty
	
	CVector3f m_min, m_max;	// bounding box infos
	CVector3f m_view;		// camera position

	bool m_culledges;		// boolean to cull or not edges before building lines
	int m_infrustrum;

};


#endif