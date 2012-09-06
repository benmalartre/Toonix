// Toonix Data
//--------------------------------------------
#include "ToonixRegister.h"
#include "ToonixGeometry.h"

TXGeometry::TXGeometry()
{
	_init = false;
	_nbv=0;
	_creasecached = false;
}

TXGeometry::~TXGeometry()
{
	ClearDatas();
}

void TXGeometry::GetSubGeometries(CICEGeometry& geom)
{
	std::vector<TXGeometry*>::iterator git=_subgeometries.begin();
	for(;git<_subgeometries.end();git++)
		delete (*git);

	ULONG nbsg = geom.GetSubGeometryCount();
	//Application().LogMessage(L"Nb Sub Geometries : "+(CString)nbsg);
	if(nbsg>0)
	{
		_subgeometries.resize(nbsg);
		_isleaf = false;
		for(ULONG i=0;i<nbsg;i++)
		{
			TXGeometry* sub = new TXGeometry();
			CICEGeometry subgeom = geom.GetSubGeometry(i);
			ULONG nbv = subgeom.GetPointPositionCount();
			sub->_isleaf = true;
			_subgeometries[i] = sub;
			_subgeometries[i]->_nbv = nbv;
		}
	}
	else 
	{
		_isleaf = true;
	}
}

void TXGeometry::Unlit()
{
	for(ULONG l=0;l<_vertices.size();l++)
	{
		_vertices[l]->_lit = false;	
	}

	for(ULONG v=0;v<_triangles.size();v++)
	{
		_triangles[v]->_lit = false;	
	}
}

void TXGeometry::ClearDatas()
{
	_boundaries.clear();
	_creases.clear();
	_creasecached = false;
	_clusteredges.clear();
	_silhouettes.clear();

	ClearVisible();

	std::vector<TXGeometry*>::iterator it = _subgeometries.begin();
	for(;it<_subgeometries.end();it++)
	{
		delete *it;
	}
	_subgeometries.clear();

	for(LONG a = 0;a<(LONG)_vertices.size();a++)delete _vertices[a];
	_vertices.clear();

	for(LONG a = 0;a<(LONG)_edges.size();a++)delete _edges[a];
	_edges.clear();

	for(LONG a = 0;a<(LONG)_triangles.size();a++)delete _triangles[a];
	_triangles.clear();

	for(LONG a = 0;a<(LONG)_polygons.size();a++)delete _polygons[a];
	_polygons.clear();
	_nbv = _nbe = 0;

}

