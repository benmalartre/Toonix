// Toonix Data
//--------------------------------------------
#include "ToonixRegister.h"
#include "ToonixLine.h"
#include "ToonixGeometry.h"

TXPoint::TXPoint(TXPoint* p)
{
	m_pos = p->m_pos;
	m_dir = p->m_dir;
	m_norm = p->m_norm;
	m_length = p->m_length;
	m_radius = p->m_radius;
	m_id = p->m_id;
	m_lineid = p->m_lineid;
	m_sublineid = p->m_sublineid;
}
TXLine::TXLine(){};

void TXLine::Build(LONG lineid)
{
	std::vector<TXEdge*>edges;
	if(lineid<INTERSECTION)return;
	else if(lineid == INTERSECTION)edges = m_geom->m_intersections;
	else if(lineid == CLUSTER)edges = m_geom->m_clusteredges;
	else if(lineid == BOUNDARY)edges = m_geom->m_boundaries;
	else if(lineid == CREASE)edges = m_geom->m_creases;
	else edges = m_geom->m_silhouettes;


	//_view = _geom->_camera->_pos;
	
	// uncheck edges checked flag
	UncheckEdges(edges);

	LONG id,lastid,nextid,firstid,sublineid;
	sublineid = 0;

	bool test = false;

	if(test)
	{
		for(ULONG b=0;b<edges.size();b++)
		{
			id = edges[b]->m_id;
			firstid = lastid = id;
			TXChain* chain = new TXChain;
			chain->m_lineid = lineid;
			chain->m_sublineid = b;
			chain->m_firstid = id;
			chain->m_lastid = -1;

			GetPoint(id,-1,id, chain);
			m_chains.push_back(chain);
		}
	}
	else
	{
		m_lastvertexid = -1;
		for(ULONG b=0;b<edges.size();b++)
		{
			id = edges[b]->m_id;
			firstid = lastid = id;

			if(!edges[b]->m_ischecked)
			{
				TXChain* chain = new TXChain;
				m_breakline = false;
				bool started = false;
				chain->m_lineid = lineid;
				chain->m_sublineid = sublineid;
				chain->m_closed = false;
				
				chain->m_firstid = id;
				chain->m_lastid = -1;

				//Application().LogMessage(L"-----------------------------------------------------------------");
				nextid = GetNextID(id,-1,lineid);
				//Application().LogMessage(L" ID : "+(CString)id+" ,Next ID : "+(CString)nextid);
				
				if(nextid == -1|| lineid<0)
				{
					GetPoint(id,nextid,lastid,chain);
					//if(lineid>=0)
					m_geom->m_edges[id]->m_ischecked = true;

				}
				
				while(nextid != -1)
				{
					chain->m_lastid = nextid;
					m_geom->m_edges[nextid]->m_ischecked = true;
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
						chain->m_lastid = nextid;
						m_geom->m_edges[nextid]->m_ischecked = true;
						lastid = id;
						id = nextid;
						nextid = GetNextID(id,lastid,lineid);
						GetPoint(id,nextid,lastid, chain);
					}
				}
				
				sublineid++;

				// is this chain Closed??
				//Application().LogMessage(L"First ID: "+(CString)chain->_firstid+L", last ID: "+(CString)chain->_lastid );
				if(chain->m_firstid == chain->m_lastid)chain->m_closed = true;
				//Application().LogMessage(L"Chain Closed: "+(CString)chain->_closed );
				
				// Extend Chains
				if(m_extend!=0)
				{
					if(chain->m_points.size()==1)
					{
						chain->m_points[0]->m_length *= (1+m_extend);
					}
					else
					{
						chain->m_points[0]->m_length *= (1+m_extend *0.5);
						CVector3f offset;
						offset.Sub(chain->m_points[0]->m_pos,chain->m_points[1]->m_pos);
						offset.NormalizeInPlace();
						offset *= m_extend *0.5;
						chain->m_points[0]->m_pos += offset;

						chain->m_points[1]->m_length *= (1+m_extend *0.5);
						ULONG last = chain->m_points.size()-1;
						offset.Sub(chain->m_points[last]->m_pos,chain->m_points[last-1]->m_pos);
						offset.NormalizeInPlace();
						offset *= m_extend *0.5;
						chain->m_points[last]->m_pos += offset;
					}
				}

				// Push Line Chains data
				chain->FilterPoints(m_filterpoints);
				m_chains.push_back(chain);
			}
		}
	}
}

