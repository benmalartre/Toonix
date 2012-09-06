// ToonixKDtree.cpp
//-------------------------------------------------
#include "ToonixKDTree.h"

KDTreeNode::KDTreeNode(const CVector3f& min, const CVector3f& max)
{
	_min = min;
	_max = max;
	_left = NULL;
	_right = NULL;
	_points = NULL;
}

KDTreeNode::~KDTreeNode()
{
	if(_left!=NULL)delete _left;
	if(_right!=NULL)delete _right;
}

int Partition(float3* const arr, const int& left, const int & right, const int & x, const int & pivot)
{
	float pval = x==0 ? arr[pivot].x : x==1 ?arr[pivot].y : arr[pivot].z;
	float3 tmp = arr[right];
	arr[right] = arr[pivot];
	arr[pivot] = tmp;

	int sindex = left;
	for(int i= left;i<right;i++)
	{
		if((x==0 ? arr[i].x : x==1 ? arr[i].y : arr[i].z)<pval)
		{
			tmp = arr[i];
			arr[i] = arr[sindex];
			arr[sindex] = tmp;
			sindex++;
		}
	}

	tmp = arr[sindex];
	arr[sindex] = arr[right];
	arr[right] = tmp;
	return sindex;
}

void QuickSort(float3* const arr, const int& left, const int& right, const int& x)
{
	if(right>left)
	{
		int pivot = rand_int(left,right);
		int npivot = Partition(arr,left,right,x,pivot);
		QuickSort(arr,left,npivot-1,x);
		QuickSort(arr,npivot+1,right,x);
	}
}

void Build(float3* const points, const int& pnum, KDTreeNode* const ctr, const int& depth, const int& max, int cdepth = 0)
{
	if(cdepth==depth || pnum<=max)
	{
		ctr->_points = points;
		ctr->pnum = pnum;
		return;
	}

	//splits along the axis of who the tree's bound is largest
	int splitx = (ctr->_max.GetX()-ctr->_min.GetX() > ctr->_max.GetY()-ctr->_min.GetY())?0:(ctr->_max.GetY()-ctr->_min.GetY() > ctr->_max.GetZ()-ctr->_min.GetZ())?1:2;
	ctr->_axis = splitx;

	// sort points in order along the axis
	QuickSort(points,0,pnum-1,splitx);

	int numl = pnum>>1;
	int numr = pnum-numl;

	CVector3f vmin, vmax;

	//calculate bound for each split
	if(splitx==0)
	{
		float px;
		if(numl != numr)
			px= points[numl].x;
		else
			px = (points[numl-1].x+points[numl].x)*0.5f;

		vmin.Set(ctr->_min.GetX(),ctr->_min.GetY(),ctr->_min.GetZ());
		vmax.Set(px,ctr->_max.GetY(),ctr->_max.GetZ());
		ctr->_left = new KDTreeNode(vmin,vmax);

		vmin.Set(px,ctr->_min.GetY(),ctr->_min.GetZ());
		vmax.Set(ctr->_min.GetX(),ctr->_max.GetY(),ctr->_max.GetZ());
		ctr->_right = new KDTreeNode(vmin, vmax);
	}
	else if(splitx==1)
	{
		float py;
		if(numl != numr)
			py= points[numl].y;
		else
			py = (points[numl-1].y+points[numl].y)*0.5f;

		vmin.Set(ctr->_min.GetX(),ctr->_min.GetY(),ctr->_min.GetZ());
		vmax.Set(ctr->_max.GetX(),py,ctr->_max.GetZ());
		ctr->_left = new KDTreeNode(vmin, vmax);
		vmin.Set(ctr->_min.GetX(),py,ctr->_min.GetZ());
		vmax.Set(ctr->_min.GetX(),ctr->_max.GetY(),ctr->_max.GetZ());
		ctr->_right = new KDTreeNode(vmin, vmax);
	}
	else
	{
		float pz;
		if(numl != numr)
			pz= points[numl].z;
		else
			pz = (points[numl-1].z+points[numl].z)*0.5f;

		vmin.Set(ctr->_min.GetX(),ctr->_min.GetY(),ctr->_min.GetZ());
		vmax.Set(ctr->_max.GetX(),ctr->_max.GetY(),pz);
		ctr->_left = new KDTreeNode(vmin,vmax);
		vmin.Set(ctr->_min.GetX(),ctr->_min.GetY(),pz);
		vmax.Set(ctr->_min.GetX(),ctr->_max.GetY(),ctr->_max.GetZ());
		ctr->_right = new KDTreeNode(vmin,vmax);
	}
}