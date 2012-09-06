// Toonix Grid3D
//----------------------------------------------

#include "ToonixRegister.h"
#include "ToonixGrid3D.h"

TXGridTriangle::TXGridTriangle(TXTriangle *tp)
{
	_t = tp;
	_n = _t->_norm;

	CVector3f tmp;
	tmp.Sub(_t->_v[0]->_pos,_t->_v[1]->_pos);
	tmp.Cross(tmp,_n);
	_en1.Normalize(tmp);
	tmp.Sub(_t->_v[1]->_pos,_t->_v[2]->_pos);
	tmp.Cross(tmp,_n);
	_en2.Normalize(tmp);
	tmp.Sub(_t->_v[2]->_pos,_t->_v[0]->_pos);
	tmp.Cross(tmp,_n);
	_en3.Normalize(tmp);
}

// place a triangle mesh in the grid
void TXGrid3D::PlaceIntoGrid(TXGeometry *geom) 
{
	_min = CVector3f(DBL_MAX, DBL_MAX, DBL_MAX);
	_max = CVector3f(-DBL_MAX, -DBL_MAX, -DBL_MAX);

	// compute bounding box
	int totalTriangles = 0;

	TXTriangle* tp;
	CVector3f p;

	for(ULONG t=0;t<geom->_triangles.size();t++)
	{
		tp = geom->_triangles[t];
		totalTriangles++;
		for(int i=0;i<3;i++)
		{
			p = tp->_v[i]->_pos;
			_min.Set(MIN(_min.GetX(),p.GetX()),MIN(_min.GetY(),p.GetY()),MIN(_min.GetZ(),p.GetZ()));
			_max.Set(MAX(_max.GetX(),p.GetX()),MAX(_max.GetY(),p.GetY()),MAX(_max.GetZ(),p.GetZ()));
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

	_xstep = (_max.GetX() - _min.GetX()) / _size;
	_ystep = (_max.GetY() - _min.GetY()) / _size;
	_zstep = (_max.GetZ() - _min.GetZ()) / _size;

	for(ULONG t=0;t<geom->_triangles.size();t++)
	{
		PlaceTriangle(geom->_triangles[t]);
	}
	 
}

// allocate proper space for a grid of size s, free old
void TXGrid3D::CreateGrid(int s) 
{
	int i, j, k;
	// clear old space
	if (_grid && (_size > 0)) 
	{
		for (i=0; i<_size; i++)
			for (j=0; j<_size; j++)
				for (k=0; k<_size; k++)
					_grid[i][j][k].clear();
	}

	// if size changed, free old and allocate new
	if (s != _size) 
	{
		if (_grid) 
		{
			for (i=0; i<_size; i++)
				for (j=0; j<_size; j++)
					delete[] _grid[i][j];
				for (i=0; i<_size; i++)
					delete[] _grid[i];
				delete[] _grid;
		}
		_size = s;
		_grid = new std::vector<TXGridTriangle*>**[s];
		for (i=0; i<s; i++)
		  _grid[i] = new std::vector<TXGridTriangle*>*[s];
		for (i=0; i<s; i++)
		  for (j=0; j<s; j++)
		_grid[i][j] = new std::vector<TXGridTriangle*>[s];
	}
}

// free space
TXGrid3D::~TXGrid3D() 
{
	int i, j, k;
	if (_grid && (_size > 0)) 
	{
		for (i=0; i<_size; i++) 
		{
			for (j=0; j<_size; j++)
			{
				for (k=0; k<_size; k++) 
				{
				  _grid[i][j][k].clear();
				}
				delete[] _grid[i][j];
			}
			delete[] _grid[i];
		}
		delete[] _grid;
	}
	_grid = 0;
	_rays.clear();
}

// place a triangle into intesected cells
void TXGrid3D::PlaceTriangle(TXTriangle *t)
{
	// bounding box
	CVector3f tmin(DBL_MAX, DBL_MAX, DBL_MAX), tmax(-DBL_MAX, -DBL_MAX, -DBL_MAX);
	for (int v=0; v<3; v++)
	{
		tmin.Set(MIN(tmin.GetX(),t->_v[v]->_pos.GetX()),
					MIN(tmin.GetY(),t->_v[v]->_pos.GetY()),
					MIN(tmin.GetZ(),t->_v[v]->_pos.GetZ()));

		
		tmax.Set(MAX(tmax.GetX(),t->_v[v]->_pos.GetX()),
					MAX(tmax.GetY(),t->_v[v]->_pos.GetY()),
					MAX(tmax.GetZ(),t->_v[v]->_pos.GetZ()));
	}

	// starting and ending cells of the bounding box
	CVector3 start(MAX((int)floor((tmin.GetX() - _min.GetX())/_xstep), 0),
		MAX((int)floor((tmin.GetY() - _min.GetY())/_ystep), 0),
		MAX((int)floor((tmin.GetZ() - _min.GetZ())/_zstep), 0));

	CVector3 end(MIN((int)floor((tmax.GetX() - _min.GetX())/_xstep), _size-1),
		MIN((int)floor((tmax.GetY() - _min.GetY())/_ystep), _size-1),
		MIN((int)floor((tmax.GetZ() - _min.GetZ())/_zstep), _size-1));

	TXGridTriangle* gt = new TXGridTriangle(t);

	bool sa = (gt->_n.GetX() >= 0), sb = (gt->_n.GetY() >= 0), sc = (gt->_n.GetZ() >= 0);
	CVector3f p1, p2;
	bool sd1, sd2;

	// put triangle in the cells in its bounding box
	// still excessive, treat triangle as a plane to detect intersection
	for (int i=(int)start.GetX(); i<=(int)end.GetX(); i++) 
	{
		p1.Set(_min.GetX() + i*_xstep,p1.GetY(),p2.GetZ());
		p2.Set(p1.GetX() + _xstep,p2.GetY(),p2.GetZ());

		for (int j=start.GetY(); j<=end.GetY(); j++) 
		{
			if (sb == sa) 
			{
				p1.Set(p1.GetX(),_min.GetY() + j*_ystep,p1.GetZ());
				p2.Set(p2.GetX(),p1.GetY()+ _ystep,p2.GetZ()); 
			}
			else
			{
				p2.Set(p2.GetX(), _min.GetY() + j*_ystep,p2.GetZ());
				p1.Set(p1.GetX(),p2.GetY() + _ystep,p1.GetZ());
			}
			for (int k=start.GetZ(); k<=end.GetZ(); k++) 
			{
				if (sc == sa)
				{
					p1.Set(p1.GetX(),p1.GetY(),_min.GetZ() + k*_zstep);
					p2.Set(p2.GetX(),p2.GetY(),p1.GetZ() + _zstep);
				}
				else
				{
					p2.Set(p2.GetX(),p2.GetY(),_min.GetZ() + k*_zstep);
					p1.Set(p1.GetX(),p1.GetY(),p2.GetZ() + _zstep);
				}
			
				CVector3f p1gt, p2gt;
				p1gt.Sub(p1,gt->_t->_v[0]->_pos);
				p2gt.Sub(p2,gt->_t->_v[0]->_pos);
				sd1 = gt->_n.Dot(p1gt) >= 0;
				sd2 = gt->_n.Dot(p2gt) >= 0;
				if (sd1 != sd2) 
					_grid[i][j][k].push_back(gt);
			}
		}
	}
}

// intersect a ray with the mesh
bool TXGrid3D::IntersectRay(const CVector3f& start, const CVector3f& end) 
{
	// pick rays
	_rays.push_back(TXRay(start, end));
	CVector3f dir;
	dir.Sub(start,end);

	int idir[3] = {
	fabs(dir.GetX()) < DBL_EPSILON ? 0 : (dir.GetX() > 0 ? 1 : -1),
	fabs(dir.GetY()) < DBL_EPSILON ? 0 : (dir.GetY() > 0 ? 1 : -1),
	fabs(dir.GetZ()) < DBL_EPSILON ? 0 : (dir.GetZ() > 0 ? 1 : -1) };

	double dist = 0;
	CVector3f pos;
	pos.Sub(end, _min);

	int ix = (int)floor((end.GetX() - _min.GetX())/_xstep),
	iy = (int)floor((end.GetY() - _min.GetY())/_ystep),
	iz = (int)floor((end.GetZ() - _min.GetZ())/_zstep);
	double tx = 1.0, ty = 1.0, tz = 1.0;

	// first cell
	ix = MIN(MAX(ix, 0), _size-1);
	iy = MIN(MAX(iy, 0), _size-1);
	iz = MIN(MAX(iz, 0), _size-1);

	// intersection test, from end to start
	while ((dist < 1) && (ix >= 0) && (ix < _size) &&(iy >= 0) && (iy < _size) && (iz >= 0) && (iz < _size)) 
	{
		int csz = _grid[ix][iy][iz].size();
		for (int i=0; i<csz; i++) 
		{
			TXGridTriangle* gt = _grid[ix][iy][iz][i];
			double rpdot = dir.Dot(gt->_n);
			if (rpdot != 0) 
			{
				CVector3f tmp;
				tmp.Sub(gt->_t->_v[0]->_pos,end);
				double t = tmp.Dot(gt->_n) / rpdot;

				if (t > DBL_EPSILON && t < 1) 
				{
					CVector3f pt;
					pt.ScaleAdd(t,dir,end);

					CVector3f pt0,pt1,pt2;
					pt0.Sub(pt,gt->_t->_v[0]->_pos);
					pt1.Sub(pt,gt->_t->_v[1]->_pos);
					pt2.Sub(pt,gt->_t->_v[2]->_pos);


					if (pt0.Dot(gt->_en1)> -DBL_EPSILON &&
					  pt1.Dot(gt->_en2)>-DBL_EPSILON &&
					  pt2.Dot(gt->_en3)>-DBL_EPSILON) 
					{
						_rays[_rays.size()-1]._p = t;
						return true;
					}
				}
			}
		}

		// next cell
		if (idir[0] != 0)
		  tx = (_min.GetX() + (ix+(idir[0]+1)/2)*_xstep - end.GetX()) / dir.GetX();
		if (idir[1] != 0)
		  ty = (_min.GetY() + (iy+(idir[1]+1)/2)*_ystep - end.GetY()) / dir.GetY();
		if (idir[2] != 0)
		  tz = (_min.GetZ() + (iz+(idir[2]+1)/2)*_zstep - end.GetZ()) / dir.GetZ();
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
