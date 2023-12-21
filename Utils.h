#ifndef TOONIX_UTILS_H
#define TOONIX_UTILS_H

#include "Register.h"

void DirectionToRotation(CMatrix3& matrix, CVector3f& in_dir, CVector3f& in_up);
CVector3f RotateVector(CVector3f& v, CQuaternionf& quat);

void LogMatrix4(CMatrix4f& mat, CString header);
void LogVector3(CVector3f& vec, CString header);

#endif
