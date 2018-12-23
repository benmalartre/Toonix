// Toonix Camera
//--------------------------------------------
#include "ToonixCamera.h"
#include "ToonixUtils.h"

TXPlane::TXPlane(void){};
TXPlane::~TXPlane(){};

void TXPlane::SetFromThreePoints(CVector3f& a, CVector3f& b, CVector3f& c)
{
	CVector3f ab, cb, n;
	ab.Sub(a,b);
	cb.Sub(c,b);
	m_norm.Cross(cb,ab);
	m_norm.NormalizeInPlace();

	m_pos = b;
	m_d = -(m_norm.Dot(m_pos));
}

void TXPlane::SetFromCamera(TXCamera* cam,bool negate)
{
	if(!negate)
	{
		CVector3f v(0,0,-1);
		m_norm = RotateVector(v, cam->m_quat);
		m_pos.ScaleAdd(cam->m_near,m_norm,cam->m_pos);
	}

	else
	{
		CVector3f v(0,0,1);
		m_norm = RotateVector(v, cam->m_quat);
		m_pos.ScaleAdd(-cam->m_far,m_norm,cam->m_pos);
	}
	m_d =  -(m_norm.Dot(m_pos));
}

void TXPlane::SetNormalAndPoint(CVector3f& norm, CVector3f &pnt)
{
	m_norm.Set(norm.GetX(),norm.GetY(),norm.GetZ());
	m_norm.NormalizeInPlace();
	m_d = -(m_norm.Dot(pnt));
}

float TXPlane::DistanceToPoint(CVector3f& v)
{
	return (m_d + m_norm.Dot(v));
}

void TXPlane::SetCoefficients(float a, float b, float c, float d)
{
	// set the normal vector
	m_norm.Set(a,b,c);

	//compute the length of the vector
	float l = m_norm.GetLength();

	// normalize the vector
	m_norm.Set(a/l,b/l,c/l);

	// and divide d by th length as well
	m_d = m_d/l;
}


void TXCamera::Set(const CMatrix4f& matrix, const float& fov, const float& aspect, const float& nearplane ,const float& farplane)
{
	m_mat = matrix;
	m_fov = fov;
	m_aspect = aspect;
	m_near = MAX(nearplane,0.001f);
	m_far = farplane;

	// compute width and height of the near and far plane sections
	m_tan = (float)tan(DegreesToRadians(m_fov)* 0.5);

	m_limits.resize(4);
	GetTransformation();
	//GetProjectionMatrix();
	GetLimits();
	GetPlanes();
}

