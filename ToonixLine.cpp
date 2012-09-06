// Toonix Data
//--------------------------------------------
#include "ToonixRegister.h"
#include "ToonixLine.h"
#include "ToonixGeometry.h"

TXPoint::TXPoint(TXPoint* p)
{
	_pos = p->_pos;
	_dir = p->_dir;
	_norm = p->_norm;
	_length = p->_length;
	_radius = p->_radius;
	_id = p->_id;
	_lineid = p->_lineid;
	_sublineid = p->_sublineid;
}
TXLine::TXLine(){};

void TXLine::Build(LONG lineid)
{
	std::vector<TXEdge*>edges;
	if(lineid<CLUSTER)return;
	else if(lineid == CLUSTER)edges = _geom->_clusteredges;
	else if(lineid == BOUNDARY)edges = _geom->_boundaries;
	else if(lineid == CREASE)edges = _geom->_creases;
	else edges = _geom->_silhouettes;

	//_view = _geom->_camera->_pos;
	
	// uncheck edges checked flag
	UncheckEdges(edges);

	LONG id,lastid,nextid,firstid,sublineid;
	sublineid = 0;

	bool test = false;

	if(!_smoothsilhouette)
	{
		for(ULONG b=0;b<edges.size();b++)
		{
			id = edges[b]->_id;
			firstid = lastid = id;
			TXChain* chain = new TXChain;
			chain->_lineid = lineid;
			chain->_sublineid = b;
			chain->_firstid = id;
			chain->_lastid = -1;

			GetPoint(id,-1,id, chain);
			_chains.push_back(chain);
		}
	}
	else
	{
		_lastvertexid = -1;
		for(ULONG b=0;b<edges.size();b++)
		{
			id = edges[b]->_id;
			firstid = lastid = id;

			if(!edges[b]->_ischecked)
			{
				TXChain* chain = new TXChain;
				_breakline = false;
				bool started = false;
				chain->_lineid = lineid;
				chain->_sublineid = sublineid;
				chain->_closed = false;
				
				chain->_firstid = id;
				chain->_lastid = -1;

				//Application().LogMessage(L"-----------------------------------------------------------------");
				nextid = GetNextID(id,-1,lineid);
				
				if(nextid == -1|| lineid<0)
				{
					GetPoint(id,nextid,lastid,chain);
					if(lineid>=0)
						_geom->_edges[id]->_ischecked = true;

				}
				
				while(nextid != -1)
				{
					chain->_lastid = nextid;
					_geom->_edges[nextid]->_ischecked = true;
					lastid = id;
					id = nextid;
					nextid = GetNextID(id,lastid,lineid);
					GetPoint(id,nextid,lastid,chain);
				}
				
				
				// check from start
				nextid = GetNextID(firstid,-1,lineid);
				if(nextid != -1)
				{
					//Application().LogMessage(L"We revert chain!!!");
					chain->Revert();
					
					id = firstid;
					GetPoint(id,nextid,lastid, chain);
					while(nextid != -1)
					{
						chain->_lastid = nextid;
						_geom->_edges[nextid]->_ischecked = true;
						lastid = id;
						id = nextid;
						nextid = GetNextID(id,lastid,lineid);
						GetPoint(id,nextid,lastid, chain);
					}
				}
				
				sublineid++;

				// is this chain Closed??
				//Application().LogMessage(L"First ID: "+(CString)chain->_firstid+L", last ID: "+(CString)chain->_lastid );
				if(chain->_firstid == chain->_lastid)chain->_closed = true;
				//Application().LogMessage(L"Chain Closed: "+(CString)chain->_closed );
				
				// Extend Chains
				if(_extend!=0)
				{
					if(chain->_points.size()==1)
					{
						chain->_points[0]->_length *= (1+_extend);
					}
					else
					{
						chain->_points[0]->_length *= (1+_extend *0.5);
						CVector3f offset;
						offset.Sub(chain->_points[0]->_pos,chain->_points[1]->_pos);
						offset.NormalizeInPlace();
						offset *= _extend *0.5;
						chain->_points[0]->_pos += offset;

						chain->_points[1]->_length *= (1+_extend *0.5);
						ULONG last = chain->_points.size()-1;
						offset.Sub(chain->_points[last]->_pos,chain->_points[last-1]->_pos);
						offset.NormalizeInPlace();
						offset *= _extend *0.5;
						chain->_points[last]->_pos += offset;
					}
				}

				// Push Line Chains data
				chain->FilterPoints(_filterpoints);
				_chains.push_back(chain);
			}
		}
	}
}

