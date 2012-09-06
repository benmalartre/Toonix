// ToonixGrid3D.h
//-------------------------------------------------
#ifndef _TOONIX_GRID_3D_H_
#define _TOONIX_GRID_3D_H_

#include <vector>
#include "ToonixRegister.h"
#include "ToonixGeometry.h"

// ray class
class TXRay {
public:
  TXRay(const CVector3f& s, const CVector3f& d) : _start(s), _end(d), _p(-1) { };

  // start position and end position
  CVector3f _start, _end;

  // if intersect with mesh, the intersection will be (1-p)*start + p*end
  // else p = -1
  double _p;
};

class TXGridTriangle{
public:
	TXGridTriangle(TXTriangle* tp);

	const CVector3f& GetPos(ULONG id) const
	{ return _t->_v[id]->_pos;};

	// original triangle
	TXTriangle* _t;

	// face normal and edge normal (point inside of the triangle)
	CVector3f _n, _en1, _en2, _en3;
};

class TXGrid3D {
public:
  TXGrid3D() : _size(0), _xstep(0), _ystep(0), _zstep(0),
	     _min(0,0,0), _max(0,0,0), _grid(0) { };
  TXGrid3D(int s) : _size(0), _xstep(0), _ystep(0), _zstep(0),
		  _min(0,0,0), _max(0,0,0), _grid(0)
  { if (s>0) CreateGrid(s); };
  ~TXGrid3D();

  // member accessors
  int size() const { return _size; };
  double xstep() const { return _xstep; };
  double ystep() const { return _ystep; };
  double zstep() const { return _zstep; };
  CVector3f& minp() { return _min; };
  CVector3f& maxp() { return _max; };
  const CVector3f& minp() const { return _min; };
  const CVector3f& maxp() const { return _max; };
  std::vector<TXRay>& rays() { return _rays; };
  // clear all rays
  void ClearRays() { _rays.clear(); };

  // place a triangle mesh in the grid
  void PlaceIntoGrid(TXGeometry *geom);
  // intersect a ray with the mesh
  bool IntersectRay(const CVector3f& start, const CVector3f& end);

private:
  // grid size
  int _size;
  // cell length in x, y, z directions
  double _xstep, _ystep, _zstep;
  // bounding box of the mesh
  CVector3f _min, _max;
  // rays tested for intersection
  std::vector<TXRay> _rays;
  // cells
  std::vector<TXGridTriangle*> *** _grid;

  // allocate proper space for a grid of size s, free old
  void CreateGrid(int s);
  // place a triangle into intesected cells
  void PlaceTriangle(TXTriangle* t);
};

#endif