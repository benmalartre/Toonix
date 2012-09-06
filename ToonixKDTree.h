 // ToonixKDTree.h
//-------------------------------------------------
#ifndef _TOONIX_KDTREE_H_
#define _TOONIX_KDTREE_H_

#include "ToonixRegister.h"
#include "ToonixGeometry.h"
#include "ToonixCamera.h"

int MAX_RECURSION_DEPTH = 24;		//max depth recursion in kd-tree
int MAX_NUMBER_POINTS = 200;		//max number of points per cell in kd-tree

struct float3
{
	float x;
	float y;
	float z;
};

int rand_int(const int &l, const int& r)
{
	return rand()%(r-l)+l;
}

float rand_float()
{
	return float(rand()%100+(rand()%100)*100)*0.0001f;
}

class KDTreeNode
{
public:
	CVector3f _min, _max;				// bounding box
	float3 * _points;					// these only used if the node is on the end of a branch
	int pnum;							// pointer to the set of 'pnum' points contained in the leaf
	KDTreeNode* _left;					// pointer to the left child
	KDTreeNode* _right;					// pointer to the right child
	int _axis;							// orientation of the splitting plane

	KDTreeNode(const CVector3f& min, const CVector3f& max);
	~KDTreeNode();

};

#endif