void TXLine::GetSilhouettes(ULONG v)
{
	// clear silhouette data
	ClearSilhouettes();

	_geom->GetDotAndSign(_eye[v], _bias[v]);

	// loop over edges 
	std::vector<TXEdge*>::iterator ei;
	for(ei=_geom->_visibleedges.begin();ei<_geom->_visibleedges.end();ei++)
	{
		if(_geom->IsSilhouette(*ei,_eye[v],_smoothsilhouette))
		{
			_geom->_silhouettes.push_back((*ei));
		}
	}

}

void TXLine::GetBoundaries()
{
	ClearChains();
	_geom->_boundaries.clear();

	std::vector<TXEdge*>::iterator ei = _geom->_visibleedges.begin();
	for(;ei<_geom->_visibleedges.end();ei++)
	{
		if((*ei)->_isboundary)
			_geom->_boundaries.push_back(*ei);
	}
	//_smoothsilhouette = true;
}

void TXLine::GetPoint(LONG edgeid, LONG nextedgeid, ULONG lastid, TXChain* chain)
{
	TXPoint* data = new TXPoint;

	CVector3f pos, dir, norm, extend;
	
	float l;
	
	if( !_smoothsilhouette || chain->_lineid <0 || nextedgeid == -1 )
	{
		int v1 = _geom->_edges[edgeid]->_pid[0];
		int v2 = _geom->_edges[edgeid]->_pid[1];

		dir.Sub(_geom->_vertices[v1]->_pos,_geom->_vertices[v2]->_pos);

		// First point
		if(chain->_points.size() == 0)
		{
			/*
			dir.NegateInPlace();
			
			if(nextedgeid == -1)
				_lastvertexid = v2;
			else
			{
				int v3 = _geom->_edges[nextedgeid]->_pid[0];
				int v4 = _geom->_edges[nextedgeid]->_pid[1];

				if(v4 == v1)
				{
					dir.NegateInPlace();
					_lastvertexid = v2;
				}
				else if(v4 == v2)
				{
					dir.NegateInPlace();
					_lastvertexid = v1;
				}
			}*/
			
		}
		else
		{
			if(v1 != _lastvertexid)
			{
				dir.NegateInPlace();
				_lastvertexid = v1;
			}
			else
			{
				_lastvertexid = v2;
			}
		}

		l = (float)dir.GetLength();

		//Get point position
		pos.LinearlyInterpolate(_geom->_vertices[v1]->_pos,_geom->_vertices[v2]->_pos,0.5);
		norm.LinearlyInterpolate(_geom->_vertices[v1]->_norm,_geom->_vertices[v2]->_norm,0.5);
		norm.NormalizeInPlace();
	}
	else
	{
		CVector3f lv1, lv2,ln1,ln2;
		CVector3f en1, en2;
		CVector3f c;
		
		float w1 = _geom->_edges[edgeid]->_weight;
		float w2 = _geom->_edges[nextedgeid]->_weight;

		int v1 = _geom->_edges[edgeid]->_pid[0];
		int v2 = _geom->_edges[edgeid]->_pid[1];
		int v3 = _geom->_edges[nextedgeid]->_pid[0];
		int v4 = _geom->_edges[nextedgeid]->_pid[1];

		//Get point position
		lv1.LinearlyInterpolate(_geom->_vertices[v2]->_pos,_geom->_vertices[v1]->_pos,w1);
		lv2.LinearlyInterpolate(_geom->_vertices[v4]->_pos,_geom->_vertices[v3]->_pos,w2);
		ln1.LinearlyInterpolate(_geom->_vertices[v2]->_norm,_geom->_vertices[v1]->_norm,w1);
		ln2.LinearlyInterpolate(_geom->_vertices[v4]->_norm,_geom->_vertices[v3]->_norm,w2);

		dir.Sub(lv2,lv1);
		l = dir.GetLength();

		pos.LinearlyInterpolate(lv1,lv2,0.5);
		norm.LinearlyInterpolate(ln1,ln2,0.5);
		norm.NormalizeInPlace();
	}

	//Set Datas
	data->_pos = pos;
	data->_length = l;
	data->_dir = dir;
	data->_norm = norm;
	data->_radius = _width;
	data->_lineid = chain->_lineid;
	data->_sublineid = chain->_sublineid;

	// push line data
	chain->AddPoint(data);
	_nbp++;
}

