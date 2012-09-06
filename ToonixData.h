// ToonixData.h
//-------------------------------------------------
#ifndef _TOONIX_DATA_H_
#define _TOONIX_DATA_H_

#include "ToonixRegister.h"
#include "ToonixGeometry.h"
#include "ToonixLine.h"
#include "ToonixMesh.h"
#include "ToonixCamera.h"
//#include "ToonixGrid3D.h"
//#include "ToonixOctree.h"

class TXData
{
public:
	TXData();
	~TXData();
	
	TXGeometry* _geom;
	/*TXGrid3D* _grid;*/
	/*TXOctree* _octree;*/
	TXCamera* _camera;

	bool _useoctree;
	bool _culledges;

	void ClearVisible();
	void SetCulling(bool cull){_culledges = cull;_geom->_culledges = cull;};
	void SetCamera(CMatrix4f mat,float fov,float aspect, float near, float far)
		{_camera->Set(mat,fov, aspect, near, far);};
	void Init(CICEGeometry& geom, bool topodirty){if(!_geom->_init || topodirty)_geom->GetCachedMeshData(geom);};
	void GetVisibilityData();
	void SetAllVisible();
	void Update(CICEGeometry& geom);
	void Update(CICEGeometry& geom, bool useoctree);

};

extern TXCamera* camera;

#endif