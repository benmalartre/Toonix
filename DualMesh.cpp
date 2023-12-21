// ToonixDualMesh.cpp
//-------------------------------------------------
#include "DualMesh.h"

std::vector<TXDualEdge*> TXDualEdge::m_checkededges;
const int TXOctree::MAX_EDGE_NUMBER = 10;

// eight surfaces of the 4D cube
enum { PX = 0, PY = 1, PZ = 2, PW = 3, NX = 4, NY = 5, NZ = 6, NW = 7 };

// DualEdge constructor
// assume pos1 and pos2 will project onto the tp-th surface
TXDualEdge::TXDualEdge(){};
TXDualEdge::TXDualEdge(TXEdge* edge, bool fac, int tp, const CVector4f& pos1, const CVector4f& pos2)
{
	m_e = edge;
	m_facing = fac;
	m_checked = false;
	m_triangles[0] = m_e->m_triangles[0];
	m_triangles[1] = m_e->m_triangles[1];
	
	m_eid[0] = m_e->GetIDInTriangle(m_triangles[0]);
	m_eid[1] = m_e->GetIDInTriangle(m_triangles[1]);;

	float d1, d2;
	float eps = float(DBL_EPSILON);
	switch (tp) {
	case PX:
	case NX:
	d1 = MAX(eps, fabs(pos1.GetX()));
	d2 = MAX(eps, fabs(pos2.GetX()));
	m_dp[0] = CVector3f(pos1.GetY()/d1, pos1.GetZ()/d1, pos1.GetW()/d1);
	m_dp[1] = CVector3f(pos2.GetY()/d2, pos2.GetZ()/d2, pos2.GetW()/d2);
	break;
	case PY:
	case NY:
	d1 = MAX(eps, fabs(pos1.GetY()));
	d2 = MAX(eps, fabs(pos2.GetY()));
	m_dp[0] = CVector3f(pos1.GetZ()/d1, pos1.GetW()/d1, pos1.GetX()/d1);
	m_dp[1] = CVector3f(pos2.GetZ()/d2, pos2.GetW()/d2, pos2.GetX()/d2);
	break;
	case PZ:
	case NZ:
	d1 = MAX(eps, fabs(pos1.GetZ()));
	d2 = MAX(eps, fabs(pos2.GetZ()));
	m_dp[0] = CVector3f(pos1.GetW()/d1, pos1.GetX()/d1, pos1.GetY()/d1);
	m_dp[1] = CVector3f(pos2.GetW()/d2, pos2.GetX()/d2, pos2.GetY()/d2);
	break;
	case PW:
	case NW:
	d1 = MAX(eps, fabs(pos1.GetW()));
	d2 = MAX(eps, fabs(pos2.GetW()));
	m_dp[0] = CVector3f(pos1.GetX()/d1, pos1.GetY()/d1, pos1.GetZ()/d1);
	m_dp[1] = CVector3f(pos2.GetX()/d2, pos2.GetY()/d2, pos2.GetZ()/d2);
	}
}

// intersect a box
bool TXDualEdge::Touch(const CVector3f& minp, const CVector3f& maxp) const {
  int m = 4;

  CVector3f step;
  step.Sub(m_dp[1], m_dp[0]);
  step.ScaleInPlace(1/4);
  CVector3f A = m_dp[0];
  CVector3f B;
  B.Add(m_dp[0],step);

  for (int i = 0; i < m; i++) {
    CVector3f bmin(MIN(A.GetX(),B.GetX()),MIN(A.GetY(),B.GetY()),MIN(A.GetZ(),B.GetZ()));
	CVector3f bmax(MAX(A.GetX(),B.GetX()),MAX(A.GetY(),B.GetY()),MAX(A.GetZ(),B.GetZ()));

    if (bmin.GetX() <= maxp.GetX() && minp.GetX() <= bmax.GetX() &&
	bmin.GetY() <= maxp.GetY() && minp.GetY() <= bmax.GetY() &&
	bmin.GetZ() <= maxp.GetZ() && minp.GetZ() <= bmax.GetZ()) return true;
    A = B;
    B += step;
  }
  return false;
}