void TXGeometry::GetCachedMeshData(CICEGeometry& geom)
{
	ClearDatas();
	GetSubGeometries(geom);

	GetPointPosition(geom);

	//loop over all edges
	_nbe = geom.GetSegmentCount();
	CLongArray sIndices;
	geom.GetSegmentIndices(sIndices);
	_edges.resize(_nbe);

	for (ULONG i=0; i<_nbe; i++)
	{
		_edges[i] = new TXEdge(i,sIndices[i*2],sIndices[i*2+1]);
		_vertices[sIndices[i*2]]->_edges.push_back(_edges[i]);
		_vertices[sIndices[i*2+1]]->_edges.push_back(_edges[i]);
		_edges[i]->_start = _vertices[sIndices[i*2]];
		_edges[i]->_end = _vertices[sIndices[i*2+1]];
	}

	// loop over triangles	*/
	LONG tCount = geom.GetTriangleCount();
	CLongArray tIndices;
	geom.GetTrianglePointIndices(tIndices);
	CVector3f a, b;
	CVector3f p;

	_triangles.resize(tCount);

	for (LONG i=0; i<tCount; i++)
	{
		_triangles[i] = new TXTriangle(i,tIndices[i*3],tIndices[i*3+1],tIndices[i*3+2]);

		// Get Normal
		a.Sub(_vertices[tIndices[i*3]]->_pos,_vertices[tIndices[i*3+1]]->_pos);
		b.Sub(_vertices[tIndices[i*3]]->_pos,_vertices[tIndices[i*3+2]]->_pos);
		_triangles[i]->_norm.Cross(a,b);
		_triangles[i]->_norm.NormalizeInPlace();

		for(LONG j=0;j<3;j++)
		{
			// find connected triangles
			_vertices[tIndices[i*3+j]]->_triangles.push_back(_triangles[i]);
			_triangles[i]->_v.push_back(_vertices[tIndices[i*3+j]]);
		}
	}

	// loop over polygons
	LONG pCount = geom.GetPolygonCount();
	
	CLongArray pSizes;
	CLongArray pIndices;
	geom.GetPolygonIndices( pSizes, pIndices );
	CLongArray pid;
	TXPolygon* polygon;
	std::vector<TXEdge*> pe;
	
	LONG nOffset = 0;
	for (LONG i=0; i<pCount; i++)
	{
		pid.Clear();
		for (LONG j=0; j<(long)pSizes[i]; j++, nOffset++)
		{
			pid.Add(pIndices[nOffset]);
		}
		polygon = new TXPolygon(i,pid);

		for (LONG j=0; j<pid.GetCount(); j++)
		{
			pe = _vertices[pid[j]]->_edges;
			for(LONG k=0;k<(LONG)pe.size();k++)
			{
				polygon->PushEdgeData(pe[k]);
			}
		}
		_polygons.push_back(polygon);
	}

	int found;
	//reloop over all edges getting adjacents + edge type
	//also get neighboring edges
	for (ULONG i=0; i<_nbe; i++)
	{
		found = 0;
		TXEdge* e = _edges[i];
		//loop all triangle connected to first point
		std::vector<TXTriangle*> et = _vertices[e->_pid[0]]->_triangles;
		for(LONG j= 0;j<(LONG)et.size();j++)
		{
			for(LONG k =0;k<3;k++)
			{
				if(et[j]->_pid[k] == e->_pid[1])
				{
					e->_triangles.push_back(et[j]);
					found++;
				}
			}
		}

		if(found<2)
		{
			e->_isboundary = true;
			_boundaries.push_back(e);
			_vertices[e->_pid[0]]->_isboundary = true;
			_vertices[e->_pid[1]]->_isboundary = true;
		}
		else 
		{
			e->_isboundary = false;
			_vertices[e->_pid[0]]->_isboundary = false;
			_vertices[e->_pid[1]]->_isboundary = false;
		}
		
		e->GetNeighborEdges();
		e->GetAdjacentEdges();
		e->GetNextEdges();

	}

	// init mesh data flag
	_init = true;
	_dirty = true;

	//Set visibility
	//SetAllVisible();
}