/*
void TXLine::GetDualSilhouettes(ULONG v)
{
	ClearSilhouettes();
	std::vector<TXEdge*> silhouettes = _geom->_silhouettes;
	
	CVector3f pos = _eye[v];
	float bias = _bias[v];
	_dualmesh[0].FindSilhouettes(CVector3f(pos.GetY(), pos.GetZ(), 1), pos.GetX(),silhouettes,bias);
	_dualmesh[1].FindSilhouettes(CVector3f(pos.GetZ(), 1, pos.GetX()), pos.GetY(),silhouettes,bias);
	_dualmesh[2].FindSilhouettes(CVector3f(1, pos.GetX(), pos.GetY()), pos.GetZ(),silhouettes,bias);
	_dualmesh[3].FindSilhouettes(pos, 1, silhouettes,bias);
	_dualmesh[4].FindSilhouettes(CVector3f(pos.GetY(), pos.GetZ(), 1), -pos.GetX(),silhouettes,bias);
	_dualmesh[5].FindSilhouettes(CVector3f(pos.GetZ(), 1, pos.GetX()), -pos.GetY(),silhouettes,bias);
	_dualmesh[6].FindSilhouettes(CVector3f(1, pos.GetX(), pos.GetY()), -pos.GetZ(),silhouettes,bias);
	_dualmesh[7].FindSilhouettes(CVector3f(), -1, silhouettes,bias);

}
*/
void TXLine::GetSilhouettes(ULONG v)
{
	//FindSilhouettes(const CVector3f& n, float d, std::vector<TXEdge*>& silhouettes, float bias);
	// clear silhouette data
	ClearSilhouettes();

	m_geom->GetDotAndSign(m_eye[v], m_bias[v]);

	// loop over edges 
	std::vector<TXEdge*>::iterator ei;
	for(ei=m_geom->m_visibleedges.begin();ei<m_geom->m_visibleedges.end();ei++)
	{
		if(m_geom->IsSilhouette(*ei,m_eye[v],m_smoothsilhouette))
		{
			m_geom->m_silhouettes.push_back((*ei));
		}
	}

}

void TXLine::GetBoundaries()
{
	ClearChains();
	m_geom->m_boundaries.clear();

	std::vector<TXEdge*>::iterator ei = m_geom->m_visibleedges.begin();
	for(;ei<m_geom->m_visibleedges.end();ei++)
	{
		if((*ei)->m_isboundary)
			m_geom->m_boundaries.push_back(*ei);
	}
	//_smoothsilhouette = true;
}

void TXLine::GetIntersections()
{
	ClearChains();
	m_geom->GetIntersectionEdges();
}

ULONG TXLine::GetNbPoints()
{
	std::vector<TXChain*>::iterator c = m_chains.begin();
	std::vector<TXPoint*>::iterator p;
	//Application().LogMessage(L"Nb Chains : "+(CString)(ULONG)_chains.size());
	m_nbp=0;

	for(;c<m_chains.end();c++)
	{
		p=(*c)->m_points.begin();
		for(;p<(*c)->m_points.end();p++)m_nbp++;
	}
	return m_nbp;
}


