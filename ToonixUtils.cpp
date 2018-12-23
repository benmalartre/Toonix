#include "ToonixUtils.h"

void DirectionToRotation(CMatrix3& matrix, CVector3f& in_dir, CVector3f& in_up)
{
	CVector3 v1, v2, dir, up;

	dir.Set(in_dir.GetX(), in_dir.GetY(), in_dir.GetZ());
	up.Set(in_up.GetX(), in_up.GetY(), in_up.GetZ());

	dir.NormalizeInPlace();
	up.NormalizeInPlace();

	v1.Cross(dir, up);
	v2.Cross(v1, dir);

	v1.NormalizeInPlace();
	v2.NormalizeInPlace();

	matrix.Set(v1.GetX(), v1.GetY(), v1.GetZ(), dir.GetX(), dir.GetY(), dir.GetZ(), v2.GetX(), v2.GetY(), v2.GetZ());
}

CVector3f RotateVector(CVector3f& v, CQuaternionf& quat)
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

	resQuat.Mul(vecQuat, q2);
	resQuat.Mul(quat, resQuat);

	CVector3f out(resQuat.GetX(), resQuat.GetY(), resQuat.GetZ());
	out.SetLength(len);
	return out;
}

void LogMatrix4(CMatrix4f& mat, CString header)
{
	float* m = mat.Get();
	CString log = header + L"(";
	for (int i = 0; i<16; i++)
	{
		log += (CString)m[i];
		if (i<15)log += L"|";
	}
	log += L")";
	Application().LogMessage(log);
}

void LogVector3(CVector3f& vec, CString header)
{
	Application().LogMessage(header + L"(" + (CString)vec.GetX() + L"," + (CString)vec.GetY() + L"," + (CString)vec.GetZ());
}
