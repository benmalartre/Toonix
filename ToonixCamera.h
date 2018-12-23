// ToonixCamera.h
//-------------------------------------------------
#ifndef TOONIX_CAMERA_H
#define TOONIX_CAMERA_H

#include "ToonixRegister.h"
#include "ToonixGeometry.h"
#include "ToonixUtils.h"

#define ANG2RAD 3.14159265358979323846/180.0

enum
{
	TOP = 0,
	BOTTOM,
	LEFT,
	RIGHT,
	NEARP,
	FARP
};

enum
{
	OUTSIDE = 0,
	INTERSECT,
	INSIDE
};

class TXGeometry;
class TXCamera;

// Plane Description
//------------------------------------
class TXPlane
{
public:
	CVector3f m_norm, m_pos;
	float m_d;

	TXPlane::TXPlane(CVector3f& v1, CVector3f& b, CVector3f& c);
	TXPlane::TXPlane(void);
	TXPlane::~TXPlane();

	void SetFromThreePoints(CVector3f& a, CVector3f& b, CVector3f& c);
	void SetNormalAndPoint(CVector3f& norm, CVector3f& pnt);
	void SetFromCamera(TXCamera* cam,bool negate);
	void SetCoefficients(float a, float b, float c, float d);
	float DistanceToPoint(CVector3f& p);
};

// Camera description
//------------------------------------
class TXCamera
{
public:
	std::vector<TXPlane> m_planes;
	std::vector<CVector3f> m_limits;

	// Camera Transform
	CMatrix4f m_mat;
	CMatrix4f m_proj;
	CVector3f m_scl;
	CQuaternionf m_quat;
	CVector3f m_pos;
	CVector3f m_forward, m_side,m_up;
	float m_near, m_far,m_tan,m_aspect,m_fov;

	void Set(const CMatrix4f& matrix, const float& fov, const float& aspect, const float& near,const float& far);
	/*
	void setCamInternals(float angle, float ratio, float nearD, float farD);
	void setCamDef(Vec3 &p, Vec3 &l, Vec3 &u);
	int pointInFrustum(Vec3 &p);
	int sphereInFrustum(Vec3 &p, float raio);
	int boxInFrustum(AABox &b);
	*/
	void GetTransformation();
	//void GetProjectionMatrix();
	//CVector3f RotateVector(CVector3f& perp);
	//void GetVisiblePoints(TXGeometry* geom);

	//CVector3f GetLimit(CVector3f& axis,CVector3f& perp, float angle);
	//bool See(CVector3f p);

	int GeometryInFrustrum(TXGeometry* geom);
	int SphereInFrustrum(CVector3f& center, float radius);
	int PointInFrustrum(CVector3f &p);

	CVector3f GetLimit(CVector3f& axis,CVector3f& perp, float angle);
	void GetLimits();
	void GetPlanes();

};

#endif