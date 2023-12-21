#ifndef _TOONIX_DATA_H_
#define _TOONIX_DATA_H_

#include "Register.h"
#include "Geometry.h"

class TXData
{
public:
	TXData();
	~TXData();
	
	TXGeometry* m_geom;
	TXCamera* m_camera;

	bool m_useoctree;
	bool m_culledges;

	void ClearVisible();
	void SetCulling(bool cull){m_culledges = cull;m_geom->m_culledges = cull;};
	
	void Init(CICEGeometry& geom, bool topodirty){if(!m_geom->m_init || topodirty)m_geom->GetCachedMeshData(geom);};
	void GetVisibilityData();
	void SetAllVisible();
	void Update(CICEGeometry& geom);
	void Update(CICEGeometry& geom, bool useoctree);

	void SetCamera(CMatrix4f& mat,float fov,float aspect, float nearplane, float farplane){m_camera->Set(mat,fov, aspect, nearplane, farplane);};
};

extern TXCamera* camera;

#endif