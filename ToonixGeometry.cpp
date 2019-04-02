// Toonix Data
//--------------------------------------------
#include "ToonixRegister.h"
#include "ToonixGeometry.h"
#include "ToonixKDTree.h"

TXGeometry::TXGeometry()
{
	m_init = false;
	m_nbv=0;
	m_creasecached = false;
	m_kdtree = new TXKDTree(CVector3f(),CVector3f());
}

TXGeometry::~TXGeometry()
{
	ClearDatas();
}

bool TXGeometry::GetDirtyState(CICEGeometry& geom)
{
	Application().LogMessage(L"Get Dirty State Called!!!");

	bool dirty = geom.IsDirty(CICEGeometry::siAnyDirtyState);
	if(!dirty) return false;

	ULONG bTypeDirtyState = geom.IsDirty( CICEGeometry::siGeometryTypeDirtyState ) ? 1 : 0;
	ULONG bTransfoDirtyState = geom.IsDirty( CICEGeometry::siTransformationDirtyState ) ? 1 : 0;
	ULONG bPointsDirtyState = geom.IsDirty( CICEGeometry::siPointPositionDirtyState ) ? 1 : 0;
	ULONG bTopologyDirtyState = geom.IsDirty( CICEGeometry::siTopologyDirtyState ) ? 1 : 0;

	m_dirty = bTypeDirtyState + bTransfoDirtyState + bPointsDirtyState + bTopologyDirtyState;
	Application().LogMessage(L"Geometry Dirty State : "+(CString)m_dirty);
	geom.ClearState();
	/*
	ULONG nbsg = geom.GetSubGeometryCount();
	if(nbsg)
	{
		for(ULONG i=0;i<nbsg;i++)
		{
			GetDirtyState(geom.GetSubGeometry(i));
		}
	}
	else
	{
		geom.ClearState();
	}
	*/

	return false;
	
	//CICEGeometry::DirtyState

	
}

/*
void TXGeometry::DeleteKDTree()
{
	
}
*/

void TXGeometry::BuildKDTree()
{
	Application().LogMessage(L" Build KD Tree Called...");
	if(!m_kdtree == NULL)
		delete m_kdtree;

	m_kdtree = new TXKDTree(m_min,m_max);

	float3* points = new float3[m_vertices.size()];
	CVector3f pos;
	for(ULONG it=0;it<m_vertices.size();it++)
	{
		pos = m_vertices[it]->m_pos;
		points[it].x = pos.GetX();
		points[it].y = pos.GetY();
		points[it].z = pos.GetZ();
		points[it].id = m_vertices[it]->m_id;

	}
	m_kdtree->Build(points, m_vertices.size(), m_kdtree, 7, 100, 0);

}

void TXGeometry::GetSubGeometries(CICEGeometry& geom)
{
	std::vector<TXGeometry*>::iterator git=m_subgeometries.begin();
	for(;git<m_subgeometries.end();git++)
		delete (*git);

	ULONG nbsg = geom.GetSubGeometryCount();
	//Application().LogMessage(L"Nb Sub Geometries : "+(CString)nbsg);
	if(nbsg>0)
	{
		ULONG basevertexID = 0;
		ULONG baseedgeID = 0;
		ULONG basetriangleID = 0;

		m_subgeometries.resize(nbsg);
		m_isleaf = false;
		m_isroot = true;
		for(ULONG i=0;i<nbsg;i++)
		{
			TXGeometry* sub = new TXGeometry();
			CICEGeometry subgeom = geom.GetSubGeometry(i);
			ULONG nbv = subgeom.GetPointPositionCount();
			// sub geometry index
			sub->m_id = i;
			sub->m_isleaf = true;
			sub->m_isroot = false;
		
			// get nb points
			sub->m_nbv = nbv;
			sub->m_basevertexID = basevertexID;

			//get nb edges
			sub->m_nbe = subgeom.GetSegmentCount();
			sub->m_baseedgeID = baseedgeID;

			// get nb triangles
			sub->m_nbt = subgeom.GetTriangleCount();
			sub->m_basetriangleID = basetriangleID;

			// set TXGeometry in Array
			m_subgeometries[i] = sub;

			//increment counter
			basevertexID += sub->m_nbv;
			baseedgeID += sub->m_nbe;
			basetriangleID += sub->m_nbt;
		}
	}
	else 
	{
		m_isleaf = true;
		m_isroot = true;
	}
}