void TXCamera::GetPlanes()
{
	CVector3f ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr, nc, fc;
	float nw, nh, fw, fh;

	nw = m_near * m_tan;
	nh = nw / m_aspect;
	fw = m_far * m_tan;
	fh = fw / m_aspect;

	m_planes.resize(6);
	CVector3f tmp;

	// compute the centers of the near and far planes
	tmp.Scale(-m_near, m_forward);
	nc.Sub(m_pos,tmp);
	tmp.Scale(-m_far, m_forward);
	fc.Sub(m_pos,tmp);

	// compute the 4 corners of the frustum on the near plane
	tmp.Scale(nh,m_up);
	ntl.Add(nc,tmp);
	ntr.Add(nc,tmp);
	nbl.Sub(nc,tmp);
	nbr.Sub(nc,tmp);

	tmp.Scale(nw,m_side);
	ntl.SubInPlace(tmp);
	ntr.AddInPlace(tmp);
	nbl.SubInPlace(tmp);
	nbr.AddInPlace(tmp);

	// compute the 4 corners of the frustum on the far plane
	tmp.Scale(fh,m_up);
	ftl.Add(fc,tmp);
	ftr.Add(fc,tmp);
	fbl.Sub(fc,tmp);
	fbr.Sub(fc,tmp);

	tmp.Scale(fw,m_side);
	ftl.SubInPlace(tmp);
	ftr.AddInPlace(tmp);
	fbl.SubInPlace(tmp);
	fbr.AddInPlace(tmp);

	m_planes[TOP].SetFromThreePoints(ntr,ntl,ftl);
	m_planes[BOTTOM].SetFromThreePoints(nbl,nbr,fbr);
	m_planes[LEFT].SetFromThreePoints(ntl,nbl,fbl);
	m_planes[RIGHT].SetFromThreePoints(nbr,ntr,fbr);
	m_planes[NEARP].SetFromCamera(this,false);
	m_planes[FARP].SetFromCamera(this,true);

	/*
	CMatrix4f mult;
	mult.Mul(_proj,_mat);

	Application().LogMessage(L"---------------------------------------------");
	LogMatrix4(_mat, L"Camera Matrix : ");
	LogMatrix4(_proj, L"Projection Matrix : ");
	LogMatrix4(mult, L"Multiplied Matrix : ");

	float* m = mult.Get();
	_planes.resize(6);

	//left clipping plane
	_planes[0]._a = m[12] + m[0];
	_planes[0]._b = m[13] + m[1];
	_planes[0]._c = m[14] + m[2];
	_planes[0]._d = m[15] + m[3];

	// Right clipping plane
	_planes[1]._a = m[12] - m[0];
	_planes[1]._b = m[13] - m[1];
	_planes[1]._c = m[14] - m[2];
	_planes[1]._d = m[15] - m[3];

	// Top clipping plane
	_planes[2]._a = m[12] - m[4];
	_planes[2]._b = m[13] - m[5];
	_planes[2]._c = m[14] - m[6];
	_planes[2]._d = m[15] - m[7];

	// Bottom clipping plane
	_planes[3]._a = m[12] + m[4];
	_planes[3]._b = m[13] + m[5];
	_planes[3]._c = m[14] + m[6];
	_planes[3]._d = m[15] + m[7];

	// Near clipping plane
	_planes[4]._a = m[12] + m[8];
	_planes[4]._b = m[13] + m[9];
	_planes[4]._c = m[14] + m[10];
	_planes[4]._d = m[15] + m[11];

	// Far clipping plane
	_planes[5]._a = m[12] - m[8];
	_planes[5]._b = m[13] - m[9];
	_planes[5]._c = m[14] - m[10];
	_planes[5]._d = m[15] - m[11];

	bool normalize = true;
	// Normalize the plane equations, if requested
	if (normalize == true)
	{
		for(int i=0;i<6;i++)
		{
			_planes[i].Normalize();
		}
	}
	*/
}
/*
void TXCamera::GetProjectionMatrix()
{
    // Some calculus before the formula.
    float size = _near * tanf(DegreesToRadians(_fov) / 2.0);
	//Application().LogMessage(L" Size : "+(CString)size);
    float left = -size, right = size, bottom = -size / _aspect, top = size / _aspect;

	float hnear = 2 * tan(_fov / 2) * _near;
	float wnear = hnear * _aspect;

	float hfar = 2 * tan(_fov / 2) * _far;
	float wfar = hfar * _aspect;
 
    // First Column
	float m0 = 2 * _near / (right - left);
    float m1 = 0.0;
	float m2 = 0.0;
    float m3 = 0.0;
 
    // Second Column
    float m4 = 0.0;
    float m5 = 2 * _near / (top - bottom);
    float m6 = 0.0;
    float m7 = 0.0;
 
    // Third Column
    float m8 = (right + left) / (right - left);
    float m9 = (top + bottom) / (top - bottom);
    float m10 = -(_far + _near) / (_far - _near);
    float m11 = -1;
 
    // Fourth Column
    float m12 = 0.0;
    float m13 = 0.0;
    float m14 = -(2 * _far * _near) / (_far - _near);
    float m15 = 0.0;

	_proj.Set(m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15);
}
*/

// tests if a sphere is within the frustrum
int TXCamera::GeometryInFrustrum(TXGeometry* geom)
{
	CVector3f d;
	d.Sub(geom->m_spherecenter, m_pos);
	if(d.Dot(m_forward)<0 && d.GetLength()>geom->m_sphereradius)
		return(OUTSIDE);
	int infrustrum = SphereInFrustrum(geom->m_spherecenter, geom->m_sphereradius);
	return infrustrum;
}

int TXCamera::SphereInFrustrum(CVector3f& center, float radius)
{
	float dist;
	// calculate our distances to each of the planes
	for(int i = 0; i < 6; ++i) 
	{
		// find the distance to this plane
		dist = m_planes[i].DistanceToPoint(center);

		// if this distance is < -sphere.radius, we are outside
		if(dist < -radius)
			return(OUTSIDE);

		// else if the distance is between +- radius, then we intersect
		if((float)fabs(dist) < radius)
			return(INTERSECT);
	}

	// otherwise we are fully in view
	return(INSIDE);
}

int TXCamera::PointInFrustrum(CVector3f &p) 
{
	int result = INSIDE;

	for(int i=0; i < 6; i++) {
		if (m_planes[i].DistanceToPoint(p) < 0)
			return OUTSIDE;
	}
	return(result);
}

void TXCamera::GetTransformation()
{
	m_scl.Set(1.0,1.0,1.0);
	float* m = m_mat.Get();

	float qx,qy,qz,qw,qw4;

	qw = sqrtf(1.0f + m[0] + m[5] + m[10]) * 0.5f;
	qw4 = 4.0f * qw;
	qx = (m[6] - m[9]) / qw4;
	qy = (m[8] - m[2]) / qw4;
	qz = (m[1] - m[4]) / qw4;

	m_quat.Set(qw,qx,qy,qz);
	m_pos.Set(m[12],m[13],m[14]);

	m_forward.Set(0,0,-1);
	m_forward = RotateVector(m_forward, m_quat);
	m_side.Set(1,0,0);
	m_side = RotateVector(m_side, m_quat);
	m_up.Set(0,1,0);
	m_up = RotateVector(m_up, m_quat);
}

