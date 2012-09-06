// ToonixOctree.cpp
//-------------------------------------------------
#include "ToonixOctree.h"

const int TXOctree::MAX_EDGE_NUMBER = 1000;

TXOctree::~TXOctree() 
{
	for (int i=0; i<8; i++) 
	{
		if (_child[i])delete _child[i];
		_child[i] = 0;
	}
	_edges.clear();
}

void TXOctree::ClearTree() 
{
	for (int i=0; i<8; i++) 
	{
		if (_child[i])delete _child[i];
		_child[i] = 0;
	}
	_edges.clear();
}

void TXOctree::BuildTree(TXGeometry* geo)
{
	ClearTree();
	// loop over all edges, insert all leaves to the tree
	std::vector<TXEdge*>::iterator it;
	for(it = geo->_edges.begin();it<geo->_edges.end();it++)
	{
		Insert(*it);
	}
	SetMin(geo->_min);
	SetMax(geo->_max);

	Split();

}

void TXOctree::Split() 
{
	int esz = _edges.size();
	//Application().LogMessage(L"Split OCtree called...Edge Number : "+(CString)esz);
	if (esz <= MAX_EDGE_NUMBER || (esz <= 2*MAX_EDGE_NUMBER && _depth > 3) ||(esz <= 3*MAX_EDGE_NUMBER && _depth > 4) ||_depth > 6 ) 
	{
		_isLeaf = true;
		return;
	}

	_isLeaf = false;

	double xx[] = {_min.GetX(), 0.5*(_min.GetX()+_max.GetX()), _max.GetX()};
	double yy[] = {_min.GetY(), 0.5*(_min.GetY()+_max.GetY()), _max.GetY()};
	double zz[] = {_min.GetZ(), 0.5*(_min.GetZ()+_max.GetZ()), _max.GetZ()};

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++) 
	{
		int m = 4*i + 2*j + k;
		_child[m] = new TXOctree( CVector3f(xx[i], yy[j], zz[k]),CVector3f(xx[i+1], yy[j+1], zz[k+1]),_depth+1 );

		int esz = _edges.size();

		for (int t = 0; t < esz; t++)
			if (_edges[t]->TouchCell(_child[m]->GetMin(), _child[m]->GetMax()))
			{
				_child[m]->Insert(_edges[t]);
			}

		if (_child[m]->GetSize() == 0) 
		{
		  delete _child[m];
		  _child[m] = 0;
		} 
		else _child[m]->Split();
	}

	//if (_depth > 0)_edges.clear();
}

int TXOctree::IsVisible(TXCamera* camera)
{
	double xx[] = {_min.GetX(), _max.GetX()};
	double yy[] = {_min.GetY(), _max.GetY()};
	double zz[] = {_min.GetZ(), _max.GetZ()};

	CVector3f p;
	ULONG out=0;

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++) 
	{
		p.Set(xx[i], yy[j], zz[k]);
		if(camera->See(p))out++;
	}

	return out;
}

void TXOctree::GetVisibleEdges(TXCamera* camera, std::vector<TXEdge*>* visibleedges)
{
	//Application().LogMessage(L"This Cell NB Edges : "+(CString)GetSize());
	int v = IsVisible(camera);
	//Application().LogMessage(L"Nb Visible COrners : "+(CString)v);
	if(v==0)
	{
		//Application().LogMessage(L"Cell is fully OUTSIDE camera frustrum");
		return;
	}
	if(v==8)
	{
		//Application().LogMessage(L"Cell is fully INSIDE camera frustrum  :: Push Visible Edges Data, for all of its children");
		PushVisibleEdges(visibleedges);
		return;
	}

	else
	{
		//Application().LogMessage(L"Leave Cell have at least ONE point inside camera frustrum  :: Push Visible Edges Data...");
		if(_isLeaf)
		{
			PushVisibleEdges(visibleedges);
		}

		else 
		{
			int cnt = 0;
			for (int i = 0; i < 8; i++)
			{
				if (_child[i])
				{
					v = _child[i]->IsVisible(camera);
					//cell is outside of camera frustrum
					if(v==0)
						continue;
					
					//cell is fully inside camera frustrum
					//we push visible vertices for all of it's children
					if(v==8)
					{
						//Application().LogMessage(L"SubCell is fully INSIDE camera frustrum  :: Push Visible Edges Data, for all of its children");
						PushVisibleEdges(visibleedges);
						continue;
					}

					
					else
					{
						_child[i]->GetVisibleEdges(camera,visibleedges);
					}
				}
			}
		}	
	}
}

void TXOctree::PushVisibleEdges(std::vector<TXEdge*>* visibleedges)
{
	std::vector<TXEdge*>::iterator it;
	for(it=_edges.begin();it<_edges.end();it++)
	{
		if(!(*it)->_isvisible)
			visibleedges->push_back(*it);
		(*it)->_isvisible = true;
	}
}