void TXGeometry::Unlit()
{
	for(ULONG l=0;l<m_vertices.size();l++)
	{
		m_vertices[l]->m_lit = false;	
	}

	for(ULONG v=0;v<m_triangles.size();v++)
	{
		m_triangles[v]->m_lit = false;	
	}
}

void TXGeometry::ClearDatas()
{
	m_boundaries.clear();
	m_creases.clear();
	m_creasecached = false;
	m_clusteredges.clear();
	m_silhouettes.clear();
	m_intersections.clear();

	ClearVisible();

	std::vector<TXGeometry*>::iterator it = m_subgeometries.begin();
	for(;it<m_subgeometries.end();it++)
	{
		delete *it;
	}
	m_subgeometries.clear();

	if(m_isroot)
	{
		for(LONG a = 0;a<(LONG)m_vertices.size();a++)delete m_vertices[a];
		m_vertices.clear();

		for(LONG a = 0;a<(LONG)m_edges.size();a++)delete m_edges[a];
		m_edges.clear();

		for(LONG a = 0;a<(LONG)m_triangles.size();a++)delete m_triangles[a];
		m_triangles.clear();

		for(LONG a = 0;a<(LONG)m_polygons.size();a++)delete m_polygons[a];
		m_polygons.clear();
	}
	else
	{
		m_vertices.clear();
		m_edges.clear();
		m_triangles.clear();
		m_polygons.clear();
		
	}
	m_nbv = m_nbe = 0;
	//delete _kdtree;

}

void TXGeometry::GetCachedMeshData(CICEGeometry& geom)
{
	ClearDatas();
	GetSubGeometries(geom);
	GetPointPosition(geom);

	//loop over all edges
	m_nbe = geom.GetSegmentCount();
	CLongArray sIndices;
	geom.GetSegmentIndices(sIndices);
	m_edges.resize(m_nbe);

	for (ULONG i=0; i<m_nbe; i++)
	{
		m_edges[i] = new TXEdge(i,sIndices[i*2],sIndices[i*2+1]);
		m_vertices[sIndices[i*2]]->m_edges.push_back(m_edges[i]);
		m_vertices[sIndices[i*2+1]]->m_edges.push_back(m_edges[i]);
		m_edges[i]->m_start = m_vertices[sIndices[i*2]];
		m_edges[i]->m_end = m_vertices[sIndices[i*2+1]];
	}

	// loop over triangles	*/
	LONG tCount = geom.GetTriangleCount();
	CLongArray tIndices;
	geom.GetTrianglePointIndices(tIndices);
	CVector3f a, b;
	CVector3f p;

	m_triangles.resize(tCount);

	for (LONG i=0; i<tCount; i++)
	{
		m_triangles[i] = new TXTriangle(i,tIndices[i*3],tIndices[i*3+1],tIndices[i*3+2]);

		// Get Normal
		a.Sub(m_vertices[tIndices[i*3]]->m_pos,m_vertices[tIndices[i*3+1]]->m_pos);
		b.Sub(m_vertices[tIndices[i*3]]->m_pos,m_vertices[tIndices[i*3+2]]->m_pos);
		m_triangles[i]->m_norm.Cross(a,b);
		m_triangles[i]->m_norm.NormalizeInPlace();

		for(LONG j=0;j<3;j++)
		{
			// find connected triangles
			m_vertices[tIndices[i*3+j]]->m_triangles.push_back(m_triangles[i]);
			m_triangles[i]->m_v.push_back(m_vertices[tIndices[i*3+j]]);
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
			pe = m_vertices[pid[j]]->m_edges;
			for(LONG k=0;k<(LONG)pe.size();k++)
			{
				polygon->PushEdgeData(pe[k]);
			}
		}
		m_polygons.push_back(polygon);
	}

	int found;
	//reloop over all edges getting adjacents + edge type
	//also get neighboring edges
	for (ULONG i=0; i<m_nbe; i++)
	{
		found = 0;
		TXEdge* e = m_edges[i];
		//loop all triangle connected to first point
		std::vector<TXTriangle*> et = m_vertices[e->m_pid[0]]->m_triangles;
		for(LONG j= 0;j<(LONG)et.size();j++)
		{
			for(LONG k =0;k<3;k++)
			{
				if(et[j]->m_pid[k] == e->m_pid[1])
				{
					e->m_triangles.push_back(et[j]);
					found++;
				}
			}
		}

		if(found<2)
		{
			e->m_isboundary = true;
			m_boundaries.push_back(e);
			m_vertices[e->m_pid[0]]->m_isboundary = true;
			m_vertices[e->m_pid[1]]->m_isboundary = true;
		}
		else 
		{
			e->m_isboundary = false;
			m_vertices[e->m_pid[0]]->m_isboundary = false;
			m_vertices[e->m_pid[1]]->m_isboundary = false;
		}
		
		e->GetNeighborEdges();
		e->GetAdjacentEdges();
		e->GetNextEdges();

	}

	// init mesh data flag
	m_init = true;
	m_dirty = true;

	// set sub geometry data
	if(m_subgeometries.size()>0)
	{
		std::vector<TXGeometry*>::iterator it = m_subgeometries.begin();
		for(;it<m_subgeometries.end();it++)
		{
			//Vertex Datas
			(*it)->m_vertices.resize((*it)->m_nbv);
			for(ULONG id= (*it)->m_basevertexID;id<(*it)->m_basevertexID+(*it)->m_nbv;id++)(*it)->m_vertices[id-(*it)->m_basevertexID] = m_vertices[id];

			//Edge Datas
			(*it)->m_edges.resize((*it)->m_nbe);
			for(ULONG id= (*it)->m_baseedgeID;id<(*it)->m_baseedgeID+(*it)->m_nbe;id++)(*it)->m_edges[id-(*it)->m_baseedgeID]  = m_edges[id];

			//Triangle Datas
			(*it)->m_triangles.resize((*it)->m_nbt);
			for(ULONG id= (*it)->m_basetriangleID;id<(*it)->m_basetriangleID+(*it)->m_nbt;id++)(*it)->m_triangles[id-(*it)->m_basetriangleID] = m_triangles[id];
		}
	}

	//Set visibility
	SetAllVisible();
}

