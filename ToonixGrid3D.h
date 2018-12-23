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
  TXRay(const CVector3f& s, const CVector3f& d) : m_start(s), m_end(d), m_p(-1) { };

  // start position and end position
  CVector3f m_start, m_end;

  // if intersect with mesh, the intersection will be (1-p)*start + p*end
  // else p = -1
  double m_p;
};

class TXGridTriangle{
public:
	TXGridTriangle(TXTriangle* tp);

	const CVector3f& GetPos(ULONG id) const
	{ return m_t->m_v[id]->m_pos;};

	// original triangle
	TXTriangle* m_t;

	// face normal and edge normal (point inside of the triangle)
	CVector3f m_n, m_en1, m_en2, m_en3;
};

class TXGrid3D {
public:
  TXGrid3D() : m_size(0), m_xstep(0), m_ystep(0), m_zstep(0),
	     m_min(0,0,0), m_max(0,0,0), m_grid(0) { };
  TXGrid3D(int s) : m_size(0), m_xstep(0), m_ystep(0), m_zstep(0),
		  m_min(0,0,0), m_max(0,0,0), m_grid(0)
  { if (s>0) CreateGrid(s); };
  ~TXGrid3D();

  // member accessors
  int size() const { return m_size; };
  double xstep() const { return m_xstep; };
  double ystep() const { return m_ystep; };
  double zstep() const { return m_zstep; };
  CVector3f& minp() { return m_min; };
  CVector3f& maxp() { return m_max; };
  const CVector3f& minp() const { return m_min; };
  const CVector3f& maxp() const { return m_max; };
  std::vector<TXRay>& rays() { return m_rays; };
  // clear all rays
  void ClearRays() { m_rays.clear(); };

  // place a triangle mesh in the grid
  void PlaceIntoGrid(TXGeometry *geom);
  // intersect a ray with the mesh
  bool IntersectRay(const CVector3f& start, const CVector3f& end);

private:
  // grid size
  int m_size;
  // cell length in x, y, z directions
  double m_xstep, m_ystep, m_zstep;
  // bounding box of the mesh
  CVector3f m_min, m_max;
  // rays tested for intersection
  std::vector<TXRay> m_rays;
  // cells
  std::vector<TXGridTriangle*> *** m_grid;

  // allocate proper space for a grid of size s, free old
  void CreateGrid(int s);
  // place a triangle into intesected cells
  void PlaceTriangle(TXTriangle* t);
};

#endif