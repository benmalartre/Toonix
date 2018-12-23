// Toonix Light CPP
//-------------------------------------------------------
#include "ToonixLight.h"

TXLight::TXLight()
{
	m_nbp = 0;
	m_nbv = 0;
	m_view = CVector3f(0,0,0);
	m_lights.resize(0);
}

void TXLight::CutTriangle(TXGeometry* geom,ULONG a, ULONG b, ULONG c)
{
	CVector3f pa,pb,pc;
	pa = geom->m_vertices[a]->m_pos;
	pb = geom->m_vertices[c]->m_pos;

	float weight = ABS(geom->m_vertices[a]->m_dot)/(ABS(geom->m_vertices[a]->m_dot)+ABS(geom->m_vertices[c]->m_dot));
	pc.LinearlyInterpolate(pa,pb,weight);

	if(geom->m_vertices[a]->m_sign==-1)
		m_vertices[a].Set(pc.GetX(),pc.GetY(),pc.GetZ());
		
	else
		m_vertices[c].Set(pc.GetX(),pc.GetY(),pc.GetZ());

	pa = geom->m_vertices[b]->m_pos;
	pb = geom->m_vertices[c]->m_pos;
	weight = ABS(geom->m_vertices[b]->m_dot)/(ABS(geom->m_vertices[b]->m_dot)+ABS(geom->m_vertices[c]->m_dot));
	pc.LinearlyInterpolate(pa,pb,weight);

	if(geom->m_vertices[a]->m_sign==-1)
		m_vertices[b].Set(pc.GetX(),pc.GetY(),pc.GetZ());
		
	else
		m_vertices[c].Set(pc.GetX(),pc.GetY(),pc.GetZ());
}

void TXLight::ClearDatas()
{
	m_geom->Unlit();
	m_vertices.clear();
	m_polygons.Clear();
	m_litvertices.clear();
	m_nbp = m_nbv = 0;
}

bool TXLight::SeeTriangle(TXTriangle* t, CVector3f& p, float& dist)
{
	if(t->IsVisible())
	{
		CVector3f a,b,c,d;
		a = m_geom->m_vertices[t->m_pid[0]]->m_pos;
		b = m_geom->m_vertices[t->m_pid[1]]->m_pos;
		c = m_geom->m_vertices[t->m_pid[2]]->m_pos;

		d.Set((a.GetX()+b.GetX()+c.GetX())/3,(a.GetY()+b.GetY()+c.GetY())/3,(a.GetZ()+b.GetZ()+c.GetZ())/3);
		d.Sub(d,p);

		if(d.GetLength()<dist)return true;
	}
	return false;
}

void TXLight::Build()
{
	ClearDatas();
	CVector3f pa,pb,pc;

	TXTriangle* triangle;
	bool islit;
	ULONG a,b,c;

	VerticeMap::iterator iter;

	for(ULONG l=0; l<m_lights.size();l++)
	{
		m_geom->GetDotAndSign(m_lights[l],m_bias[l]);

		for(ULONG t=0;t<m_geom->m_triangles.size();t++)
		{
			triangle = m_geom->m_triangles[t];

			// skip if triangle is NOT facing the camera
			if(!triangle->m_facing)continue;

			// skip if triangle too far from light
			if(!SeeTriangle(triangle, m_lights[l], m_distance[l]))continue;

			islit = false;

			a=triangle->m_pid[0];
			b=triangle->m_pid[1];
			c=triangle->m_pid[2];

			LONG sa,sb,sc;
			sa = m_geom->m_vertices[a]->m_sign;
			sb = m_geom->m_vertices[b]->m_sign;
			sc = m_geom->m_vertices[c]->m_sign;

			int cnt = sa+sb+sc;
			int vi;

			if(m_reverse)
			{
				if(cnt >0)islit = true;
			}
			else
			{
				if(cnt <0)islit = true;
			}

			if (islit)
			{
				for(ULONG ti=0;ti<3;ti++)
				{
					vi = triangle->m_pid[ti];
					if(!m_geom->m_vertices[vi]->m_lit)
					{
						//Push along normal
						CVector3f vn = m_geom->m_vertices[vi]->m_norm;
						vn*=m_push;
						vn+=m_geom->m_vertices[vi]->m_pos;
						m_vertices.push_back(vn);

						m_litvertices.insert(std::pair<int,int>(vi,m_nbv));
						m_geom->m_vertices[vi]->m_lit = true;
						m_nbv++;
					}
					iter = m_litvertices.begin();
					iter = m_litvertices.find(vi);
					if(iter != m_litvertices.end())
						m_polygons.Add(iter->second);
					else
						m_polygons.Add(-2);
					m_nbp++;
				}
				m_polygons.Add(-2);
				m_nbp++;
			}
		}
	}
}
