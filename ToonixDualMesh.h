// ToonixDualMesh.h
//-------------------------------------------------
#ifndef _TOONIX_DUALMESH_H_
#define _TOONIX_DUALMESH_H_

#include "ToonixRegister.h"
#include "ToonixGeometry.h"
#include "ToonixGrid3d.h"

class TXOctree;

// projection of mesh's internal edges in the dual space
class TXDualEdge
{
public:
  // pos1 and pos2 should be projected to the same surface of the 4D cube
  TXDualEdge(TXEdge* e, bool fac, int tp, const CVector4f& pos1, const CVector4f& pos2);
  TXDualEdge();
  ~TXDualEdge() {};

  // the two triangles that share the edge
  TXTriangle* GetTriangle(int i) const { return m_triangles[i]; };

  // the edge number in the two triangles
  ULONG GetEdgeIDInTriangle(int i) const { return m_eid[i]; };
  // end point positions in the dual space
  const CVector3f& GetDualPosition(int i) const { return m_dp[i]; };
  // front facing or back facing
  bool IsFacing() const { return m_facing; };

  // silhouette checked tag
  bool IsChecked() const { return m_checked; };
  void Check()   { m_checked = true; m_checkededges.push_back(this); };
  void Uncheck() { m_checked = false; };

  // touch a box
  bool Touch(const CVector3f& minp, const CVector3f& maxp) const;

  // project edges to dual space
  static void Project(TXOctree *dualmesh, TXEdge* edge);

  // clear silhouette checked tags
  static void ClearCheckTags() {
    int csz = (int)m_checkededges.size();
    for (int i = 0; i < csz; i++ )
      m_checkededges[i]->Uncheck();
    m_checkededges.clear();
  };

  TXEdge* GetEdge(){return m_e;};

private:
	TXEdge* m_e;
	TXTriangle *m_triangles[2];
	ULONG m_eid[2];
	CVector3f m_dp[2];
	bool m_facing;

	bool m_checked;
	static std::vector<TXDualEdge*> m_checkededges;
};

// TXOctree
class TXOctree 
{
public:
  TXOctree() : m_depth(0), m_min(-1, -1, -1), m_max(1, 1, 1), m_isLeaf(true)
    { for (int i=0; i<8; i++) m_child[i] = 0; };
  TXOctree(const CVector3f& minp, const CVector3f& maxp, int depth = 0) :
    m_depth(depth), m_min(minp), m_max(maxp), m_isLeaf(true)
    { for (int i=0; i<8; i++) m_child[i] = 0; };
  ~TXOctree();

  // depth in octree
  int GetDepth() const { return m_depth; };

  // bounding box
  const CVector3f& GetMin() const { return m_min; };
  const CVector3f& GetMax() const { return m_max; };

  // leaf
  bool IsLeaf() const { return m_isLeaf; };

  // edge size
  size_t GetSize() const { return m_edges.size(); };
  std::vector<TXDualEdge*>& GetEdges() { return m_edges; };

  // insert an edge
  void Insert(TXDualEdge *e) { m_edges.push_back(e); };

  // split into 8
  void Split();

  // find silhouettes
  void FindSilhouettes(const CVector3f& n, float d, std::vector<TXEdge*>& silhouettes, float bias);

  void ClearSilhouettes(){
  std::vector<TXDualEdge*>::iterator it;
  for(it=m_edges.begin();it<m_edges.end();it++)
	  (*it)->ClearCheckTags();};

  // build the tree
  static TXOctree* BuildTree(TXGeometry* geom);

private:
  // depth in octree
  int m_depth;

  // bounding box
  CVector3f m_min, m_max;

  // leaf ?
  bool m_isLeaf;

  // children
  TXOctree *m_child[8];

  // edges
  std::vector<TXDualEdge*> m_edges;

  // the camera plane
  CVector3f m_normal;
  double m_d;

  // touch the camera plane
  bool TouchPlane(const CVector3f& n, float d, float bias);

  // static members
  static const int MAX_EDGE_NUMBER;
};

#endif /* _TOONIX_DUALMESH_H_ */