void TXGeometry::GetPointPosition(CICEGeometry& geom)
{
	CDoubleArray points;
    geom.GetPointPositions( points ) ;
	LONG pCount = geom.GetPointPositionCount();

	_min = CVector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	_max = CVector3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	CVector3f pv;
	bool clear = false;
	if(_nbv != pCount)clear = true;

	if(clear)
	{
		// Clean old data
		for(ULONG v=0;v<_vertices.size();v++)delete _vertices[v];

		_nbv = pCount;
		_vertices.resize(_nbv);
	}

	//only ONE geometrie
	if(_isleaf)
	{
		// loop over vertices
		for (ULONG i=0; i<_nbv; i++)
		{	
			if(clear)
				_vertices[i] = new TXVertex(i,(float)points[i*3],(float)points[i*3+1],(float)points[i*3+2]);
			else
				_vertices[i]->_pos.Set((float)points[i*3],(float)points[i*3+1],(float)points[i*3+2]);

			pv = _vertices[i]->_pos;
		
			_min.Set(MIN(_min.GetX(),pv.GetX()),MIN(_min.GetY(),pv.GetY()),MIN(_min.GetZ(),pv.GetZ()));
			_max.Set(MAX(_max.GetX(),pv.GetX()),MAX(_max.GetY(),pv.GetY()),MAX(_max.GetZ(),pv.GetZ()));
		}

		//Calculate Bounding Sphere
		_spherecenter.LinearlyInterpolate(_min,_max,0.5);
		CVector3f diagonal;
		diagonal.Sub(_max,_min);
		_sphereradius = diagonal.GetLength()*0.5;
	}

	//Multiple Sub Geometries
	else
	{
		std::vector<TXGeometry*>::iterator sgit = _subgeometries.begin();
		ULONG i=0;
		for(;sgit<_subgeometries.end();sgit++)
		{
			(*sgit)->_min.Set(FLT_MAX, FLT_MAX, FLT_MAX);
			(*sgit)->_max.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);

			for(ULONG v=0;v<(*sgit)->_nbv;v++)
			{
				if(clear)
					_vertices[i] = new TXVertex(i,(float)points[i*3],(float)points[i*3+1],(float)points[i*3+2]);
				else
					_vertices[i]->_pos.Set((float)points[i*3],(float)points[i*3+1],(float)points[i*3+2]);

				pv = _vertices[i]->_pos;
			
				(*sgit)->_min.Set(MIN((*sgit)->_min.GetX(),pv.GetX()),MIN((*sgit)->_min.GetY(),pv.GetY()),MIN((*sgit)->_min.GetZ(),pv.GetZ()));
				(*sgit)->_max.Set(MAX((*sgit)->_max.GetX(),pv.GetX()),MAX((*sgit)->_max.GetY(),pv.GetY()),MAX((*sgit)->_max.GetZ(),pv.GetZ()));

				_min.Set(MIN(_min.GetX(),pv.GetX()),MIN(_min.GetY(),pv.GetY()),MIN(_min.GetZ(),pv.GetZ()));
				_max.Set(MAX(_max.GetX(),pv.GetX()),MAX(_max.GetY(),pv.GetY()),MAX(_max.GetZ(),pv.GetZ()));
				i++;
			}

			//Calculate Bounding Sphere
			(*sgit)->_spherecenter.LinearlyInterpolate((*sgit)->_min,(*sgit)->_max,0.5);
			CVector3f radii;
			radii.Sub((*sgit)->_max,(*sgit)->_spherecenter);
			(*sgit)->_sphereradius = radii.GetLength();
		}
	}
}

void TXGeometry::GetRuntimeMeshData(CICEGeometry& geom)
{
	// loop over triangles and get them normal
	LONG tCount = geom.GetTriangleCount();
	CLongArray tIndices;
	geom.GetTrianglePointIndices(tIndices);
	CVector3f a, b, c;
	CVector3f p;

	for (LONG i=0; i<tCount; i++)
	{
		// Get Normal
		a.Sub(_vertices[tIndices[i*3]]->_pos,_vertices[tIndices[i*3+1]]->_pos);
		b.Sub(_vertices[tIndices[i*3]]->_pos,_vertices[tIndices[i*3+2]]->_pos);
		_triangles[i]->_norm.Cross(a,b);
		_triangles[i]->_norm.NormalizeInPlace();
		c.Sub(_view,_vertices[tIndices[i*3]]->_pos);
		_triangles[i]->_facing = (c.Dot(_triangles[i]->_norm)>0);
	}

	//loop vertices calculating normal
	std::vector<TXTriangle*> t;
	CVector3f n;
	LONG nbt;
	for(LONG i=0;i<_visiblevertices.size();i++)
	{
		n.SetNull();
		t = _visiblevertices[i]->_triangles;
		nbt = (long)t.size();
		for(LONG j=0;j<nbt;j++)
		{
			n += t[j]->_norm;
		}
		n.NormalizeInPlace();
		_visiblevertices[i]->_norm = n;
	}
}

void TXGeometry::GetCreaseEdges(float crease)
{
	_creases.clear();
	TXEdge* ed = NULL;
	//get crease edges
	for(ULONG e=0;e<_visibleedges.size();e++)
	{
		ed = _visibleedges[e];
		if(!ed->_isboundary)
		{
			float angle = ed->_triangles[0]->_norm.GetAngle(ed->_triangles[1]->_norm);

			if(abs(angle)>crease)
			{
				ed->_iscrease = true;
				_creases.push_back(ed);
			}
			else
				ed->_iscrease = false;
		}
	}
	_creasecached = true;
}

