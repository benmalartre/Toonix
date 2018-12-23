//-------------------------------------------------
// ToonixKDTree.h
//-------------------------------------------------
#ifndef _TOONIX_KDTREE_H_
#define _TOONIX_KDTREE_H_

#include "ToonixRegister.h"
#include "ToonixGeometry.h"
#include "ToonixCamera.h"


/*
int MAX_RECURSION_DEPTH;		//max depth recursion in kd-tree
int MAX_NUMBER_POINTS;		//max number of points per cell in kd-tree
*/

struct float3{
	float x;
	float y;
	float z;
	ULONG id;
};

class TXKDTree
{
public:
	bool m_leaf;							// is this node a leaf node

	CVector3f m_min, m_max;				// bounding box
	std::vector<ULONG> m_points;		// these only used if the node is on the end of a branch
	ULONG m_numPoints;					// numPoints
	TXKDTree* m_left;					// pointer to the left child
	TXKDTree* m_right;					// pointer to the right child
	short m_axis;						// orientation of the splitting plane

	ULONG m_oglid;							// unique identifier id for ogl

	TXKDTree();
	TXKDTree(const CVector3f& min, const CVector3f& max);
	~TXKDTree();

	int Partition(float3* points, const int& left, const int & right, const int & x, const int & pivot);
	void QuickSort(float3* points, const int& left, const int& right, const int& x);
	void Build(float3* points, const int& pnum, TXKDTree* const ctr, const int& depth, const int& max, int cdepth);

};

#endif