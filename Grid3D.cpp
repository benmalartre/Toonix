// Toonix Grid3D
//----------------------------------------------

#include "Register.h"
#include "Grid3D.h"

TXGridTriangle::TXGridTriangle(TXTriangle *tp)
{
	m_t = tp;
	m_n = m_t->m_norm;

	CVector3f tmp;
	tmp.Sub(m_t->m_v[0]->m_pos,m_t->m_v[1]->m_pos);
	tmp.Cross(tmp,m_n);
	m_en1.Normalize(tmp);
	tmp.Sub(m_t->m_v[1]->m_pos,m_t->m_v[2]->m_pos);
	tmp.Cross(tmp,m_n);
	m_en2.Normalize(tmp);
	tmp.Sub(m_t->m_v[2]->m_pos,m_t->m_v[0]->m_pos);
	tmp.Cross(tmp,m_n);
	m_en3.Normalize(tmp);
}

// place a triangle mesh in the grid
void TXGrid3D::PlaceIntoGrid(TXGeometry *geom) 
{
	m_min = CVector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	m_max = CVector3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// compute bounding box
	int totalTriangles = 0;

	TXTriangle* tp;
	CVector3f p;

	for(ULONG t=0;t<geom->m_triangles.size();t++)
	{
		tp = geom->m_triangles[t];
		totalTriangles++;
		for(int i=0;i<3;i++)
		{
			p = tp->m_v[i]->m_pos;
			m_min.Set(
				MIN(m_min.GetX(),p.GetX()),
				MIN(m_min.GetY(),p.GetY()),
				MIN(m_min.GetZ(),p.GetZ())
			);
			m_max.Set(MAX(m_max.GetX(),p.GetX()),
				MAX(m_max.GetY(),p.GetY()),
				MAX(m_max.GetZ(),p.GetZ())
			);
		}
	}

	Application().LogMessage(L"Total Triangles : "+(CString)totalTriangles);

	// select grid size
	int grid_size;
	if (totalTriangles <= 10000) grid_size = 10;
	else if (totalTriangles <= 40000) grid_size = 15;
	else grid_size = 20;

	// create grid
	CreateGrid(grid_size);

	m_xstep = (m_max.GetX() - m_min.GetX()) / m_size;
	m_ystep = (m_max.GetY() - m_min.GetY()) / m_size;
	m_zstep = (m_max.GetZ() - m_min.GetZ()) / m_size;

	for(ULONG t=0;t<geom->m_triangles.size();t++)
	{
		PlaceTriangle(geom->m_triangles[t]);
	}
	 
}

// allocate proper space for a grid of size s, free old
void TXGrid3D::CreateGrid(int s) 
{
	int i, j, k;
	// clear old space
	if (m_grid && (m_size > 0)) 
	{
		for (i=0; i<m_size; i++)
			for (j=0; j<m_size; j++)
				for (k=0; k<m_size; k++)
					m_grid[i][j][k].clear();
	}

	// if size changed, free old and allocate new
	if (s != m_size) 
	{
		if (m_grid) 
		{
			for (i=0; i<m_size; i++)
				for (j=0; j<m_size; j++)
					delete[] m_grid[i][j];
				for (i=0; i<m_size; i++)
					delete[] m_grid[i];
				delete[] m_grid;
		}
		m_size = s;
		m_grid = new std::vector<TXGridTriangle*>**[s];
		for (i=0; i<s; i++)
		  m_grid[i] = new std::vector<TXGridTriangle*>*[s];
		for (i=0; i<s; i++)
		  for (j=0; j<s; j++)
		m_grid[i][j] = new std::vector<TXGridTriangle*>[s];
	}
}

// free space
TXGrid3D::~TXGrid3D() 
{
	int i, j, k;
	if (m_grid && (m_size > 0)) 
	{
		for (i=0; i<m_size; i++) 
		{
			for (j=0; j<m_size; j++)
			{
				for (k=0; k<m_size; k++) 
				{
				  m_grid[i][j][k].clear();
				}
				delete[] m_grid[i][j];
			}
			delete[] m_grid[i];
		}
		delete[] m_grid;
	}
	m_grid = 0;
	m_rays.clear();
}

