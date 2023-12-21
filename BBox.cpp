#include "BBox.h"
#include "Common.h"


TXBBox::TXBBox()

{
   Clear();
}


CVector3f TXBBox::GetCenter() const
{
   CVector3f result;
   result.LinearlyInterpolate(GetMin(),GetMax(),0.5f);
   return result;
}

void TXBBox::MergeMin( CVector3f & in_min )
{
   if(m_validmin)

	   //if(in_min<_min)_min = in_min
      MinimumVector3f(m_min,in_min);
   else
      m_min =  in_min;
   m_validmin = true;
}


void TXBBox::MergeMax( CVector3f & in_max )
{
   if(m_validmax)
      MaximumVector3f(m_max,in_max);
   else
      m_max = m_max;
   m_validmax = true;
}

void TXBBox::Clear()
{
   m_min.Set(0,0,0);

   m_max.Set(0,0,0);
   m_validmin = false;

   m_validmax = false;

}

void TXBBox::Extend(float in_extension)
{
   if(m_validmin)
      m_min.SubInPlace(CVector3f(in_extension,in_extension,in_extension));
   if(m_validmax)
      m_max.AddInPlace(CVector3f(in_extension,in_extension,in_extension));
}

void TXBBox::Merge( TXBBox & in_other)
{
   if(in_other.IsValidMin())
      MergeMin(in_other.m_min);
   if(in_other.IsValidMax())
      MergeMax(in_other.m_max);
}


void TXBBox::Scale(const CVector3f & in_scl)

{
   if(m_validmin)
   {
      m_min.PutX( m_min.GetX()*in_scl.GetX());
	  m_min.PutY( m_min.GetY()*in_scl.GetY());
	  m_min.PutZ( m_min.GetZ()*in_scl.GetZ());
   }
   if(m_validmax)
   {
      m_max.PutX( m_max.GetX()*in_scl.GetX());
	  m_max.PutY( m_max.GetY()*in_scl.GetY());
	  m_max.PutZ( m_max.GetZ()*in_scl.GetZ());
   }
}


void TXBBox::Rotate(const CQuaternionf & in_rot)
{
   if(m_validmin)
      RotateVector(m_min,in_rot);
   if(m_validmax)
      RotateVector(m_max,in_rot);
}


void TXBBox::Translate(const CVector3f & in_pos)
{
   if(m_validmin)
      m_min.AddInPlace(in_pos);
   if(m_validmax)
      m_max.AddInPlace(in_pos);
}


bool TXBBox::IsPointInside(const CVector3f & in_pos) const
{
   return in_pos.GetX() >= m_min.GetX() && in_pos.GetX() <= m_max.GetX() &&
          in_pos.GetY() >= m_min.GetY() && in_pos.GetY() <= m_max.GetY() &&
          in_pos.GetZ() >= m_min.GetZ() && in_pos.GetZ() <= m_max.GetZ();
}

bool TXBBox::GetIntersectionWithBbox(const TXBBox & in_bbox, TXBBox * out_pResult)
{
   // check if the one box is inside the other one
   CVector3f min2,max2;
   min2 = in_bbox.GetMin();
   max2 = in_bbox.GetMax();

   if((min2.GetX() < m_min.GetX() && max2.GetX() < m_min.GetX()) ||
      (min2.GetX() > m_max.GetX() && max2.GetX() > m_max.GetX()) ||
      (min2.GetY() < m_min.GetY() && max2.GetY() < m_min.GetY()) ||
      (min2.GetY() > m_max.GetY() && max2.GetY() > m_max.GetY()) ||
      (min2.GetZ() < m_min.GetZ() && max2.GetZ() < m_min.GetZ()) ||
      (min2.GetZ() > m_max.GetZ() && max2.GetZ() > m_max.GetZ()))
      return false;

   // compose the resulting bbox
   if(out_pResult != NULL)
   {
		MinimumVector3f(min2,m_min);
		MaximumVector3f(max2,m_max);
		out_pResult->SetMin(min2);
		out_pResult->SetMax(max2);
   }
   return true;
}