/*
CVector3f TXCamera::RotateVector(CVector3f& v)
{
	float len = v.GetLength();
	CVector3f vn;
	CQuaternionf q2;

	vn.Normalize(v);
	q2.Conjugate(m_quat);

	CQuaternionf vecQuat, resQuat;

	vecQuat.PutX(vn.GetX());
	vecQuat.PutY(vn.GetY());
	vecQuat.PutZ(vn.GetZ());

	resQuat.Mul(vecQuat,q2);
	resQuat.Mul(m_quat,resQuat);

	CVector3f out(resQuat.GetX(),resQuat.GetY(),resQuat.GetZ());
	out.SetLength(len);
	return out;
}

*/
/*
bool TXCamera::See(CVector3f p)
{
	//Test each vertices
	float limitangle = DegreesToRadians(_fov);
	float currentangle, d;

	CVector3f dir;
	dir.Sub(p,_pos);
	//dir.NormalizeInPlace();
	d = dir.GetLength();
	currentangle = dir.GetAngle(_forward);
	if(d>_far || d<_near || currentangle>limitangle)
		return false;

	if(dir.Dot(_limits[0])<=0 ||dir.Dot(_limits[1])<=0 ||dir.Dot(_limits[2])<=0 ||dir.Dot(_limits[3])<=0)
		return false;

	return true;
}

void TXCamera::GetVisiblePoints(TXGeometry* geom)
{
	if(!geom->_culledges)
	{
		geom->SetAllVisible();
		return;
	}

	CVector3f forward(0,0,-1);
	forward = RotateVector(forward,_quat);
	
	geom->_visiblevertices.clear();
	geom->_nbvv = 0;
	CVector3f dir, axis, perp;
	axis.Set(0,1,0);
	perp.Set(1,0,0);
	float angle = DegreesToRadians(_fov *0.5);

	CVector3f limit1 = GetLimit(axis, perp, angle);

	axis.Set(0,-1,0);
	perp.Set(-1,0,0);
	CVector3f limit2 = GetLimit(axis, perp, angle);

	angle = DegreesToRadians(_fov*0.5*(1/_aspect));
	axis.Set(-1,0,0);
	perp.Set(0,1,0);
	CVector3f limit3 = GetLimit(axis, perp, angle);

	axis.Set(1,0,0);
	perp.Set(0,-1,0);
	CVector3f limit4 = GetLimit(axis, perp, angle);

	//Test Bounding Box Visibility
	CVector3f bbmin, bbmax;
	bbmin.Sub(geom->_min,_pos);
	bbmax.Sub(geom->_max,_pos);

	if(bbmin.Dot(limit1)>0 && bbmin.Dot(limit2)>0 && bbmin.Dot(limit3)>0 && bbmin.Dot(limit4)>0&&
		bbmax.Dot(limit1)>0 && bbmax.Dot(limit2)>0 && bbmax.Dot(limit3)>0 && bbmax.Dot(limit4)>0)
	{
		Application().LogMessage(L"All Points Are Visible --> Testing vertices Skipped...");
		geom->SetAllVisible();
		return;
	}

	//Test each vertices
	float limitangle = DegreesToRadians(_fov);
	float currentangle, d;

	for(ULONG p=0;p<geom->_vertices.size();p++)
	{
		dir.Sub(geom->_vertices[p]->_pos,_pos);
		//dir.NormalizeInPlace();
		d = dir.GetLength();
		currentangle = dir.GetAngle(forward);
		if(d>_far || d<_near || currentangle>limitangle)
		{
			geom->_vertices[p]->_visible = false;
			continue;
		}

		if(dir.Dot(limit1)<=0 ||dir.Dot(limit2)<=0 ||dir.Dot(limit3)<=0 ||dir.Dot(limit4)<=0)
		{
			geom->_vertices[p]->_visible = false;
		}
		else
		{
			geom->_visiblevertices.push_back(geom->_vertices[p]);
			geom->_vertices[p]->_visible = true;
			geom->_nbvv++;
		}
	}
}
*/
CVector3f TXCamera::GetLimit(CVector3f& axis,CVector3f& perp, float angle)
{	
	CRotationf r1, r2;
	r1.Set(axis,angle);
	r2.Set(m_quat);
	r1.Mul(r2);
	CQuaternionf q = r1.GetQuaternion();
	CVector3f limit = RotateVector(perp, q);
	return limit;
}

void TXCamera::GetLimits()
{
	m_forward.Set(0,0,-1);
	m_forward = RotateVector(m_forward, m_quat);
	
	CVector3f dir, axis, perp;
	axis.Set(0,1,0);
	perp.Set(1,0,0);
	float angle = (float)DegreesToRadians(m_fov *0.5f);

	m_limits[0] = GetLimit(axis, perp, angle);

	axis.Set(0,-1,0);
	perp.Set(-1,0,0);
	m_limits[1] = GetLimit(axis, perp, angle);

	angle = (float)DegreesToRadians(m_fov*0.5f*(1.0f/m_aspect));
	axis.Set(-1,0,0);
	perp.Set(0,1,0);
	m_limits[2] = GetLimit(axis, perp, angle);

	axis.Set(1,0,0);
	perp.Set(0,-1,0);
	m_limits[3] = GetLimit(axis, perp, angle);
}