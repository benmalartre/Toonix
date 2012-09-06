// Toonix Mesh CPP
//-------------------------------------------------------
#include "ToonixMesh.h"

TXMesh::TXMesh(TXLine *line)
{
	_nbv = 0;
	_nbp = 0;
	_factor = 1;
	_line=line;
}

void TXMesh::Build()
{
	_subdiv = MAX(2,_subdiv);
	_line->_width = MAX(0.001f,_line->_width);
	_nbv = _nbp =0;

	_section.Resize(_subdiv);
	_section[0].Set(1.0,0,0);
	CRotation pointsToInstanceRot(0,-.5 * DegreesToRadians(360.0 / float(_subdiv)),0);
	_section[0].MulByMatrix3InPlace(pointsToInstanceRot.GetMatrix());
	pointsToInstanceRot.SetFromXYZAngles(0,DegreesToRadians(360.0 / float(_subdiv)),0);
	for(ULONG i=1;i<_subdiv;i++)_section[i].MulByMatrix3(_section[i-1],pointsToInstanceRot.GetMatrix());

	_vertices.clear();
	_polygons.Clear();

	for(ULONG c=0;c<_line->_chains.size();c++)
	{
		_points = &_line->_chains[c]->_points;
		bool hasnext = false;
		bool firstcap = true;
		bool closed = false;
		ULONG pSize = (ULONG)_points->size();

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
				if(pSize>2)closed = _line->_chains[c]->_closed;
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
	_xf.SetSclX((*_points)[id]->_radius);
	_xf.SetSclY((*_points)[id]->_length);
	_xf.SetSclZ((*_points)[id]->_radius);

	CVector3f tangent,up,norm;
	if(hasnext)
	{
		tangent.LinearlyInterpolate((*_points)[id]->_dir,(*_points)[id+1]->_dir,0.5);
		norm.LinearlyInterpolate((*_points)[id]->_norm,(*_points)[id+1]->_norm,0.5);
		
		up.LinearlyInterpolate((*_points)[id]->_pos,(*_points)[id+1]->_pos,0.5);
		up.SubInPlace(_view);
		
		
		up.Cross(tangent,up);
		/*
		up.NormalizeInPlace();
		up.Cross(tangent,up);
		*/
		

	}
	else
	{
		tangent = (*_points)[id]->_dir;
		norm = (*_points)[id]->_norm;
		
		up = (*_points)[id]->_pos;
		up.SubInPlace(_view);
		
		up.Cross(tangent,up);
		/*
		up.NormalizeInPlace();
		up.Cross(tangent,up);
		*/
		
	}

	DirectionToRotation(tangent,up);
	_xf.SetRotationFromMatrix3(_matrix);

	_offset.Normalize((*_points)[id]->_dir);
	_offset.ScaleInPlace((*_points)[id]->_length * 0.5f);
	_noffset.Negate(_offset);
	
	if(firstcap)
	{
		_xf.SetPosX((*_points)[id]->_pos.GetX()+_noffset.GetX());
		_xf.SetPosY((*_points)[id]->_pos.GetY()+_noffset.GetY());
		_xf.SetPosZ((*_points)[id]->_pos.GetZ()+_noffset.GetZ());

		for(ULONG j=0;j<_subdiv;j++)
		{
			_p.MulByTransformation(_section[j],_xf);
			_vertices.push_back(CVector3f((float)_p.GetX(),(float)_p.GetY(),(float)_p.GetZ()));
			_nbv++;
		}
	}

	_xf.SetPosX((*_points)[id]->_pos.GetX()+_offset.GetX());
	_xf.SetPosY((*_points)[id]->_pos.GetY()+_offset.GetY());
	_xf.SetPosZ((*_points)[id]->_pos.GetZ()+_offset.GetZ());

	if(hasnext==true)
	{
		DirectionToRotation(tangent,up);
		_xf.SetRotationFromMatrix3(_matrix);
	}

	ULONG sub =0;

	if(_subdiv>2)
	{
		for(ULONG j=0;j<_subdiv;j++)
		{
			if(!closed) sub = 0;
			else sub =  chainsize * _subdiv;
			
			ULONG s = (ULONG)_vertices.size();
			if(j==_subdiv-1)
			{
				_polygons.Add(s - _subdiv - _subdiv + 1);
				_polygons.Add(s - _subdiv + 1-sub);
			}
			else
			{
				_polygons.Add(s - _subdiv + 1);
				_polygons.Add(s + 1 - sub);
			}
			_polygons.Add(s-sub);
			_polygons.Add(s - _subdiv);

			_polygons.Add(-2);
			_nbp+=5;

			//if(!closed)
			//{
				_p.MulByTransformation(_section[j],_xf);
				//_vertices.Add(_p);
				_vertices.push_back(CVector3f((float)_p.GetX(),(float)_p.GetY(),(float)_p.GetZ()));
			//}
			_nbv++;
		}
	}
	else
	{
		if(!closed) sub = 0;
		else sub = chainsize * _subdiv;
		
		ULONG s = (ULONG)_vertices.size();
		_polygons.Add(s - _subdiv + 1);
		_polygons.Add(s + 1 - sub);
		_polygons.Add(s - sub);
		_polygons.Add(s - _subdiv);
		_polygons.Add(-2);
		_nbp+=5;

		if(!closed)
		{
			_p.MulByTransformation(_section[0],_xf);
			_vertices.push_back(CVector3f((float)_p.GetX(),(float)_p.GetY(),(float)_p.GetZ()));
			_p.MulByTransformation(_section[1],_xf);
			_vertices.push_back(CVector3f((float)_p.GetX(),(float)_p.GetY(),(float)_p.GetZ()));
			_nbv+=2;
		}
	}

	// cap line
	if(!hasnext && _subdiv>2)
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

	_matrix.Set(v1.GetX(), v1.GetY(), v1.GetZ(), dir.GetX(), dir.GetY(), dir.GetZ(), v2.GetX(), v2.GetY(), v2.GetZ());
}
