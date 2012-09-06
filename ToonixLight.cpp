// Toonix Light CPP
//-------------------------------------------------------
#include "ToonixLight.h"

TXLight::TXLight()
{
	_nbp = 0;
	_nbv = 0;
	_view = CVector3f(0,0,0);
	_lights.resize(0);
}

void TXLight::CutTriangle(TXGeometry* geom,ULONG a, ULONG b, ULONG c)
{
	CVector3f pa,pb,pc;
	pa = geom->_vertices[a]->_pos;
	pb = geom->_vertices[c]->_pos;

	float weight = ABS(geom->_vertices[a]->_dot)/(ABS(geom->_vertices[a]->_dot)+ABS(geom->_vertices[c]->_dot));
	pc.LinearlyInterpolate(pa,pb,weight);

	if(geom->_vertices[a]->_sign==-1)
		_vertices[a].Set(pc.GetX(),pc.GetY(),pc.GetZ());
		
	else
		_vertices[c].Set(pc.GetX(),pc.GetY(),pc.GetZ());

	pa = geom->_vertices[b]->_pos;
	pb = geom->_vertices[c]->_pos;
	weight = ABS(geom->_vertices[b]->_dot)/(ABS(geom->_vertices[b]->_dot)+ABS(geom->_vertices[c]->_dot));
	pc.LinearlyInterpolate(pa,pb,weight);

	if(geom->_vertices[a]->_sign==-1)
		_vertices[b].Set(pc.GetX(),pc.GetY(),pc.GetZ());
		
	else
		_vertices[c].Set(pc.GetX(),pc.GetY(),pc.GetZ());
}

void TXLight::ClearDatas()
{
	_geom->Unlit();
	_vertices.clear();
	_polygons.Clear();
	_litvertices.clear();
	_nbp = _nbv = 0;
}

bool TXLight::SeeTriangle(TXTriangle* t, CVector3f& p, float& dist)
{
	CVector3f a,b,c,d;
	a = _geom->_vertices[t->_pid[0]]->_pos;
	b = _geom->_vertices[t->_pid[1]]->_pos;
	c = _geom->_vertices[t->_pid[2]]->_pos;

	d.Set((a.GetX()+b.GetX()+c.GetX())/3,(a.GetY()+b.GetY()+c.GetY())/3,(a.GetZ()+b.GetZ()+c.GetZ())/3);
	d.Sub(d,p);

	if(d.GetLength()<dist)return true;
	else return false;
}

void TXLight::Build()
{
	ClearDatas();
	CVector3f pa,pb,pc;

	TXTriangle* triangle;
	bool islit;
	ULONG a,b,c;

	VerticeMap::iterator iter;

	for(ULONG l=0; l<_lights.size();l++)
	{
		_geom->GetDotAndSign(_lights[l],_bias[l]);

		for(ULONG t=0;t<_geom->_triangles.size();t++)
		{
			triangle = _geom->_triangles[t];

			// skip if triangle is NOT facing the camera
			if(!triangle->_facing)continue;

			// skip if triangle too far from light
			if(!SeeTriangle(triangle, _lights[l], _distance[l]))continue;

			islit = false;

			a=triangle->_pid[0];
			b=triangle->_pid[1];
			c=triangle->_pid[2];

			LONG sa,sb,sc;
			sa = _geom->_vertices[a]->_sign;
			sb = _geom->_vertices[b]->_sign;
			sc = _geom->_vertices[c]->_sign;

			int cnt = sa+sb+sc;
			int vi;

			if(_reverse)
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
					vi = triangle->_pid[ti];
					if(!_geom->_vertices[vi]->_lit)
					{
						//Push along normal
						CVector3f vn = _geom->_vertices[vi]->_norm;
						vn*=_push;
						vn+=_geom->_vertices[vi]->_pos;
						_vertices.push_back(vn);

						_litvertices.insert(std::pair<int,int>(vi,_nbv));
						_geom->_vertices[vi]->_lit = true;
						_nbv++;
					}
					iter = _litvertices.begin();
					iter = _litvertices.find(vi);
					if(iter != _litvertices.end())
						_polygons.Add(iter->second);
					else
						_polygons.Add(-2);
					_nbp++;
				}
				_polygons.Add(-2);
				_nbp++;
			}
		}
	}
}