void TXGeometry::GetPointPosition(CICEGeometry& geom)
{
	CDoubleArray points;
    geom.GetPointPositions( points ) ;
	LONG pCount = geom.GetPointPositionCount();

	m_min = CVector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	m_max = CVector3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	CVector3f pv;
	bool clear = false;
	if(m_nbv != pCount)clear = true;

	if(clear)
	{
		// Clean old data
		for(ULONG v=0;v<m_vertices.size();v++)delete m_vertices[v];

		m_nbv = pCount;
		m_vertices.resize(m_nbv);
	}

	//only ONE geometrie
	if(m_isleaf)
	{
		// loop over vertices
		for (ULONG i=0; i<m_nbv; i++)
		{	
			if(clear)
				m_vertices[i] = new TXVertex(i,(float)points[i*3],(float)points[i*3+1],(float)points[i*3+2]);
			else
				m_vertices[i]->m_pos.Set((float)points[i*3],(float)points[i*3+1],(float)points[i*3+2]);

			pv = m_vertices[i]->m_pos;
		
			m_min.Set(MIN(m_min.GetX(),pv.GetX()),MIN(m_min.GetY(),pv.GetY()),MIN(m_min.GetZ(),pv.GetZ()));
			m_max.Set(MAX(m_max.GetX(),pv.GetX()),MAX(m_max.GetY(),pv.GetY()),MAX(m_max.GetZ(),pv.GetZ()));
		}

		//Calculate Bounding Sphere
		m_spherecenter.LinearlyInterpolate(m_min,m_max,0.5);
		CVector3f diagonal;
		diagonal.Sub(m_max,m_min);
		m_sphereradius = (float)(diagonal.GetLength()*0.5);
	}

	//Multiple Sub Geometries
	else
	{
		std::vector<TXGeometry*>::iterator sgit = m_subgeometries.begin();
		ULONG i=0;
		for(;sgit<m_subgeometries.end();sgit++)
		{
			(*sgit)->m_min.Set(FLT_MAX, FLT_MAX, FLT_MAX);
			(*sgit)->m_max.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);

			for(ULONG v=0;v<(*sgit)->m_nbv;v++)
			{
				if(clear)
					m_vertices[i] = new TXVertex(i,(float)points[i*3],(float)points[i*3+1],(float)points[i*3+2]);
				else
					m_vertices[i]->m_pos.Set((float)points[i*3],(float)points[i*3+1],(float)points[i*3+2]);

				pv = m_vertices[i]->m_pos;
			
				(*sgit)->m_min.Set(
					MIN((*sgit)->m_min.GetX(),pv.GetX()),
					MIN((*sgit)->m_min.GetY(),pv.GetY()),
					MIN((*sgit)->m_min.GetZ(),pv.GetZ())
				);

				(*sgit)->m_max.Set(
					MAX((*sgit)->m_max.GetX(),pv.GetX()),
					MAX((*sgit)->m_max.GetY(),pv.GetY()),
					MAX((*sgit)->m_max.GetZ(),pv.GetZ())
				);

				m_min.Set(
					MIN(m_min.GetX(),pv.GetX()),
					MIN(m_min.GetY(),pv.GetY()),
					MIN(m_min.GetZ(),pv.GetZ())
				);

				m_max.Set(
					MAX(m_max.GetX(),pv.GetX()),
					MAX(m_max.GetY(),pv.GetY()),
					MAX(m_max.GetZ(),pv.GetZ())
				);
				i++;
			}

			//Calculate Bounding Sphere
			(*sgit)->m_spherecenter.LinearlyInterpolate((*sgit)->m_min,(*sgit)->m_max,0.5);
			CVector3f radii;
			radii.Sub((*sgit)->m_max,(*sgit)->m_spherecenter);
			(*sgit)->m_sphereradius = radii.GetLength();
		}
	}
}

