#ifndef __TOONIX_BBOX__
#define __TOONIX_BBOX__

#include "ToonixRegister.h"
#include <vector>

/**
* A float based bounding box.
*/
class TXBBox
{
private:
  CVector3f m_min;
  CVector3f m_max;
  bool m_validmin;
  bool m_validmax;
public:

  // constructors
  TXBBox();
  TXBBox(const TXBBox & in_other)
  {
     m_min = in_other.m_min;
     m_max = in_other.m_max;
     m_validmin = in_other.m_validmin;
     m_validmax = in_other.m_validmax;
  }
  TXBBox(const CVector3f & in_min,const CVector3f & in_max)
  {
     m_min = in_min;
     m_max = in_max;
     m_validmin = true;
     m_validmax = true;
  }
  void Clear();

  // getters
  CVector3f GetMin() const { return m_min; }
  CVector3f GetMax() const { return m_max; }
  CVector3f * GetMinPtr() { return &m_min; }
  CVector3f * GetMaxPtr() { return &m_max; }
  bool IsValidMin() const { return m_validmin; }
  bool IsValidMax() const { return m_validmax; }
  bool IsValid() const { return m_validmin && m_validmax; }
  CVector3f GetCenter() const;

  // setters
  void Set(const TXBBox & in_other)
  {
     m_min = in_other.m_min;
     m_max = in_other.m_max;
     m_validmin = in_other.m_validmin;
     m_validmax = in_other.m_validmax;
  }
  void SetMin(const CVector3f & in_min) { m_min = in_min; m_validmin = true; }
  void SetMax(const CVector3f & in_max) { m_max = in_max; m_validmax = true; }
  void Extend(float in_extension);

  // mergers
  void MergeMin( CVector3f & in_min );
  void MergeMax( CVector3f & in_max );
  void Merge( CVector3f & in_vec) { MergeMin(in_vec); MergeMax(in_vec); }
  void Merge( TXBBox & in_other);

  // manipulation
  void Scale(const CVector3f & in_scl);
  void Rotate(const CQuaternionf & in_rot);
  void Translate(const CVector3f & in_pos);

  // intersection
  bool IsPointInside(const CVector3f & in_pos) const;
  bool GetIntersectionWithBbox(const TXBBox & in_bboox, TXBBox * out_pResult = NULL);
};

typedef TXBBox * TXPBBox;
typedef std::vector<TXBBox> TXBBoxVec;
typedef std::vector<TXPBBox> TXPBBoxVec;

#endif