void TXGeometry::GetDotAndSign(CVector3f& view, float bias)
{
	CVector3f c;
	// loop over all vertices getting dot and sign
	for(ULONG i=0;i<_nbvv;i++)
	{
		c.Sub(view,_visiblevertices[i]->_pos);
		c.NormalizeInPlace();
		_visiblevertices[i]->_dot = c.Dot(_visiblevertices[i]->_norm) - bias;
		if(_visiblevertices[i]->_dot>=0)_visiblevertices[i]->_sign = 1;
		else _visiblevertices[i]->_sign = -1;
	}
}

void TXGeometry::GetVisible(TXCamera* camera)
{
	ClearVisible();

	bool multi = false;
	if(_subgeometries.size()>0)multi = true;

	if(!multi)
	{
		_infrustrum = camera->GeometryInFrustrum(this);
	
		//Intersect
		if(_infrustrum == 1)
		{
			for(ULONG p=0;p<_nbv;p++)
			{
				if(camera->PointInFrustrum(_vertices[p]->_pos))
				{
					_visiblevertices.push_back(_vertices[p]);
					_vertices[p]->SetVisible(true);
					_nbvv++;
				}
			}
		}
		else if(_infrustrum == 2)
		{
			SetAllVisible();
		}
	}

	else
	{
		std::vector<TXGeometry*>::iterator git=_subgeometries.begin();
		ULONG vid = 0;
		for(;git<_subgeometries.end();git++)
		{
			(*git)->_infrustrum = camera->GeometryInFrustrum(*git);
			if((*git)->_infrustrum)
			{
				//Inside Frustrum : All Points Visible
				if((*git)->_infrustrum == INSIDE)
				{
					for(int i=0;i<(*git)->_nbv;i++)
					{
						_visiblevertices.push_back(_vertices[vid]);
						_vertices[vid]->SetVisible(true);
						_nbvv++;
						vid++;
					}
				}
				//Intersect : Parse Points
				// Idealy we would have one Octree per SubGeometries for even faster parsing
				else
				{
					for(int i=0;i<(*git)->_nbv;i++)
					{
						if(camera->PointInFrustrum(_vertices[vid]->_pos))
						{
							_visiblevertices.push_back(_vertices[vid]);
							_vertices[vid]->SetVisible(true);
							_nbvv++;
						}
						vid++;
					}
				}
			}
			// Ouside Frustrum : Discard All Points
			else
			{
				vid += (*git)->_nbv;
			}
		}
	}
	
	CLongArray change;
	//loop over all edges
	std::vector<TXEdge*>::iterator ei;
	for(ei=_edges.begin();ei<_edges.end();ei++)
	{
		if((*ei)->_start->_visible || (*ei)->_end->_visible)
		{
			if(!(*ei)->_start->_visible) 
				change.Add((*ei)->_start->_id);
			if(!(*ei)->_end->_visible) 
				change.Add((*ei)->_end->_id);

			_visibleedges.push_back(*ei);
			(*ei)->_isvisible = true;
			_nbve++;
		}
	}

	//Enable vertices connected to a visible edge
	for(LONG e=0;e<change.GetCount();e++)
	{
		_vertices[change[e]]->_visible = true;
		_visiblevertices.push_back(_vertices[change[e]]);
		_nbvv++;
	}
	
}
/*
bool TXGeometry::GetBBoxVisibility(TXCamera* camera)
{
	//Test Bounding Box Visibility
	CVector3f bbmin, bbmax;
	bbmin.Sub(_min,camera->_pos);
	bbmax.Sub(_max,camera->_pos);

	if(bbmin.Dot(camera->_limits[0])>0 && bbmin.Dot(camera->_limits[1])>0 && bbmin.Dot(camera->_limits[2])>0 && bbmin.Dot(camera->_limits[3])>0&&
		bbmax.Dot(camera->_limits[0])>0 && bbmax.Dot(camera->_limits[1])>0 && bbmax.Dot(camera->_limits[2])>0 && bbmax.Dot(camera->_limits[3])>0)
			return true;
	return false;
}
*/
void TXGeometry::ClearVisible()
{
	_visibleedges.clear();
	_visiblevertices.clear();

	_nbvv = 0;
	_nbve = 0;

	std::vector<TXVertex*>::iterator vi = _vertices.begin();
	for(;vi<_vertices.end();vi++)
		(*vi)->SetVisible(false);

	std::vector<TXEdge*>::iterator ei = _edges.begin();
	for(;ei<_edges.end();ei++)
		(*ei)->_isvisible = false;
}