void TXGeometry::GetRuntimeMeshData(CICEGeometry& geom)
{
	// Query Dirty States to know what to update
	GetDirtyState( geom);

	// Get Triangles Normals
	LONG tCount = geom.GetTriangleCount();
	CLongArray tIndices;
	geom.GetTrianglePointIndices(tIndices);
	CVector3f a, b, c;
	CVector3f p;

	for (LONG i=0; i<tCount; i++)
	{
		a.Sub(m_vertices[tIndices[i*3]]->m_pos,m_vertices[tIndices[i*3+1]]->m_pos);
		b.Sub(m_vertices[tIndices[i*3]]->m_pos,m_vertices[tIndices[i*3+2]]->m_pos);
		m_triangles[i]->m_norm.Cross(a,b);
		m_triangles[i]->m_norm.NormalizeInPlace();
		c.Sub(m_view,m_vertices[tIndices[i*3]]->m_pos);
		m_triangles[i]->m_facing = (c.Dot(m_triangles[i]->m_norm)>0);
	}

	// Get Vertex Normals
	std::vector<TXTriangle*> t;
	CVector3f n;
	LONG nbt;
	std::vector<TXVertex*>::iterator vv = m_visiblevertices.begin();
	for(;vv != m_visiblevertices.end();vv++)
	{
		n.SetNull();
		t = (*vv)->m_triangles;
		nbt = (long)t.size();
		for(LONG j=0;j<nbt;j++)
		{
			n += t[j]->m_norm;
		}
		n.NormalizeInPlace();
		(*vv)->m_norm = n;
		(*vv)->IsCrease();
	}

	//BuildKDTree();
}

void TXGeometry::GetCreaseEdges(float crease)
{
	m_creases.clear();
	TXEdge* ed = NULL;
	//get crease edges
	for(ULONG e=0;e<m_visibleedges.size();e++)
	{
		ed = m_visibleedges[e];
		if(!ed->m_isboundary)
		{
			float angle = ed->m_triangles[0]->m_norm.GetAngle(ed->m_triangles[1]->m_norm);

			if(abs(angle)>crease)
			{
				ed->m_iscrease = true;
				m_creases.push_back(ed);
			}
			else
				ed->m_iscrease = false;
		}
	}
	m_creasecached = true;
}

void TXGeometry::GetIntersectionEdges()
{
	m_intersections.clear();
	ULONG nbSubGeometries = m_subgeometries.size();
	ULONG current = 0;

	// Intersections works ONLY with Group Geometry
	if(nbSubGeometries>0)
	{
		TXGeometry* geo1;
		TXGeometry* geo2;

		std::vector<TXTriangle*>::iterator t;
		std::vector<TXEdge*>::iterator e;
		bool intersect;
		while(current<nbSubGeometries)
		{
			geo1 = m_subgeometries[current];
			for(ULONG i=0;i<nbSubGeometries;i++)
			{
				geo2 = m_subgeometries[i];
				if(geo1->m_id == geo2->m_id)continue;
				
				// We NEED to check first for Bounding Box Collision
				for(e = geo1->m_edges.begin();e<geo1->m_edges.end();e++)
				{
					intersect = false;
					
					for(t = geo2->m_triangles.begin();t<geo2->m_triangles.end();t++)
					{
						if((*e)->IntersectWithTriangle((*t)))
						{
							m_intersections.push_back(*e);

							intersect = true;
							//Application().LogMessage(L"Edge ID "+(CString)(*e)->_id+L" is Intersection Edge"+(CString)(*e)->_isintersection);
							break;
						}
					}
					(*e)->m_isintersection = intersect;
				}
			}
			current++;
		}

	}
	//Application().LogMessage(L"Get Intersections called...");
}