// place a triangle into intesected cells
void TXGrid3D::PlaceTriangle(TXTriangle *t)
{
	// bounding box
	CVector3f tmin(FLT_MAX, FLT_MAX, FLT_MAX), tmax(-FLT_MAX, -DBL_MAX, -FLT_MAX);
	for (int v=0; v<3; v++)
	{
		tmin.Set(
			MIN(tmin.GetX(),t->m_v[v]->m_pos.GetX()),
			MIN(tmin.GetY(),t->m_v[v]->m_pos.GetY()),
			MIN(tmin.GetZ(),t->m_v[v]->m_pos.GetZ())
		);

		
		tmax.Set(
			MAX(tmax.GetX(),t->m_v[v]->m_pos.GetX()),
			MAX(tmax.GetY(),t->m_v[v]->m_pos.GetY()),
			MAX(tmax.GetZ(),t->m_v[v]->m_pos.GetZ())
		);
	}

	// starting and ending cells of the bounding box
	CVector3 start(
		MAX((int)floor((tmin.GetX() - m_min.GetX())/m_xstep), 0),
		MAX((int)floor((tmin.GetY() - m_min.GetY())/m_ystep), 0),
		MAX((int)floor((tmin.GetZ() - m_min.GetZ())/m_zstep), 0)
	);

	CVector3 end(
		MIN((int)floor((tmax.GetX() - m_min.GetX())/m_xstep), m_size-1),
		MIN((int)floor((tmax.GetY() - m_min.GetY())/m_ystep), m_size-1),
		MIN((int)floor((tmax.GetZ() - m_min.GetZ())/m_zstep), m_size-1)
	);

	TXGridTriangle* gt = new TXGridTriangle(t);

	bool sa = (gt->m_n.GetX() >= 0), sb = (gt->m_n.GetY() >= 0), sc = (gt->m_n.GetZ() >= 0);
	CVector3f p1, p2;
	bool sd1, sd2;

	// put triangle in the cells in its bounding box
	// still excessive, treat triangle as a plane to detect intersection
	for (int i=(int)start.GetX(); i<=(int)end.GetX(); i++) 
	{
		p1.Set(m_min.GetX() + i*m_xstep,p1.GetY(),p2.GetZ());
		p2.Set(p1.GetX() + m_xstep,p2.GetY(),p2.GetZ());

		for (int j=(int)start.GetY(); j<=(int)end.GetY(); j++) 
		{
			if (sb == sa) 
			{
				p1.Set(p1.GetX(),m_min.GetY() + j*m_ystep,p1.GetZ());
				p2.Set(p2.GetX(),p1.GetY()+ m_ystep,p2.GetZ()); 
			}
			else
			{
				p2.Set(p2.GetX(), m_min.GetY() + j*m_ystep,p2.GetZ());
				p1.Set(p1.GetX(),p2.GetY() + m_ystep,p1.GetZ());
			}
			for (int k=start.GetZ(); k<=end.GetZ(); k++) 
			{
				if (sc == sa)
				{
					p1.Set(p1.GetX(),p1.GetY(),m_min.GetZ() + k*m_zstep);
					p2.Set(p2.GetX(),p2.GetY(),p1.GetZ() + m_zstep);
				}
				else
				{
					p2.Set(p2.GetX(),p2.GetY(),m_min.GetZ() + k*m_zstep);
					p1.Set(p1.GetX(),p1.GetY(),p2.GetZ() + m_zstep);
				}
			
				CVector3f p1gt, p2gt;
				p1gt.Sub(p1,gt->m_t->m_v[0]->m_pos);
				p2gt.Sub(p2,gt->m_t->m_v[0]->m_pos);
				sd1 = gt->m_n.Dot(p1gt) >= 0;
				sd2 = gt->m_n.Dot(p2gt) >= 0;
				if (sd1 != sd2) 
					m_grid[i][j][k].push_back(gt);
			}
		}
	}
}