void TXDualEdge::Project(TXOctree *dualmesh, TXEdge* edge) 
{
	if(edge->m_triangles.size()!=2)
	{
		edge->m_isboundary = true;
		return;
	}

	TXTriangle* t1 = edge->m_triangles[0];
	TXTriangle* t2 = edge->m_triangles[1];

	CVector3f tn1 = t1->m_norm;
	CVector3f tn2 = t2->m_norm;

	float ff = tn1.Dot(tn2);
	bool fac = (ff > 0);

	CVector4f n1(tn1.GetX(),tn1.GetY(),tn1.GetZ(),-tn1.Dot(t1->m_v[0]->m_pos));
	CVector4f N1;
	N1.Absolute(n1);

	CVector4f n2(tn2.GetX(),tn2.GetY(),tn2.GetZ(),-tn2.Dot(t2->m_v[0]->m_pos));
	CVector4f N2;
	N2.Absolute(n2);

	CVector4f n;
	n.Sub(n2,n1);

	float t[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

	// if the edge should be projected on two surfaces, it will intersect
	// the plane passing orig and the intersection line of the two surfaces
	// these are the 12 possible intersections
	if (n.GetX() - n.GetY() != 0) t[1] = - (n1.GetX() - n1.GetY()) / (n.GetX() - n.GetY());
	if (n.GetX() - n.GetZ() != 0) t[2] = - (n1.GetX() - n1.GetZ()) / (n.GetX() - n.GetZ());
	if (n.GetX() - n.GetW() != 0) t[3] = - (n1.GetX() - n1.GetW()) / (n.GetX() - n.GetW());
	if (n.GetX() + n.GetY() != 0) t[4] = - (n1.GetX() + n1.GetY()) / (n.GetX() + n.GetY());
	if (n.GetX() + n.GetZ() != 0) t[5] = - (n1.GetX() + n1.GetZ()) / (n.GetX() + n.GetZ());
	if (n.GetX() + n.GetW() != 0) t[6] = - (n1.GetX() + n1.GetW()) / (n.GetX() + n.GetW());
	if (n.GetY() - n.GetZ() != 0) t[7] = - (n1.GetY() - n1.GetZ()) / (n.GetY() - n.GetZ());
	if (n.GetY() - n.GetW() != 0) t[8] = - (n1.GetY() - n1.GetW()) / (n.GetY() - n.GetW());
	if (n.GetY() + n.GetZ() != 0) t[9] = - (n1.GetY() + n1.GetZ()) / (n.GetY() + n.GetZ());
	if (n.GetY() + n.GetW() != 0)t[10] = - (n1.GetY() + n1.GetW()) / (n.GetY() + n.GetW());
	if (n.GetZ() - n.GetW() != 0)t[11] = - (n1.GetZ() - n1.GetW()) / (n.GetZ() - n.GetW());
	if (n.GetZ() + n.GetW() != 0)t[12] = - (n1.GetZ() + n1.GetW()) / (n.GetZ() + n.GetW());

	int p1 = 0, p2;
	while (p1 < 13) 
	{
		p2 = 13;
		for (int i=1; i<13; i++) 
		{
			if (t[i] > t[p1] && t[i] < t[p2])p2 = i;
		}

		// the part that is projected on one of the surfaces
		CVector4f pos1, pos2;
		pos1.ScaleAdd(t[p1],n,n1);
		pos2.ScaleAdd(t[p2],n,n1);


		CVector4f mid;
		mid.Add(pos1,pos2);
		mid.ScaleInPlace(0.5);

		CVector4f MID;
		MID.Absolute(mid);

		p1 = p2;

		// the surface projected on
		int tp;
		if (MID.GetX() >= MID.GetY() && MID.GetX() >= MID.GetZ() && MID.GetX() >= MID.GetW())
		  tp = MID.GetX() > 0 ? PX : NX;
		else if (MID.GetY() >= MID.GetZ() && MID.GetY() >= MID.GetW())
		  tp = MID.GetY() > 0 ? PY : NY;
		else if (MID.GetZ() >= MID.GetW())
		  tp = MID.GetZ() > 0 ? PZ : NZ;
		else
		  tp = MID.GetW() > 0 ? PW : NW;

		// project this part on the surface
		TXDualEdge *dualedge = new TXDualEdge(edge, fac,tp, pos1, pos2);
		dualmesh[tp].Insert(dualedge);
	}

}

TXOctree::~TXOctree() 
{
  for (int i=0; i<8; i++) 
  {
    if (m_child[i]) delete m_child[i];
    m_child[i] = 0;
  }
  // delete dual edges
  if (m_depth == 0) 
  {
    size_t sz = m_edges.size();
    for (size_t j=0; j<sz; j++)
      delete m_edges[j];
  }
  m_edges.clear();
}

void TXOctree::Split() 
{
	size_t esz = m_edges.size();

	if (esz <= MAX_EDGE_NUMBER || 
	  (esz <= 2*MAX_EDGE_NUMBER && m_depth > 3) ||
	  (esz <= 3*MAX_EDGE_NUMBER && m_depth > 4) ||
	  m_depth > 6 ) 
	{
		m_isLeaf = true;
		return;
	}

	m_isLeaf = false;

	double xx[] = {m_min.GetX(), 0.5*(m_min.GetX()+m_max.GetX()), m_max.GetX()};
	double yy[] = {m_min.GetY(), 0.5*(m_min.GetY()+m_max.GetY()), m_max.GetY()};
	double zz[] = {m_min.GetZ(), 0.5*(m_min.GetZ()+m_max.GetZ()), m_max.GetZ()};

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++) 
			{
				int m = 4*i + 2*j + k;
				m_child[m] = new TXOctree( CVector3f(xx[i], yy[j], zz[k]),CVector3f(xx[i+1], yy[j+1], zz[k+1]),m_depth+1 );

				size_t esz = m_edges.size();

				for (int t = 0; t < esz; t++)
					if (m_edges[t]->Touch(m_child[m]->GetMin(), m_child[m]->GetMax()))
						m_child[m]->Insert(m_edges[t]);

				if (m_child[m]->GetSize() == 0) 
				{
				  delete m_child[m];
				  m_child[m] = 0;
				} 
				else m_child[m]->Split();
			}

	if (m_depth > 0)m_edges.clear();
}