void TXGeometry::SetAllVisible()
{
	_visiblevertices.clear();
	_visibleedges.clear();
	_nbvv = _nbv;
	_nbve = _nbe;

	std::vector<TXVertex*>::iterator vi = _vertices.begin();
	for(;vi<_vertices.end();vi++)
	{
		_visiblevertices.push_back(*vi);
		(*vi)->SetVisible(true);
	}

	std::vector<TXEdge*>::iterator ei = _edges.begin();
	for(;ei<_edges.end();ei++)
	{
		_visibleedges.push_back(*ei);
		(*ei)->_isvisible = true;
	}
}

bool TXGeometry::IsSilhouette(TXEdge* e,CVector3f view, bool smooth)
{
	CVector3f c;

	if(smooth  || e->_isboundary)
	{
		if((e->_start->_sign + e->_end->_sign)== 0)
		{
			e->_issilhouette = true;
			if(e->_start->_isboundary && !e->_end->_isboundary)
				e->_weight = 1;
			else if(!e->_start->_isboundary && e->_end->_isboundary)
				e->_weight = 0;
			else
				e->_weight = ABS(e->_end->_dot)/(ABS(e->_start->_dot)+ABS(e->_end->_dot));
			return true;
		}
	}
	else
	{
		float dot1, dot2;

		c.LinearlyInterpolate(e->_start->_pos,e->_end->_pos,0.5);
		c.Sub(view,c);
		c.NormalizeInPlace();
		dot1 = c.Dot(e->_triangles[0]->_norm);
		dot2 = c.Dot(e->_triangles[1]->_norm);
		e->_weight = 0.5;

		if(dot1*dot2<0/*||(_geom->_vertices[p1id]->_sign + _geom->_vertices[p2id]->_sign)== 0*/)
		{
			e->_weight = ABS(e->_end->_dot)/(ABS(e->_start->_dot)+ABS(e->_end->_dot));
			e->_issilhouette = true;
			return true;
		}
	}

	return false;
}

TXVertex::TXVertex(LONG id,float px, float py, float pz)
{
	_id = id;
	_pos.Set(px,py,pz);
}

TXVertex::~TXVertex()
{
}

bool TXVertex::IsInCell(const CVector3f& min, const CVector3f& max)
{
	if(_pos.GetX()>=min.GetX() && _pos.GetX()<max.GetX()&&
		_pos.GetY()>=min.GetY() && _pos.GetY()<max.GetY()&&
		_pos.GetZ()>=min.GetZ() && _pos.GetZ()<max.GetZ())return true;
	return false;
}

TXEdge::TXEdge(LONG id,LONG p1, LONG p2)
{
	_id = id;
	_pid.Resize(2);
	_pid[0] = p1;
	_pid[1] = p2;
	_issilhouette = false;
	_iscrease = false;
	_isboundary = false;
	_ischecked = false;
	_weight = -1;
}

TXEdge::~TXEdge()
{
}

void TXEdge::GetNeighborEdges()
{
	std::vector<TXPolygon*>::iterator pi;
	std::vector<TXEdge*> e;
	std::vector<TXEdge*>::iterator ei;

	for(pi=_polygons.begin();pi<_polygons.end();pi++)
	{
		e = (*pi)->_edges;
		for(ei=e.begin();ei<e.end();ei++)
		{
			if((*ei)->_id != _id)
			{
				_neighbors.push_back(*ei);
			}
		}
	}
}