void TXLine::GetPoint(LONG edgeid, LONG nextedgeid, ULONG lastid, TXChain* chain)
{
	TXPoint* data = new TXPoint;

	CVector3f pos, dir, norm, extend;
	
	float l;
	
	if(((m_smoothsilhouette && chain->m_lineid == SILHOUETTE)|| chain->m_lineid == INTERSECTION)&& nextedgeid != -1)
	{
		//Application().LogMessage(L"Edge ID : "+(CString)edgeid+L", Next Edge ID : "+(CString)nextedgeid);
		CVector3f lv1, lv2,ln1,ln2;
		CVector3f en1, en2;
		CVector3f c;
		
		float w1 = m_geom->m_edges[edgeid]->m_weight;
		float w2 = m_geom->m_edges[nextedgeid]->m_weight;

		int v1 = m_geom->m_edges[edgeid]->m_pid[0];
		int v2 = m_geom->m_edges[edgeid]->m_pid[1];
		int v3 = m_geom->m_edges[nextedgeid]->m_pid[0];
		int v4 = m_geom->m_edges[nextedgeid]->m_pid[1];

		//Get point position
		lv1.LinearlyInterpolate(m_geom->m_vertices[v2]->m_pos,m_geom->m_vertices[v1]->m_pos,w1);
		lv2.LinearlyInterpolate(m_geom->m_vertices[v4]->m_pos,m_geom->m_vertices[v3]->m_pos,w2);
		ln1.LinearlyInterpolate(m_geom->m_vertices[v2]->m_norm,m_geom->m_vertices[v1]->m_norm,w1);
		ln2.LinearlyInterpolate(m_geom->m_vertices[v4]->m_norm,m_geom->m_vertices[v3]->m_norm,w2);

		dir.Sub(lv2,lv1);
		l = dir.GetLength();

		pos.LinearlyInterpolate(lv1,lv2,0.5);
		norm.LinearlyInterpolate(ln1,ln2,0.5);
		norm.NormalizeInPlace();
	}

	//if( !_smoothsilhouette || chain->_lineid <0 || nextedgeid == -1 )
	else
	{
		int v1 = m_geom->m_edges[edgeid]->m_pid[0];
		int v2 = m_geom->m_edges[edgeid]->m_pid[1];

		dir.Sub(m_geom->m_vertices[v1]->m_pos, m_geom->m_vertices[v2]->m_pos);

		// First point
		if(chain->m_points.size() == 0)
		{
			
			dir.NegateInPlace();
			
			if(nextedgeid == -1)
				m_lastvertexid = v2;
			else
			{
				int v3 = m_geom->m_edges[nextedgeid]->m_pid[0];
				int v4 = m_geom->m_edges[nextedgeid]->m_pid[1];

				if(v4 == v1)
				{
					dir.NegateInPlace();
					m_lastvertexid = v2;
				}
				else if(v4 == v2)
				{
					dir.NegateInPlace();
					m_lastvertexid = v1;
				}
			}
			
		}
		else
		{
			if(v1 != m_lastvertexid)
			{
				dir.NegateInPlace();
				m_lastvertexid = v1;
			}
			else
			{
				m_lastvertexid = v2;
			}
		}

		l = (float)dir.GetLength();

		//Get point position
		pos.LinearlyInterpolate(m_geom->m_vertices[v1]->m_pos,m_geom->m_vertices[v2]->m_pos,0.5);
		norm.LinearlyInterpolate(m_geom->m_vertices[v1]->m_norm,m_geom->m_vertices[v2]->m_norm,0.5);
		norm.NormalizeInPlace();
	}
	

	//Set Datas
	data->m_pos = pos;
	data->m_length = l;
	data->m_dir = dir;
	data->m_norm = norm;
	data->m_radius = m_width;
	data->m_lineid = chain->m_lineid;
	data->m_sublineid = chain->m_sublineid;
	data->m_color = CColor4f(0.0,0.0,0.0,1.0);

	// push line data
	chain->AddPoint(data);
	m_nbp++;
}

LONG TXLine::GetNextID(LONG edgeid, LONG lastid, LONG lineid)
{
	LONG eid;
	std::vector<TXEdge*> e;
	std::vector<TXEdge*>::iterator ei;

	if(lineid==CREASE||lineid==CLUSTER)
		e = m_geom->m_edges[edgeid]->m_next;
	else if(lineid==BOUNDARY)
		e = m_geom->m_edges[edgeid]->m_adjacents;
	else	
		e = m_geom->m_edges[edgeid]->m_neighbors;

	//CString sNeighbors = L"Edge ID "+(CString)edgeid+L" Non Intersection Neighbors : (";

	for(ei=e.begin();ei<e.end();ei++)
	{
		eid = (*ei)->m_id;
		//Application().LogMessage(L"Edge ID "+(CString)edgeid +L" Neighbors : "+(CString)eid);
		if(eid == edgeid)continue;
		if((*ei)->Check(lineid))
		{
			//Application().LogMessage(L"Edge ID isn't checked...");
			if(eid !=lastid)
				return eid;
		}
		//else 
			//sNeighbors+=(CString)(*ei)->_id+L",";
	}

	//sNeighbors += ")";
	//Application().LogMessage(sNeighbors);
	return -1;
}


