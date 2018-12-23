//------------------------------------------------
// ToonixCommon.h
// common functions for Custom ICE Nodes
//------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixGLDrawer.h"

class TXLine;
class TXGLLine;

void InitTXLineData(CRef& in_ctxt);
void CleanTXLineData(CRef& in_ctxt);

void InitTXKDTreeData(CRef& in_ctxt);
void CleanTXKDTreeData(CRef& in_ctxt);

// Utilities
//-------------------------------------------------------

inline void MinimumVector3f(CVector3f & in_a, CVector3f & in_b)
{
	in_a.PutX(in_a.GetX() < in_b.GetX() ? in_a.GetX() : in_b.GetX());
	in_a.PutY(in_a.GetY() < in_b.GetY() ? in_a.GetY() : in_b.GetY());
	in_a.PutZ(in_a.GetZ() < in_b.GetZ() ? in_a.GetZ() : in_b.GetZ());
}

inline void MaximumVector3f(CVector3f & in_a, CVector3f & in_b)
{
	in_a.PutX(in_a.GetX() > in_b.GetX() ? in_a.GetX() : in_b.GetX());
	in_a.PutY(in_a.GetY() > in_b.GetY() ? in_a.GetY() : in_b.GetY());
	in_a.PutZ(in_a.GetZ() > in_b.GetZ() ? in_a.GetZ() : in_b.GetZ());
}

inline void RotateVector(CVector3f& v, const CQuaternionf& quat)
{
	float len = v.GetLength();
	CVector3f vn;
	CQuaternionf q2;

	vn.Normalize(v);
	q2.Conjugate(quat);

	CQuaternionf vecQuat, resQuat;

	vecQuat.PutX(vn.GetX());
	vecQuat.PutY(vn.GetY());
	vecQuat.PutZ(vn.GetZ());

	resQuat.Mul(vecQuat,q2);
	resQuat.Mul(quat,resQuat);

	v.Set(resQuat.GetX(),resQuat.GetY(),resQuat.GetZ());
	v.SetLength(len);
}
