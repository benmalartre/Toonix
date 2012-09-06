 // ToonixOctree.h
//-------------------------------------------------
#ifndef _TOONIX_OCTREE_H_
#define _TOONIX_OCTREE_H_

#include "ToonixRegister.h"
#include "ToonixGeometry.h"
#include "ToonixCamera.h"

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
	std::vector<TXEdge*>& GetEdges() { return _edges; };

	// insert an edge
	void Insert(TXEdge *e) { _edges.push_back(e); };

	// split into 8
	void Split();

	void SetMin(CVector3f min){_min=min;};
	void SetMax(CVector3f max){_max=max;};

	// visibility
	int IsVisible(TXCamera* camera);
	void GetVisibleEdges(TXCamera* camera, std::vector<TXEdge*>* visibleedges);
	void PushVisibleEdges(std::vector<TXEdge*>* visibleedges);
	void GetVisibleEdges(TXCamera* camera);
	void PushVisibleEdges();

	// build the tree

	void BuildTree(TXGeometry* geom);
	void ClearTree();

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
	std::vector<TXEdge*> _edges;

	// the camera
	TXCamera* _camera;

	// static members
	static const int MAX_EDGE_NUMBER;
};

#endif /* _TOONIX_OCTREE_H_ */