// intersect a plane
bool TXOctree::TouchPlane(const CVector3f& n, float d, float bias) 
{
	bool sa = n.GetX()>=0, sb = n.GetY()>=0, sc = n.GetZ()>=0;
	float p1x = m_min.GetX(), p1y, p1z, p2x = m_max.GetX(), p2y, p2z;

	if (sb == sa) 
	{
		p1y = m_min.GetY(); p2y = m_max.GetY();
	} 
	else 
	{
		p1y = m_max.GetY(); p2y = m_min.GetY();
	}

	if (sc == sa) 
	{
		p1z = m_min.GetZ(); p2z = m_max.GetZ();
	} 
	else 
	{
		p1z = m_max.GetZ(); p2z = m_min.GetZ();
	}

	float dot1 = n.GetX()*p1x + n.GetY()*p1y + n.GetZ()*p1z + d ;
	float dot2 = n.GetX()*p2x + n.GetY()*p2y + n.GetZ()*p2z + d ;
	bool sd1 = dot1 >= bias;
	bool sd2 = dot2 >= bias;
	return (sd1 != sd2);
}

// looking for silhouettes recursively
void TXOctree::FindSilhouettes(const CVector3f& n, float d, std::vector<TXEdge*>& silhouettes, float bias) 
{
	if (m_isLeaf) 
	{
		TXEdge* e;
		for (ULONG i = 0; i <m_edges.size(); i++)
		{
			if (!m_edges[i]->IsChecked()) 
			{
				m_edges[i]->Check();
				
				bool b1 = n.Dot(m_edges[i]->GetDualPosition(0)) + d >= bias;
				bool b2 = n.Dot(m_edges[i]->GetDualPosition(1)) + d >= bias;

				e = m_edges[i]->GetEdge();

				if (b1 != b2) 
				{
					
					e->m_issilhouette = true;
					silhouettes.push_back(e);
				}
				else
					e->m_issilhouette = false;
			}
		}
	} 

	else 
	{
		for (int i = 0; i < 8; i++)
		{
		  if (m_child[i])
			if (m_child[i]->TouchPlane(n, d,bias))
			  m_child[i]->FindSilhouettes(n, d, silhouettes,bias);
		}
	}
}

// build octree
TXOctree* TXOctree::BuildTree(TXGeometry *geo) 
{
	TXOctree* tree = new TXOctree[8];

	// loop over all edges, insert all leaves to the tree
	std::vector<TXEdge*>::iterator it;
	for(it = geo->m_edges.begin();it<geo->m_edges.end();it++)
	{
		TXDualEdge::Project(tree, *it);
	}

	for (int j = 0; j < 8; j++) tree[j].Split();

	tree->m_isLeaf = false;

	return tree;
}