LONG TXLine::GetNextID(LONG edgeid, LONG lastid, LONG lineid)
{
	LONG eid;
	std::vector<TXEdge*> e;
	std::vector<TXEdge*>::iterator ei;

	if(lineid==CREASE||lineid==CLUSTER)
		e = _geom->_edges[edgeid]->_next;
	else if(lineid==BOUNDARY)
		e = _geom->_edges[edgeid]->_adjacents;
	else	
		e = _geom->_edges[edgeid]->_neighbors;

	for(ei=e.begin();ei<e.end();ei++)
	{
		eid = (*ei)->_id;
		//Application().LogMessage(L"Edge ID "+(CString)edgeid +L" Neighbors : "+(CString)eid);
		if(eid == edgeid)continue;

		if((*ei)->Check(lineid))
		{
			//Application().LogMessage(L"Edge ID isn't checked...");
			if(eid !=lastid)
				return eid;
		}
	}
	return -1;
}


bool TXLine::BreakLine(LONG edgeid, LONG nextid)
{
	CVector3f a,b;
	LONG p1id, p2id, p3id, p4id;
	p1id = _geom->_edges[edgeid]->_pid[0];
	p2id = _geom->_edges[edgeid]->_pid[1];
	p3id = _geom->_edges[nextid]->_pid[0];
	p4id = _geom->_edges[nextid]->_pid[1];

	if(p1id == p3id)
	{
		a.Sub(_geom->_vertices[p1id]->_pos,_geom->_vertices[p2id]->_pos);
		b.Sub(_geom->_vertices[p4id]->_pos,_geom->_vertices[p3id]->_pos);
	}

	else if(p1id == p4id)
	{
		a.Sub(_geom->_vertices[p1id]->_pos,_geom->_vertices[p2id]->_pos);
		b.Sub(_geom->_vertices[p3id]->_pos,_geom->_vertices[p4id]->_pos);
	}

	else if(p2id == p3id)
	{
		a.Sub(_geom->_vertices[p2id]->_pos,_geom->_vertices[p1id]->_pos);
		b.Sub(_geom->_vertices[p4id]->_pos,_geom->_vertices[p3id]->_pos);
	}	

	else
	{
		a.Sub(_geom->_vertices[p2id]->_pos,_geom->_vertices[p1id]->_pos);
		b.Sub(_geom->_vertices[p3id]->_pos,_geom->_vertices[p4id]->_pos);
	}
	a.NormalizeInPlace();
	b.NormalizeInPlace();

	double angle = a.GetAngle(b);
	double _break = 33.0;

	if(angle<_break)
	{
		double dangle = RadiansToDegrees(angle);
		//Application().LogMessage(L"Will Break Chain : Angle between edge id : "+(CString)edgeid+L", and edge id : "+(CString)nextid +" ---> "+(CString)dangle);
		return true;
	}

	return false;
}

void TXLine::UncheckEdges(std::vector<TXEdge*>& inedges)
{
	std::vector<TXEdge*>::iterator ei = inedges.begin();
	for(;ei<inedges.end();ei++)
	{
		(*ei)->_ischecked = false;
	}
}

void TXLine::ClearSilhouettes()
{
	_geom->_silhouettes.clear();
	std::vector<TXEdge*>::iterator ei = _geom->_visibleedges.begin();
	for(;ei<_geom->_visibleedges.end();ei++)
	{
		(*ei)->_issilhouette = false;
	}
}

