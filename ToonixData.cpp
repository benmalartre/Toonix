// Toonix Data
//--------------------------------------------
#include "ToonixData.h"
#include "ToonixGLDrawer.h"

//TXGLDrawer _drawer;

TXData::TXData()
{
	_geom = new TXGeometry();
	/*_grid = new TXGrid3D();
	_octree = new TXOctree();*/
	_camera = new TXCamera();
};

TXData::~TXData()
{
	delete _camera;
	/*delete _octree;
	delete _grid;*/
	delete _geom;
}

void TXData::Update(CICEGeometry& geom)
{
	_geom->GetPointPosition(geom);
	//_useoctree = false;
	_geom->GetRuntimeMeshData(geom);
	GetVisibilityData();
}

void TXData::Update(CICEGeometry& geom, bool useoctree)
{
	_geom->GetPointPosition(geom);
	/*
	_useoctree = useoctree;
	if(_useoctree)
	{
		_octree->BuildTree(_geom);
		Application().LogMessage(L"Octree size : "+(CString)_octree->GetSize());
	}
	*/
	_geom->GetRuntimeMeshData(geom);
	GetVisibilityData();
	_drawer.SetCamera(_camera);
	_drawer.SetGeometry(_geom);
}

void TXData::GetVisibilityData()
{
	_geom->_view = _camera->_pos;

	if(!_culledges)
	{
		_geom->SetAllVisible();
		return;
	}

	else
	{
		_geom->GetVisible(_camera);
	}
	/*
	if(_useoctree)
	{

		_geom->ClearVisible();
		//Test Octree Cells Visibility Recursively
		_geom->_visibleedges->clear();
		_octree->GetVisibleEdges(_camera,_geom->_visibleedges);
		
		std::vector<TXEdge*>::iterator it = _geom->_visibleedges->begin();
		for(;it<_geom->_visibleedges->end();it++)
		{
			if(!(*it)->_start->_visible)
			{
				_geom->_visiblevertices->push_back((*it)->_start);
				(*it)->_start->SetVisible(true);
			}
			if(!(*it)->_end->_visible)
			{
				_geom->_visiblevertices->push_back((*it)->_end);
				(*it)->_end->SetVisible(true);

			}
		}
		_geom->_nbve = _geom->_visibleedges->size();
		_geom->_nbvv =  _geom->_visiblevertices->size();
		return;
	}
	
	if(!_geom->GetBBoxVisibility(_camera))
	{
		_geom->GetVisible(_camera);
	}
	*/
}