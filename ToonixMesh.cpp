// Toonix Mesh CPP
//-------------------------------------------------------
#include "ToonixMesh.h"

TXMesh::TXMesh(TXLine *line)
{
	m_nbv = 0;
	m_nbp = 0;
	m_factor = 1;
	m_line=line;
}

void TXMesh::Build()
{
	m_subdiv = MAX(2,m_subdiv);
	m_line->m_width = MAX(0.001f,m_line->m_width);
	m_nbv = m_nbp =0;

	m_section.Resize(m_subdiv);
	m_section[0].Set(1.0,0,0);
	CRotation pointsToInstanceRot(0,-.5 * DegreesToRadians(360.0 / float(m_subdiv)),0);
	m_section[0].MulByMatrix3InPlace(pointsToInstanceRot.GetMatrix());
	pointsToInstanceRot.SetFromXYZAngles(0,DegreesToRadians(360.0 / float(m_subdiv)),0);
	for(ULONG i=1;i<m_subdiv;i++)m_section[i].MulByMatrix3(m_section[i-1],pointsToInstanceRot.GetMatrix());

	m_vertices.clear();
	m_polygons.Clear();

	for(ULONG c=0;c<m_line->m_chains.size();c++)
	{
		m_points = &m_line->m_chains[c]->m_points;
		bool hasnext = false;
		bool firstcap = true;
		bool closed = false;
		ULONG pSize = (ULONG)m_points->size();

		for(ULONG p =0;p<pSize;p++)
		{
			if(p<(pSize-1))
			{
				closed = false;
				hasnext = true;
			}
			else 
			{
				hasnext = false;
				if(pSize>2)closed = m_line->m_chains[c]->m_closed;
				else closed = false;
			}

			AddPoint( p, firstcap, hasnext, closed,pSize);
			if(!hasnext)firstcap = true;
			else firstcap = false;
		}
	}
}

void TXMesh::AddPoint(LONG id, bool firstcap, bool hasnext, bool closed, ULONG chainsize)
{
	m_xf.SetSclX((*m_points)[id]->m_radius);
	m_xf.SetSclY((*m_points)[id]->m_length);
	m_xf.SetSclZ((*m_points)[id]->m_radius);

	CVector3f tangent,up,norm;
	if(hasnext)
	{
		tangent.LinearlyInterpolate((*m_points)[id]->m_dir,(*m_points)[id+1]->m_dir,0.5);
		norm.LinearlyInterpolate((*m_points)[id]->m_norm,(*m_points)[id+1]->m_norm,0.5);
		
		up.LinearlyInterpolate((*m_points)[id]->m_pos,(*m_points)[id+1]->m_pos,0.5);
		up.SubInPlace(m_view);
		
		
		up.Cross(tangent,up);
		/*
		up.NormalizeInPlace();
		up.Cross(tangent,up);
		*/
		

	}
	else
	{
		tangent = (*m_points)[id]->m_dir;
		norm = (*m_points)[id]->m_norm;
		
		up = (*m_points)[id]->m_pos;
		up.SubInPlace(m_view);
		
		up.Cross(tangent,up);
		/*
		up.NormalizeInPlace();
		up.Cross(tangent,up);
		*/
		
	}

	DirectionToRotation(tangent,up);
	m_xf.SetRotationFromMatrix3(m_matrix);

	m_offset.Normalize((*m_points)[id]->m_dir);
	m_offset.ScaleInPlace((*m_points)[id]->m_length * 0.5f);
	m_noffset.Negate(m_offset);
	
	if(firstcap)
	{
		m_xf.SetPosX((*m_points)[id]->m_pos.GetX()+m_noffset.GetX());
		m_xf.SetPosY((*m_points)[id]->m_pos.GetY()+m_noffset.GetY());
		m_xf.SetPosZ((*m_points)[id]->m_pos.GetZ()+m_noffset.GetZ());

		for(ULONG j=0;j<m_subdiv;j++)
		{
			m_p.MulByTransformation(m_section[j],m_xf);
			m_vertices.push_back(CVector3f((float)m_p.GetX(),(float)m_p.GetY(),(float)m_p.GetZ()));
			m_nbv++;
		}
	}

	m_xf.SetPosX((*m_points)[id]->m_pos.GetX()+m_offset.GetX());
	m_xf.SetPosY((*m_points)[id]->m_pos.GetY()+m_offset.GetY());
	m_xf.SetPosZ((*m_points)[id]->m_pos.GetZ()+m_offset.GetZ());

	if(hasnext==true)
	{
		DirectionToRotation(tangent,up);
		m_xf.SetRotationFromMatrix3(m_matrix);
	}

	ULONG sub =0;

	if(m_subdiv>2)
	{
		for(ULONG j=0;j<m_subdiv;j++)
		{
			if(!closed) sub = 0;
			else sub =  chainsize * m_subdiv;
			
			ULONG s = (ULONG)m_vertices.size();
			if(j==m_subdiv-1)
			{
				m_polygons.Add(s - m_subdiv - m_subdiv + 1);
				m_polygons.Add(s - m_subdiv + 1-sub);
			}
			else
			{
				m_polygons.Add(s - m_subdiv + 1);
				m_polygons.Add(s + 1 - sub);
			}
			m_polygons.Add(s-sub);
			m_polygons.Add(s - m_subdiv);

			m_polygons.Add(-2);
			m_nbp+=5;

			//if(!closed)
			//{
				m_p.MulByTransformation(m_section[j],m_xf);
				//_vertices.Add(_p);
				m_vertices.push_back(CVector3f((float)m_p.GetX(),(float)m_p.GetY(),(float)m_p.GetZ()));
			//}
			m_nbv++;
		}
	}
	else
	{
		if(!closed) sub = 0;
		else sub = chainsize * m_subdiv;
		
		ULONG s = (ULONG)m_vertices.size();
		m_polygons.Add(s - m_subdiv + 1);
		m_polygons.Add(s + 1 - sub);
		m_polygons.Add(s - sub);
		m_polygons.Add(s - m_subdiv);
		m_polygons.Add(-2);
		m_nbp+=5;

		if(!closed)
		{
			m_p.MulByTransformation(m_section[0],m_xf);
			m_vertices.push_back(CVector3f((float)m_p.GetX(),(float)m_p.GetY(),(float)m_p.GetZ()));
			m_p.MulByTransformation(m_section[1],m_xf);
			m_vertices.push_back(CVector3f((float)m_p.GetX(),(float)m_p.GetY(),(float)m_p.GetZ()));
			m_nbv+=2;
		}
	}

	// cap line
	if(!hasnext && m_subdiv>2)
	{

	}

}

void TXMesh::DirectionToRotation(CVector3f& in_dir, CVector3f& in_up)
{
	CVector3 v1, v2,dir,up;

	dir.Set(in_dir.GetX(),in_dir.GetY(),in_dir.GetZ());
	up.Set(in_up.GetX(),in_up.GetY(),in_up.GetZ());

	dir.NormalizeInPlace();
	up.NormalizeInPlace();

	v1.Cross(dir, up);
	v2.Cross(v1, dir);

	v1.NormalizeInPlace();
	v2.NormalizeInPlace();

	m_matrix.Set(v1.GetX(), v1.GetY(), v1.GetZ(), dir.GetX(), dir.GetY(), dir.GetZ(), v2.GetX(), v2.GetY(), v2.GetZ());
}