bool TXLine::BreakLine(LONG edgeid, LONG nextid)
{
	CVector3f a,b;
	LONG p1id, p2id, p3id, p4id;
	p1id = m_geom->m_edges[edgeid]->m_pid[0];
	p2id = m_geom->m_edges[edgeid]->m_pid[1];
	p3id = m_geom->m_edges[nextid]->m_pid[0];
	p4id = m_geom->m_edges[nextid]->m_pid[1];

	if(p1id == p3id)
	{
		a.Sub(m_geom->m_vertices[p1id]->m_pos, m_geom->m_vertices[p2id]->m_pos);
		b.Sub(m_geom->m_vertices[p4id]->m_pos, m_geom->m_vertices[p3id]->m_pos);
	}

	else if(p1id == p4id)
	{
		a.Sub(m_geom->m_vertices[p1id]->m_pos, m_geom->m_vertices[p2id]->m_pos);
		b.Sub(m_geom->m_vertices[p3id]->m_pos, m_geom->m_vertices[p4id]->m_pos);
	}

	else if(p2id == p3id)
	{
		a.Sub(m_geom->m_vertices[p2id]->m_pos, m_geom->m_vertices[p1id]->m_pos);
		b.Sub(m_geom->m_vertices[p4id]->m_pos, m_geom->m_vertices[p3id]->m_pos);
	}	

	else
	{
		a.Sub(m_geom->m_vertices[p2id]->m_pos, m_geom->m_vertices[p1id]->m_pos);
		b.Sub(m_geom->m_vertices[p3id]->m_pos, m_geom->m_vertices[p4id]->m_pos);
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
		(*ei)->m_ischecked = false;
	}
}

void TXLine::ClearSilhouettes()
{
	m_geom->m_silhouettes.clear();
	std::vector<TXEdge*>::iterator ei = m_geom->m_visibleedges.begin();
	for(;ei<m_geom->m_visibleedges.end();ei++)
	{
		(*ei)->m_issilhouette = false;
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
	for(ULONG i=0;i<m_chains.size();i++)
	{
		m_chains[i]->Clear();
		delete m_chains[i];
	}
	m_chains.clear();
	m_nbp = 0;
}

void TXLine::Append(TXLine* line)
{
	std::vector<TXChain*>::iterator it;
	std::vector<TXPoint*>::iterator ip;
	ULONG nbp = m_nbp;
	for(it =line->m_chains.begin();it<line->m_chains.end();it++)
	{
		TXChain* c = new TXChain();
		for(ip =(*it)->m_points.begin();ip<(*it)->m_points.end();ip++)
		{
			TXPoint* p= new TXPoint(*ip);
			c->AddPoint(p);
			m_nbp++;
		}
		c->m_lineid = (*it)->m_lineid;
		c->m_sublineid = (*it)->m_lineid;
		c->m_closed = (*it)->m_closed;
		
		c->m_firstid = (*it)->m_firstid;
		c->m_lastid = (*it)->m_lastid;
		m_chains.push_back(c);
	}
}

void TXChain::AddPoint(TXPoint *&point)
{
	m_points.push_back(point);
}

void TXChain::Revert()
{
	std::reverse(m_points.begin(),m_points.end());
	for(ULONG p=0;p<m_points.size();p++)m_points[p]->m_dir.NegateInPlace();
	LONG tmpid = m_firstid;
	m_firstid = m_lastid;
	m_lastid = tmpid;
}

void TXChain::Clear()
{
	for(LONG c=0;c<(LONG)m_points.size();c++)delete m_points[c];
	m_points.clear();
}

void TXChain::FilterPoints(float filter)
{
	std::vector<TXPoint*>::iterator ip;
	CVector3f a;
	CVector3f b;
	CVector3f c;
	float l;
	ip = m_points.begin();
	a = (*ip)->m_pos;
	ip++;
	while(ip<m_points.end()-1)
	{
		b = (*ip)->m_pos;
		c.Sub(b,a);
		l = c.GetLength();
		if(l<filter)
		{
			m_points.erase(ip);
		}
		else
		{
			a = b;
			ip++;
		}
	}
}