void TXEdge::GetNextEdges()
{
	CLongArray adjacents;
	std::vector<TXEdge*> e;
	std::vector<TXPolygon*>::iterator ip;
	std::vector<TXEdge*>::iterator ie;
	for(ip= _polygons.begin();ip<_polygons.end();ip++)
	{
		e = (*ip)->_edges;
		for(ie=e.begin();ie<e.end();ie++)
		{
			adjacents.Add((*ie)->_id);
		}
	}

	//Get Next Edges
	std::vector<TXEdge*> n = _end->_edges;
	bool isAdjacent = false;
	float angle = 0.0;
	CVector3f a, b;

	TXEdge* next = NULL;

	// Store only ONE non-adjacent edge with maximal angle
	for(std::vector<TXEdge*>::iterator ie=n.begin();ie<n.end();ie++)
	{
		if((*ie)->_id == _id)continue;

		isAdjacent = false;
		for(LONG ia=0;ia<adjacents.GetCount();ia++)
		{
			if((*ie)->_id == adjacents[ia])
			{
				isAdjacent = true;
				break;
			}
		}

		if(!isAdjacent)
		{
			a.Sub(_start->_pos,_end->_pos);
			if((*ie)->_pid[0] == _pid[1])
				b.Sub((*ie)->_end->_pos,(*ie)->_start->_pos);
			else
				b.Sub((*ie)->_start->_pos,(*ie)->_end->_pos);

			a.NormalizeInPlace();
			b.NormalizeInPlace();

			float na = a.GetAngle(b);
			if(na>angle)
			{
				angle = na;
				next = *ie;
			}
		}
	}

	if( next)
	{
		_next.push_back(next);
	}

	//Get Previous Edges
	n = _start->_edges;
	angle = 0.0;
	next = NULL;
	
	for(std::vector<TXEdge*>::iterator ip = n.begin();ip<n.end();ip++)
	{
		isAdjacent = false;
		if((*ip)->_id == _id)continue;

		for(LONG ia=0;ia<adjacents.GetCount();ia++)
		{
			if((*ip)->_id == adjacents[ia])
			{
				isAdjacent = true;
			}
		}
		
		if(!isAdjacent)
		{
			a.Sub(_end->_pos,_start->_pos);
			if((*ip)->_pid[0] == _pid[0])
				b.Sub((*ip)->_end->_pos,(*ip)->_start->_pos);
			else
				b.Sub((*ip)->_start->_pos,(*ip)->_end->_pos);

			a.NormalizeInPlace();
			b.NormalizeInPlace();

			float na = a.GetAngle(b);
			if(na>angle)
			{
				angle = na;
				next = *ip;
			}
		}
	}
	if( next)
	{
		_next.push_back(next);
	}
}

void TXEdge::GetAdjacentEdges()
{
	std::vector<TXEdge*> e;
	std::vector<TXEdge*>::iterator ei;
	float angle;

	//search for edges adjacent to first vertex
	e = _start->_edges;
	for(ei=e.begin();ei<e.end();ei++)
	{
		if((*ei)->_id != _id)
		{
			_adjacents.push_back(*ei);
			angle = GetAngle(*ei);
			_angles.Add(angle);
		}
	}

	//search for edges adjacent to second vertex
	e = _end->_edges;
	for(ei=e.begin();ei<e.end();ei++)
	{
		if((*ei)->_id != _id)
		{
			_adjacents.push_back(*ei);
			angle = GetAngle(*ei);
			_angles.Add(angle);
		}
	}
}

float TXEdge::GetAngle(TXEdge* other)
{
	ULONG p1,p2,p3,p4;
	CVector3f v1, v2;

	p1 = _pid[0];
	p2 = _pid[1];
	p3 = other->_pid[0];
	p4 = other->_pid[1];

	v1.Sub(_start->_pos,_end->_pos);
	v2.Sub(other->_start->_pos,other->_end->_pos);

	if(p1 == p4 )v1.NegateInPlace();
	else if(p4 == p1)v2.NegateInPlace();

	float angle = v1.GetAngle(v2);
	return angle;
}


ULONG TXEdge::GetIDInTriangle(TXTriangle* t)
{
	for(ULONG i=0;i<3;i++)
	{
		if((t->_pid[i] == _pid[0] && t->_pid[(i+1)%3] == _pid[1])||
			(t->_pid[i] == _pid[1] && t->_pid[(i+1)%3] == _pid[0]))return i;
	}
	return 0;
}