void TXGeometry::GetDotAndSign(CVector3f& view, float bias)
{
	CVector3f c;
	// loop over all vertices getting dot and sign
	for(ULONG i=0;i<m_nbvv;i++)
	{
		c.Sub(view,m_visiblevertices[i]->m_pos);
		c.NormalizeInPlace();
		m_visiblevertices[i]->m_dot = c.Dot(m_visiblevertices[i]->m_norm) - bias;
		if(m_visiblevertices[i]->m_dot>=0)m_visiblevertices[i]->m_sign = 1;
		else m_visiblevertices[i]->m_sign = -1;
	}
}

void TXGeometry::GetVisible(TXCamera* camera)
{
	ClearVisible();

	bool multi = false;
	if(m_subgeometries.size()>0)multi = true;

	if(!multi)
	{
		m_infrustrum = camera->GeometryInFrustrum(this);
	
		//Intersect
		if(m_infrustrum == 1)
		{
			for(ULONG p=0;p<m_nbv;p++)
			{
				if(camera->PointInFrustrum(m_vertices[p]->m_pos))
				{
					m_visiblevertices.push_back(m_vertices[p]);
					m_vertices[p]->SetVisible(true);
					m_nbvv++;
				}
			}
		}
		else if(m_infrustrum == 2)
		{
			SetAllVisible();
		}
	}

	else
	{
		std::vector<TXGeometry*>::iterator git=m_subgeometries.begin();
		ULONG vid = 0;
		for(;git<m_subgeometries.end();git++)
		{
			(*git)->m_infrustrum = camera->GeometryInFrustrum(*git);
			if((*git)->m_infrustrum)
			{
				//Inside Frustrum : All Points Visible
				if((*git)->m_infrustrum == INSIDE)
				{
					for(ULONG i=0;i<(*git)->m_nbv;i++)
					{
						m_visiblevertices.push_back(m_vertices[vid]);
						m_vertices[vid]->SetVisible(true);
						m_nbvv++;
						vid++;
					}
				}
				//Intersect : Parse Points
				// Idealy we would have one Octree per SubGeometries for even faster parsing
				else
				{
					for(ULONG i=0;i<(*git)->m_nbv;i++)
					{
						if(camera->PointInFrustrum(m_vertices[vid]->m_pos))
						{
							m_visiblevertices.push_back(m_vertices[vid]);
							m_vertices[vid]->SetVisible(true);
							m_nbvv++;
						}
						vid++;
					}
				}
			}
			// Ouside Frustrum : Discard All Points
			else
			{
				vid += (*git)->m_nbv;
			}
		}
	}
	
	CLongArray change;
	//loop over all edges
	std::vector<TXEdge*>::iterator ei;
	for(ei=m_edges.begin();ei<m_edges.end();ei++)
	{
		if((*ei)->m_start->m_visible || (*ei)->m_end->m_visible)
		{
			if(!(*ei)->m_start->m_visible) 
				change.Add((*ei)->m_start->m_id);
			if(!(*ei)->m_end->m_visible) 
				change.Add((*ei)->m_end->m_id);

			m_visibleedges.push_back(*ei);
			(*ei)->m_isvisible = true;
			m_nbve++;
		}
	}

	//Enable vertices connected to a visible edge
	for(LONG e=0;e<change.GetCount();e++)
	{
		m_vertices[change[e]]->m_visible = true;
		m_visiblevertices.push_back(m_vertices[change[e]]);
		m_nbvv++;
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
	m_visibleedges.clear();
	m_visiblevertices.clear();

	m_nbvv = 0;
	m_nbve = 0;

	std::vector<TXVertex*>::iterator vi = m_vertices.begin();
	for(;vi<m_vertices.end();vi++)
		(*vi)->SetVisible(false);

	std::vector<TXEdge*>::iterator ei = m_edges.begin();
	for(;ei<m_edges.end();ei++)
		(*ei)->m_isvisible = false;
}

void TXGeometry::SetAllVisible()
{
	m_visiblevertices.clear();
	m_visibleedges.clear();
	m_nbvv = m_nbv;
	m_nbve = m_nbe;

	std::vector<TXVertex*>::iterator vi = m_vertices.begin();
	for(;vi<m_vertices.end();vi++)
	{
		m_visiblevertices.push_back(*vi);
		(*vi)->SetVisible(true);
	}

	std::vector<TXEdge*>::iterator ei = m_edges.begin();
	for(;ei<m_edges.end();ei++)
	{
		m_visibleedges.push_back(*ei);
		(*ei)->m_isvisible = true;
	}
}

bool TXGeometry::IsSilhouette(TXEdge* e,CVector3f view, bool smooth)
{
	CVector3f c;

	if(smooth  || e->m_isboundary)
	{
		if((e->m_start->m_sign + e->m_end->m_sign)== 0)
		{
			e->m_issilhouette = true;
			if(e->m_start->m_iscrease && !e->m_end->m_iscrease)
				e->m_weight = 1;

			else if(!e->m_start->m_iscrease && e->m_end->m_iscrease)
				e->m_weight = 0;

			else if(e->m_start->m_iscrease && e->m_end->m_iscrease)
			{
				float d1 = e->m_start->m_norm.Dot(view);
				float d2 = e->m_end->m_norm.Dot(view);
				if(d1>d2)e->m_weight = 0;
				else e->m_weight = 1;
			}
			else
				e->m_weight = ABS(e->m_end->m_dot)/(ABS(e->m_start->m_dot)+ABS(e->m_end->m_dot));
			return true;
		}
	}
	else
	{
		float dot1, dot2;

		c.LinearlyInterpolate(e->m_start->m_pos,e->m_end->m_pos,0.5);
		c.Sub(view,c);
		c.NormalizeInPlace();
		dot1 = c.Dot(e->m_triangles[0]->m_norm);
		dot2 = c.Dot(e->m_triangles[1]->m_norm);
		e->m_weight = 0.5;

		if(dot1*dot2<0/*||(_geom->_vertices[p1id]->_sign + _geom->_vertices[p2id]->_sign)== 0*/)
		{
			e->m_weight = ABS(e->m_end->m_dot)/(ABS(e->m_start->m_dot)+ABS(e->m_end->m_dot));
			e->m_issilhouette = true;
			return true;
		}
	}

	return false;
}

TXVertex::TXVertex(LONG id,float px, float py, float pz)
{
	m_id = id;
	m_pos.Set(px,py,pz);
}

TXVertex::~TXVertex()
{
}

void TXVertex::IsCrease()
{
	std::vector<TXEdge*>::iterator it = m_edges.begin();
	float dot;
	for(;it!=m_edges.end();it++)
	{
		dot = (*it)->GetDot(this);
		if(abs(dot)>0.5)
		{	
			m_iscrease = true;
			return;
		}
	}
	m_iscrease = false;
}

bool TXVertex::IsInCell(const CVector3f& min, const CVector3f& max)
{
	if(m_pos.GetX()>=min.GetX() && m_pos.GetX()<max.GetX()&&
		m_pos.GetY()>=min.GetY() && m_pos.GetY()<max.GetY()&&
		m_pos.GetZ()>=min.GetZ() && m_pos.GetZ()<max.GetZ())return true;
	return false;
}

TXEdge::TXEdge(LONG id,LONG p1, LONG p2)
{
	m_id = id;
	m_pid.Resize(2);
	m_pid[0] = p1;
	m_pid[1] = p2;
	m_issilhouette = false;
	m_iscrease = false;
	m_isboundary = false;
	m_ischecked = false;
	m_weight = -1;
}

TXEdge::~TXEdge()
{
}

void TXEdge::GetNeighborEdges()
{
	std::vector<TXPolygon*>::iterator pi;
	std::vector<TXEdge*> e;
	std::vector<TXEdge*>::iterator ei;

	for(pi=m_polygons.begin();pi<m_polygons.end();pi++)
	{
		e = (*pi)->m_edges;
		for(ei=e.begin();ei<e.end();ei++)
		{
			if((*ei)->m_id != m_id)
			{
				m_neighbors.push_back(*ei);
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
	for(ip= m_polygons.begin();ip<m_polygons.end();ip++)
	{
		e = (*ip)->m_edges;
		for(ie=e.begin();ie<e.end();ie++)
		{
			adjacents.Add((*ie)->m_id);
		}
	}

	//Get Next Edges
	std::vector<TXEdge*> n = m_end->m_edges;
	bool isAdjacent = false;
	float angle = 0.0;
	CVector3f a, b;

	TXEdge* next = NULL;

	// Store only ONE non-adjacent edge with maximal angle
	for(std::vector<TXEdge*>::iterator ie=n.begin();ie<n.end();ie++)
	{
		if((*ie)->m_id == m_id)continue;

		isAdjacent = false;
		for(LONG ia=0;ia<adjacents.GetCount();ia++)
		{
			if((*ie)->m_id == adjacents[ia])
			{
				isAdjacent = true;
				break;
			}
		}

		if(!isAdjacent)
		{
			a.Sub(m_start->m_pos,m_end->m_pos);
			if((*ie)->m_pid[0] == m_pid[1])
				b.Sub((*ie)->m_end->m_pos,(*ie)->m_start->m_pos);
			else
				b.Sub((*ie)->m_start->m_pos,(*ie)->m_end->m_pos);

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
		m_next.push_back(next);
	}

	//Get Previous Edges
	n = m_start->m_edges;
	angle = 0.0;
	next = NULL;
	
	for(std::vector<TXEdge*>::iterator ip = n.begin();ip<n.end();ip++)
	{
		isAdjacent = false;
		if((*ip)->m_id == m_id)continue;

		for(LONG ia=0;ia<adjacents.GetCount();ia++)
		{
			if((*ip)->m_id == adjacents[ia])
			{
				isAdjacent = true;
			}
		}
		
		if(!isAdjacent)
		{
			a.Sub(m_end->m_pos,m_start->m_pos);
			if((*ip)->m_pid[0] == m_pid[0])
				b.Sub((*ip)->m_end->m_pos,(*ip)->m_start->m_pos);
			else
				b.Sub((*ip)->m_start->m_pos,(*ip)->m_end->m_pos);

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
		m_next.push_back(next);
	}
}

void TXEdge::GetAdjacentEdges()
{
	std::vector<TXEdge*> e;
	std::vector<TXEdge*>::iterator ei;
	float angle;

	//search for edges adjacent to first vertex
	e = m_start->m_edges;
	for(ei=e.begin();ei<e.end();ei++)
	{
		if((*ei)->m_id != m_id)
		{
			m_adjacents.push_back(*ei);
			angle = GetAngle(*ei);
			m_angles.Add(angle);
		}
	}

	//search for edges adjacent to second vertex
	e = m_end->m_edges;
	for(ei=e.begin();ei<e.end();ei++)
	{
		if((*ei)->m_id != m_id)
		{
			m_adjacents.push_back(*ei);
			angle = GetAngle(*ei);
			m_angles.Add(angle);
		}
	}
}

float TXEdge::GetAngle(TXEdge* other)
{
	ULONG p1,p2,p3,p4;
	CVector3f v1, v2;

	p1 = m_pid[0];
	p2 = m_pid[1];
	p3 = other->m_pid[0];
	p4 = other->m_pid[1];

	v1.Sub(m_start->m_pos,m_end->m_pos);
	v2.Sub(other->m_start->m_pos,other->m_end->m_pos);

	if(p1 == p4 )v1.NegateInPlace();
	else if(p4 == p1)v2.NegateInPlace();

	float angle = v1.GetAngle(v2);
	return angle;
}

float TXEdge::GetAngle(TXVertex* v)
{
	CVector3f dir;
	if(v->m_id == m_start->m_id)
		dir.Sub(m_end->m_pos,v->m_pos);
	else
		dir.Sub(m_start->m_pos,v->m_pos);
	float angle = v->m_norm.GetAngle(dir);
	return angle;
}

float TXEdge::GetDot(TXVertex* v)
{
	CVector3f dir;
	if(v->m_id == m_start->m_id)
		dir.Sub(m_end->m_pos,v->m_pos);
	else
		dir.Sub(m_start->m_pos,v->m_pos);
	float dot = v->m_norm.Dot(dir);
	return dot;
}



ULONG TXEdge::GetIDInTriangle(TXTriangle* t)
{
	for(ULONG i=0;i<3;i++)
	{
		if((t->m_pid[i] == m_pid[0] && t->m_pid[(i+1)%3] == m_pid[1])||
			(t->m_pid[i] == m_pid[1] && t->m_pid[(i+1)%3] == m_pid[0]))return i;
	}
	return 0;
}


bool TXEdge::ShareTriangle(TXEdge* other)
{
	std::vector<TXTriangle*> t = m_triangles;

	for(LONG z=0;z<(LONG)t.size();z++)
	{
		if(other->IsInTriangle(t[z]))return true;
	}

	return false;
}

bool TXEdge::IsInTriangle(TXTriangle* t)
{
	LONG cnt = 0;
	for(LONG y=0;y<3;y++)
	{
		if(t->m_pid[y] == m_pid[0] || t->m_pid[y] == m_pid[1])cnt++;
	}
	if(cnt == 2)return true;
	return false;
}

bool TXEdge::IsConnectedToTriangle(TXTriangle* t)
{
	for(LONG y=0;y<3;y++)
	{
		if(t->m_pid[y] == m_pid[0] || t->m_pid[y] == m_pid[1])return true;
	}
	return false;
}

LONG TXEdge::SharePoint(TXEdge*& other)
{
	if(m_pid[0] == other->m_pid[0] || m_pid[0] == other->m_pid[1]) return 1;
	else if(m_pid[1] == other->m_pid[0] || m_pid[1] == other->m_pid[1]) return 2;
	else return 0;
}

bool TXEdge::Check(LONG lineid)
{
	if(m_ischecked)return false;

	// intersection
	if(lineid == INTERSECTION)return m_isintersection;

	//cluster
	else if(lineid == CLUSTER)return m_isclusteredge;

	//boundary
	else if(lineid == BOUNDARY)return m_isboundary;

	//crease
	else if(lineid == CREASE)return m_iscrease;

	//silhouettes
	else return m_issilhouette;
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
	if(m_start->IsInCell(minp, maxp))return true;
	if(m_end->IsInCell(minp,maxp))return true;
	TXBBox bbox(minp,maxp);
	return IntersectWithBBox(&bbox);

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

bool TXEdge::IntersectWithBBox(TXBBox* in_bbox)
{
	Application().LogMessage(L"Edge Intersect With BBox Called...");
	return false;
}

bool TXEdge::IntersectWithTriangle(TXTriangle* in_triangle)
{
	//Check Intersection with the plane of the triangle
	CVector3f p;
	CVector3f intersection;

	p.Sub(m_start->m_pos,in_triangle->m_v[0]->m_pos);
	float distance1 = p.Dot(in_triangle->m_norm);
	p.Sub(m_end->m_pos,in_triangle->m_v[0]->m_pos);
	float distance2 = p.Dot(in_triangle->m_norm);
	if((distance1 > 0 && distance2 < 0) || (distance1 < 0 && distance2 > 0))
	{
		m_weight = 1-(fabs(distance1) / (fabs(distance1) + fabs(distance2)));
		//Application().LogMessage(L" Weight : "+(CString)_weight);
		intersection.LinearlyInterpolate(m_start->m_pos, m_end->m_pos, m_weight);
	}
	else return false;

	// Get UV from Point
	TXUVCoord uv = in_triangle->GetUVFromPoint(intersection);
	//Application().LogMessage(L"UV : "+(CString)uv._u+L","+(CString)uv._v);
   if(!uv.IsOnTriangle())
	  return false;
   return true;
}


TXTriangle::TXTriangle(LONG id,LONG p1, LONG p2, LONG p3)
{
	m_id = id;
	m_pid.Resize(3);
	m_pid[0] = p1;
	m_pid[1] = p2;
	m_pid[2] = p3;
}

bool TXTriangle::IsVisible()
{
	for(int i=0;i<3;i++)
	{
		if((*m_v[i]).IsVisible())return true;
	}
	return false;
}

TXUVCoord TXTriangle::GetUVFromPoint(const CVector3f & in_pos)
{
   // Compute vectors
   CVector3f v0,v1,v2;
   v0.Sub(m_v[2]->m_pos,m_v[0]->m_pos);
   v1.Sub(m_v[1]->m_pos,m_v[0]->m_pos);
   v2.Sub(in_pos,m_v[0]->m_pos);

   // Compute dot products
   float dot00 = v0.Dot(v0);
   float dot01 = v0.Dot(v1);
   float dot02 = v0.Dot(v2);
   float dot11 = v1.Dot(v1);
   float dot12 = v1.Dot(v2);

   // Compute barycentric coordinates
   float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);

   // pack into struct
   TXUVCoord uv;
   uv.m_u = (dot11 * dot02 - dot01 * dot12) * invDenom;
   uv.m_v = (dot00 * dot12 - dot01 * dot02) * invDenom;
   return uv;
}

TXPolygon::TXPolygon(LONG id, XSI::CLongArray pid)
{
	m_id = id;
	m_pid = pid;
}

TXPolygon::~TXPolygon()
{
}

void TXPolygon::PushEdgeData(TXEdge*& edge)
{
	// if edge already in _edges return
	for(LONG v=0;v<(LONG)m_edges.size();v++)
	{
		if(m_edges[v]->m_id == edge->m_id)return;
	}

	// check if edge belong to polygon
	LONG cnt = 0;
	for(LONG v=0;v<(LONG)m_pid.GetCount();v++)
	{
		if(m_pid[v] == edge->m_pid[0]|| m_pid[v] == edge->m_pid[1])cnt++;
	}
	if(cnt == 2)
	{
		m_edges.push_back(edge);
		edge->m_polygons.push_back(this);
	}
}

