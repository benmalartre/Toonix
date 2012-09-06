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
  TXTriangle* GetTriangle(int i) const { return _triangles[i]; };

  // the edge number in the two triangles
  ULONG GetEdgeIDInTriangle(int i) const { return _eid[i]; };
  // end point positions in the dual space
  const CVector3f& GetDualPosition(int i) const { return _dp[i]; };
  // front facing or back facing
  bool IsFacing() const { return _facing; };

  // silhouette checked tag
  bool IsChecked() const { return _checked; };
  void Check()   { _checked = true; _checkededges.push_back(this); };
  void Uncheck() { _checked = false; };

  // touch a box
  bool Touch(const CVector3f& minp, const CVector3f& maxp) const;

  // project edges to dual space
  static void Project(TXOctree *dualmesh, TXEdge* edge);

  // clear silhouette checked tags
  static void ClearCheckTags() {
    int csz = _checkededges.size();
    for (int i = 0; i < csz; i++ )
      _checkededges[i]->Uncheck();
    _checkededges.clear();
  };

  TXEdge* GetEdge(){return _e;};

private:
	TXEdge* _e;
	TXTriangle *_triangles[2];
	ULONG _eid[2];
	CVector3f _dp[2];
	bool _facing;

	bool _checked;
	static std::vector<TXDualEdge*> _checkededges;
};

// TXOctree
class TXOctree 
{
public:
  TXOctree() : _depth(0), _min(-1, -1, -1), _max(1, 1, 1), _isLeaf(true)
    { for (int i=0; i<8; i++) _child[i] = 0; };
  TXOctree(const CVector3f& minp, const CVector3f& maxp, int depth = 0) :
    _depth(depth), _min(minp), _max(maxp), _isLeaf(true)
    { for (int i=0; i<8; i++) _child[i] = 0; };
  ~TXOctree();

  // depth in octree
  int GetDepth() const { return _depth; };

  // bounding box
  const CVector3f& GetMin() const { return _min; };
  const CVector3f& GetMax() const { return _max; };

  // leaf
  bool IsLeaf() const { return _isLeaf; };

  // edge size
  int GetSize() const { return _edges.size(); };
  std::vector<TXDualEdge*>& GetEdges() { return _edges; };

  // insert an edge
  void Insert(TXDualEdge *e) { _edges.push_back(e); };

  // split into 8
  void Split();

  // find silhouettes
  void FindSilhouettes(const CVector3f& n, float d, std::vector<TXEdge*>& silhouettes, float bias);

  void ClearSilhouettes(){
  std::vector<TXDualEdge*>::iterator it;
  for(it=_edges.begin();it<_edges.end();it++)
	  (*it)->ClearCheckTags();};

  // build the tree
  static TXOctree* BuildTree(TXGeometry* geom);

private:
  // depth in octree
  int _depth;

  // bounding box
  CVector3f _min, _max;

  // leaf ?
  bool _isLeaf;

  // children
  TXOctree *_child[8];

  // edges
  std::vector<TXDualEdge*> _edges;

  // the camera plane
  CVector3f _normal;
  double _d;

  // touch the camera plane
  bool TouchPlane(const CVector3f& n, float d, float bias);

  // static members
  static const int MAX_EDGE_NUMBER;
};

#endif /* _TOONIX_DUALMESH_H_ */
