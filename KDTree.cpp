// ToonixKDtree.cpp
//-------------------------------------------------
#include "KDTree.h"

int rand_int(const int &l, const int& r)
{
	return rand()%(r-l)+l;
}

float rand_float()
{
	return float(rand()%100+(rand()%100)*100)*0.0001f;
}

TXKDTree::TXKDTree()
{
	m_left = NULL;
	m_right = NULL;
}

TXKDTree::TXKDTree(const CVector3f& min, const CVector3f& max)
{
	m_min = min;
	m_max = max;
	m_left = NULL;
	m_right = NULL;
}

TXKDTree::~TXKDTree()
{
	if(m_left!=NULL)delete m_left;
	if(m_right!=NULL)delete m_right;
}

int TXKDTree::Partition(float3* points, const int& left, const int & right, const int & x, const int & pivot)
{
	float pval = x==0 ? points[pivot].x : x==1 ?points[pivot].y : points[pivot].z;
	float3 tmp = points[right];
	points[right] = points[pivot];
	points[pivot] = tmp;

	int sindex = left;
	for(int i= left;i<right;i++)
	{
		if((x==0 ? points[i].x : x==1 ? points[i].y : points[i].z)<pval)
		{
			tmp = points[i];
			points[i] = points[sindex];
			points[sindex] = tmp;
			sindex++;
		}
	}

	tmp = points[sindex];
	points[sindex] = points[right];
	points[right] = tmp;
	return sindex;
}

void TXKDTree::QuickSort(float3* points, const int& left, const int& right, const int& x)
{
	if(right>left)
	{
		int pivot = rand_int(left,right);
		int npivot = Partition(points,left,right,x,pivot);
		QuickSort(points,left,npivot-1,x);
		QuickSort(points,npivot+1,right,x);
	}
}

void TXKDTree::Build(float3* points, const int& pnum, TXKDTree* const ctr, const int& depth, const int& max, int cdepth=0)
{
	Application().LogMessage(L" Build KD Tree Called with Depth : "+(CString)cdepth);

	if(cdepth==depth || pnum<=max)
	{
		CString accum = L"";
		Application().LogMessage(L"This is a leaf, with "+(CString)pnum+L" points...");
		ctr->m_points.clear();
		for(int i= 0;i<pnum;i++)
		{
			ctr->m_points.push_back(points[i].id);
			accum += (CString)points[i].id+L",";
		}
		Application().LogMessage(L"IDs : "+accum);
		ctr->m_numPoints = pnum;
		ctr->m_leaf = true;
		return;
	}
	else
		ctr->m_leaf = false;

	//splits along the axis of who the tree's bound is largest
	int splitx = (ctr->m_max.GetX()-ctr->m_min.GetX() > ctr->m_max.GetY()-ctr->m_min.GetY()) ? 0:(ctr->m_max.GetY()-ctr->m_min.GetY() > ctr->m_max.GetZ()-ctr->m_min.GetZ())?1:2;
	ctr->m_axis = splitx;

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

		vmin.Set(ctr->m_min.GetX(),ctr->m_min.GetY(),ctr->m_min.GetZ());
		vmax.Set(px,ctr->m_max.GetY(),ctr->m_max.GetZ());
		ctr->m_left = new TXKDTree(vmin,vmax);

		vmin.Set(px,ctr->m_min.GetY(),ctr->m_min.GetZ());
		vmax.Set(ctr->m_min.GetX(),ctr->m_max.GetY(),ctr->m_max.GetZ());
		ctr->m_right = new TXKDTree(vmin, vmax);
	}
	else if(splitx==1)
	{
		float py;
		if(numl != numr)
			py= points[numl].y;
		else
			py = (points[numl-1].y+points[numl].y)*0.5f;

		vmin.Set(ctr->m_min.GetX(),ctr->m_min.GetY(),ctr->m_min.GetZ());
		vmax.Set(ctr->m_max.GetX(),py,ctr->m_max.GetZ());
		ctr->m_left = new TXKDTree(vmin, vmax);
		vmin.Set(ctr->m_min.GetX(),py,ctr->m_min.GetZ());
		vmax.Set(ctr->m_min.GetX(),ctr->m_max.GetY(),ctr->m_max.GetZ());
		ctr->m_right = new TXKDTree(vmin, vmax);
	}
	else
	{
		float pz;
		if(numl != numr)
			pz= points[numl].z;
		else
			pz = (points[numl-1].z+points[numl].z)*0.5f;

		vmin.Set(ctr->m_min.GetX(),ctr->m_min.GetY(),ctr->m_min.GetZ());
		vmax.Set(ctr->m_max.GetX(),ctr->m_max.GetY(),pz);
		ctr->m_left = new TXKDTree(vmin,vmax);
		vmin.Set(ctr->m_min.GetX(),ctr->m_min.GetY(),pz);
		vmax.Set(ctr->m_min.GetX(),ctr->m_max.GetY(),ctr->m_max.GetZ());
		ctr->m_right = new TXKDTree(vmin,vmax);
	}

	//build sub-trees
	Build(points,numl,ctr->m_left,depth,max,cdepth+1);
	Build(points+numl,numr,ctr->m_right,depth,max,cdepth+1);
}