bool TXEdge::ShareTriangle(TXEdge*& other)
{
	std::vector<TXTriangle*> t = _triangles;

	for(LONG z=0;z<(LONG)t.size();z++)
	{
		if(other->IsInTriangle(t[z]))return true;
	}

	return false;
}

bool TXEdge::IsInTriangle(TXTriangle*& t)
{
	LONG cnt = 0;
	for(LONG y=0;y<3;y++)
	{
		if(t->_pid[y] == _pid[0] || t->_pid[y] == _pid[1])cnt++;
	}
	if(cnt == 2)return true;
	return false;
}

LONG TXEdge::SharePoint(TXEdge*& other)
{
	if(_pid[0] == other->_pid[0] || _pid[0] == other->_pid[1]) return 1;
	else if(_pid[1] == other->_pid[0] || _pid[1] == other->_pid[1]) return 2;
	else return 0;
}

bool TXEdge::Check(LONG lineid)
{
	if(_ischecked)return false;

	//cluster
	if(lineid == CLUSTER)return _isclusteredge;

	//boundary
	else if(lineid == BOUNDARY)return _isboundary;

	//crease
	else if(lineid == CREASE)return _iscrease;

	//silhouettes
	else return _issilhouette;
}

// edge touch cell if ONE of its points is inside cell
bool TXEdge::TouchCell(const CVector3f& minp, const CVector3f& maxp)
{
	/*
	if(_id<10)
	{
		Application().LogMessage(L"MIN:("+(CString)minp.GetX()+L","+(CString)minp.GetY()+L","+(CString)minp.GetZ()+L")");
		Application().LogMessage(L"MAX:("+(CString)maxp.GetX()+L","+(CString)maxp.GetY()+L","+(CString)maxp.GetZ()+L")");
		Application().LogMessage(L"Edge ID "+(CString)_id +L":("+(CString)_start->_pos.GetX()+L","+(CString)_start->_pos.GetY()+L","+(CString)_start->_pos.GetZ()+L")");
	}
	*/
	if(_start->IsInCell(minp, maxp))return true;
	if(_end->IsInCell(minp,maxp))return true;
	return false;
	/*
	int m = 4;

	CVector3f step;
	step.Sub(_end->_pos, _start->_pos);
	step.ScaleInPlace(1/4);
	CVector3f A = _start->_pos;
	CVector3f B;
	B.Add(_start->_pos,step);

	for (int i = 0; i < m; i++) 
	{
		CVector3f bmin(MIN(A.GetX(),B.GetX()),MIN(A.GetY(),B.GetY()),MIN(A.GetZ(),B.GetZ()));
		CVector3f bmax(MAX(A.GetX(),B.GetX()),MAX(A.GetY(),B.GetY()),MAX(A.GetZ(),B.GetZ()));

		if (bmin.GetX() <= maxp.GetX() && minp.GetX() <= bmax.GetX() &&
		bmin.GetY() <= maxp.GetY() && minp.GetY() <= bmax.GetY() &&
		bmin.GetZ() <= maxp.GetZ() && minp.GetZ() <= bmax.GetZ()) return true;
		A = B;
		B += step;
	}
	return false;
	*/
}


TXTriangle::TXTriangle(LONG id,LONG p1, LONG p2, LONG p3)
{
	_id = id;
	_pid.Resize(3);
	_pid[0] = p1;
	_pid[1] = p2;
	_pid[2] = p3;
}

TXPolygon::TXPolygon(LONG id, XSI::CLongArray pid)
{
	_id = id;
	_pid = pid;
}

TXPolygon::~TXPolygon()
{
}

void TXPolygon::PushEdgeData(TXEdge*& edge)
{
	// if edge already in _edges return
	for(LONG v=0;v<(LONG)_edges.size();v++)
	{
		if(_edges[v]->_id == edge->_id)return;
	}

	// check if edge belong to polygon
	LONG cnt = 0;
	for(LONG v=0;v<(LONG)_pid.GetCount();v++)
	{
		if(_pid[v] == edge->_pid[0]|| _pid[v] == edge->_pid[1])cnt++;
	}
	if(cnt == 2)
	{
		_edges.push_back(edge);
		edge->_polygons.push_back(this);
	}
}

