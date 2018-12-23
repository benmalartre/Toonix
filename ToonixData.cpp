// Toonix Data
//--------------------------------------------
#include "ToonixData.h"
#include "ToonixGLDrawer.h"

//TXGLDrawer _drawer;

TXData::TXData()
{
	m_geom = new TXGeometry();
	/*_grid = new TXGrid3D();
	_octree = new TXOctree();*/
	m_camera = new TXCamera();
};

TXData::~TXData()
{
	delete m_camera;
	/*delete _octree;
	delete _grid;*/
	delete m_geom;
}


void TXData::Update(CICEGeometry& geom)
{
	m_geom->GetPointPosition(geom);
	//_useoctree = false;
	m_geom->GetRuntimeMeshData(geom);
	GetVisibilityData();
}

void TXData::Update(CICEGeometry& geom, bool useoctree)
{
	m_geom->GetPointPosition(geom);
	
	/*
	_useoctree = useoctree;
	if(_useoctree)
	{
		_dualmesh = TXOctree::BuildTree(_geom);
		//_octree->BuildTree(_geom);
		Application().LogMessage(L"Octree size : "+(CString)_dualmesh->GetSize());
	}
	*/
	
	m_geom->GetRuntimeMeshData(geom);
	GetVisibilityData();
	//_drawer.SetCamera(_camera);
	//_drawer.SetGeometry(_geom);
}

void TXData::GetVisibilityData()
{
	m_geom->m_view = m_camera->m_pos;

	if(!m_culledges)
	{
		m_geom->SetAllVisible();
		return;
	}

	else
	{
		m_geom->GetVisible(m_camera);
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