void TXLine::ClearDatas()
{

}

void TXLine::EmptyData()
{
	ClearChains();
	Build(CLUSTER-1);
}
/*
CRotationf TXLine::LookAt(CRotationf& io_rot, CVector3f& dir, CVector3f& up)
{
	CVector3f y(0,1,0);
	CVector3f z(0,0,1);
	CVector3f axis;
	CRotationf r;
	CMatrix3f m;

	dir.NormalizeInPlace();
	up.NormalizeInPlace();

	float angle = -(dir.GetAngle(y));

	axis.Cross(dir,y);
	io_rot.Set(axis,angle);

	CQuaternionf q = io_rot.GetQuaternion();
	float xx = q.GetX()*q.GetX();
	float xy = q.GetX()*q.GetY();
	float xz = q.GetX()*q.GetZ();
	float xw = q.GetX()*q.GetW();
	float yy = q.GetY()*q.GetY();
	float yz = q.GetY()*q.GetZ();
	float yw = q.GetY()*q.GetW();
	float zz = q.GetZ()*q.GetZ();
	float zw = q.GetZ()*q.GetW();

//	m.Set(1-2*(yy+zz),2*(xy+zw),2*(xz-yw),0,
//			2*(xy-zw),1-2*(xx+zz),2*(yz+xw),0,
//			2*(xz+yw),2*(yz-xw),1-2*(xx+yy),0,
//			0,0,0,1);

	m.Set(1-2*(yy+zz),2*(xy+zw),2*(xz-yw),
			2*(xy-zw),1-2*(xx+zz),2*(yz+xw),
			2*(xz+yw),2*(yz-xw),1-2*(xx+yy));

	z.MulByMatrix3InPlace(m);

	axis.Cross(dir,up);
	axis.NormalizeInPlace();
	angle = axis.GetAngle(z);

	axis.Cross(z,axis);
	r.Set(axis,angle);

	io_rot.Mul(r);
	return io_rot;
}
*/

TXLine::~TXLine()
{
	ClearChains();
}

void TXLine::ClearChains()
{
	for(ULONG i=0;i<_chains.size();i++)
	{
		_chains[i]->Clear();
		delete _chains[i];
	}
	_chains.clear();
	_nbp = 0;
}

void TXLine::Append(TXLine* line)
{
	std::vector<TXChain*>::iterator it;
	std::vector<TXPoint*>::iterator ip;
	ULONG nbp = _nbp;
	for(it =line->_chains.begin();it<line->_chains.end();it++)
	{
		TXChain* c = new TXChain();
		for(ip =(*it)->_points.begin();ip<(*it)->_points.end();ip++)
		{
			TXPoint* p= new TXPoint(*ip);
			c->AddPoint(p);
			_nbp++;
		}
		c->_lineid = (*it)->_lineid;
		c->_sublineid = (*it)->_lineid;
		c->_closed = (*it)->_closed;
		
		c->_firstid = (*it)->_firstid;
		c->_lastid = (*it)->_lastid;
		_chains.push_back(c);
	}
}

void TXChain::AddPoint(TXPoint *&point)
{
	_points.push_back(point);
}

void TXChain::Revert()
{
	std::reverse(_points.begin(),_points.end());
	for(ULONG p=0;p<_points.size();p++)_points[p]->_dir.NegateInPlace();
	LONG tmpid = _firstid;
	_firstid = _lastid;
	_lastid = tmpid;
}

void TXChain::Clear()
{
	for(LONG c=0;c<(LONG)_points.size();c++)delete _points[c];
	_points.clear();
}

void TXChain::FilterPoints(float filter)
{
	std::vector<TXPoint*>::iterator ip;
	CVector3f a;
	CVector3f b;
	CVector3f c;
	float l;
	ip = _points.begin();
	a = (*ip)->_pos;
	ip++;
	while(ip<_points.end()-1)
	{
		b = (*ip)->_pos;
		c.Sub(b,a);
		l = c.GetLength();
		if(l<filter)
		{
			_points.erase(ip);
		}
		else
		{
			a = b;
			ip++;
		}
	}
}