// intersect a ray with the mesh
bool TXGrid3D::IntersectRay(const CVector3f& start, const CVector3f& end) 
{
	// pick rays
	m_rays.push_back(TXRay(start, end));
	CVector3f dir;
	dir.Sub(start,end);

	int idir[3] = {
	fabs(dir.GetX()) < DBL_EPSILON ? 0 : (dir.GetX() > 0 ? 1 : -1),
	fabs(dir.GetY()) < DBL_EPSILON ? 0 : (dir.GetY() > 0 ? 1 : -1),
	fabs(dir.GetZ()) < DBL_EPSILON ? 0 : (dir.GetZ() > 0 ? 1 : -1) };

	double dist = 0;
	CVector3f pos;
	pos.Sub(end, m_min);

	int ix = (int)floor((end.GetX() - m_min.GetX())/m_xstep),
	iy = (int)floor((end.GetY() - m_min.GetY())/m_ystep),
	iz = (int)floor((end.GetZ() - m_min.GetZ())/m_zstep);
	double tx = 1.0, ty = 1.0, tz = 1.0;

	// first cell
	ix = MIN(MAX(ix, 0), m_size-1);
	iy = MIN(MAX(iy, 0), m_size-1);
	iz = MIN(MAX(iz, 0), m_size-1);

	// intersection test, from end to start
	while ((dist < 1) && (ix >= 0) && (ix < m_size) &&(iy >= 0) && (iy < m_size) && (iz >= 0) && (iz < m_size)) 
	{
		int csz = m_grid[ix][iy][iz].size();
		for (int i=0; i<csz; i++) 
		{
			TXGridTriangle* gt = m_grid[ix][iy][iz][i];
			double rpdot = dir.Dot(gt->m_n);
			if (rpdot != 0) 
			{
				CVector3f tmp;
				tmp.Sub(gt->m_t->m_v[0]->m_pos,end);
				double t = tmp.Dot(gt->m_n) / rpdot;

				if (t > DBL_EPSILON && t < 1) 
				{
					CVector3f pt;
					pt.ScaleAdd(t,dir,end);

					CVector3f pt0,pt1,pt2;
					pt0.Sub(pt,gt->m_t->m_v[0]->m_pos);
					pt1.Sub(pt,gt->m_t->m_v[1]->m_pos);
					pt2.Sub(pt,gt->m_t->m_v[2]->m_pos);


					if (pt0.Dot(gt->m_en1)> -DBL_EPSILON &&
					  pt1.Dot(gt->m_en2)>-DBL_EPSILON &&
					  pt2.Dot(gt->m_en3)>-DBL_EPSILON) 
					{
						m_rays[m_rays.size()-1].m_p = t;
						return true;
					}
				}
			}
		}

		// next cell
		if (idir[0] != 0)
		  tx = (m_min.GetX() + (ix+(idir[0]+1)/2)*m_xstep - end.GetX()) / dir.GetX();
		if (idir[1] != 0)
		  ty = (m_min.GetY() + (iy+(idir[1]+1)/2)*m_ystep - end.GetY()) / dir.GetY();
		if (idir[2] != 0)
		  tz = (m_min.GetZ() + (iz+(idir[2]+1)/2)*m_zstep - end.GetZ()) / dir.GetZ();
		if ((tx <= ty) && (tx <= tz)) 
		{
			dist = tx, ix += idir[0];
			if (ty == tx) iy += idir[1];
			if (tz == tx) iz += idir[2];
		} 
		else if (ty <= tz) 
		{
			dist = ty, iy += idir[1];
			if (tz == ty) iz += idir[2];
		} 
		else dist = tz, iz += idir[2];
	}

	return false;
}
