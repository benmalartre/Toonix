//Toonix Get Data
//------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixData.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_Geometry = 0,
	ID_IN_CameraCulling = 1,
	ID_IN_CameraGlobal = 2,
	ID_IN_CameraFov = 3,
	ID_IN_CameraAspect = 4,
	ID_IN_CameraNear = 5,
	ID_IN_CameraFar = 6,
	ID_IN_UseOctree = 7,
	ID_G_100 = 100,
	ID_OUT_ToonixData = 201,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};

CStatus RegisterToonixGetData( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixGetData",L"ToonixGetData");

	CStatus st;
	st = nodeDef.PutColor(ToonixNodeR,ToonixNodeG,ToonixNodeB);
	st.AssertSucceeded( ) ;

	st = nodeDef.PutThreadingModel(siICENodeSingleThreading);
	st.AssertSucceeded( ) ;


	// Add custom types definition.
	st = nodeDef.DefineCustomType(L"ToonixData",L"ToonixData",L"ToonixData",ToonixDataR,ToonixDataG,ToonixDataB);
	st.AssertSucceeded( ) ;


	// Add input ports and groups.
	st = nodeDef.AddPortGroup(ID_G_100);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Geometry,ID_G_100,siICENodeDataGeometry,siICENodeStructureSingle,siICENodeContextSingleton,L"Geometry",L"Geometry");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_CameraCulling,ID_G_100,siICENodeDataBool,siICENodeStructureSingle,siICENodeContextSingleton,L"CameraCulling",L"CameraCulling",false);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_CameraGlobal,ID_G_100,siICENodeDataMatrix44,siICENodeStructureSingle,siICENodeContextSingleton,L"CameraGlobal",L"CameraGlobal");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_CameraFov,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"CameraFov",L"CameraFov");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_CameraAspect,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"CameraAspect",L"CameraAspect");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_CameraNear,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"CameraNear",L"CameraNear",0.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_CameraFar,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"CameraFar",L"CameraFar",1000.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_UseOctree,ID_G_100,siICENodeDataBool,siICENodeStructureSingle,siICENodeContextSingleton,L"UseOctree",L"UseOctree",false);
	st.AssertSucceeded( ) ;

	// Add output ports.
	CStringArray ToonixDataCustomType(1);
	ToonixDataCustomType[0] = L"ToonixData";

	st = nodeDef.AddOutputPort(ID_OUT_ToonixData,ToonixDataCustomType,siICENodeStructureSingle,siICENodeContextSingleton,L"ToonixData",L"ToonixData",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Toonix");

	return CStatus::OK;
}

int GetToonixDataDirtyState(ICENodeContext& in_ctxt, bool culling)
{
	CICEPortState GeomState( in_ctxt, ID_IN_Geometry );
	bool geomDirty = GeomState.IsDirty( CICEPortState::siAnyDirtyState );

	CICEPortState CullingState( in_ctxt, ID_IN_CameraCulling );
	bool cullingDirty = CullingState.IsDirty( CICEPortState::siAnyDirtyState );
	CullingState.ClearState();

	if(!culling || geomDirty)return ((int)(geomDirty||cullingDirty));

	CICEPortState CameraGlobalState( in_ctxt, ID_IN_CameraGlobal );
	bool globalDirty = CameraGlobalState.IsDirty( CICEPortState::siAnyDirtyState );
	CameraGlobalState.ClearState();

	CICEPortState CameraFovState( in_ctxt, ID_IN_CameraFov);
	bool fovDirty = CameraFovState.IsDirty( CICEPortState::siAnyDirtyState );
	CameraFovState.ClearState();

	CICEPortState CameraAspectState( in_ctxt, ID_IN_CameraAspect );
	bool aspectDirty = CameraAspectState.IsDirty( CICEPortState::siAnyDirtyState );
	CameraAspectState.ClearState();

	CICEPortState CameraNearState( in_ctxt, ID_IN_CameraNear);
	bool nearDirty = CameraNearState.IsDirty( CICEPortState::siAnyDirtyState );
	CameraNearState.ClearState();

	CICEPortState CameraFarState( in_ctxt, ID_IN_CameraFar );
	bool farDirty = CameraFarState.IsDirty( CICEPortState::siAnyDirtyState );
	CameraFarState.ClearState();

	if(globalDirty||fovDirty||aspectDirty||nearDirty||farDirty)return 2;
	else return 0;
}

XSIPLUGINCALLBACK CStatus ToonixGetData_Evaluate( ICENodeContext& in_ctxt )
{
	// Get User Data
	TXData* data = (TXData*)(CValue::siPtrType)in_ctxt.GetUserData( );

	// The current output port being evaluated...
	LONG out_portID = in_ctxt.GetEvaluatedOutputPortID( );

	switch( out_portID )
	{	
		case ID_OUT_ToonixData :
		{
			//Application().LogMessage(L"Toonix Get Data :: Nb Crease Edges : "+(CString)(ULONG)data->_geom->_creases.size());
			// Get the output port array ...			
			CDataArrayCustomType outData( in_ctxt );
			
			XSI::CDataArrayCustomType::TData* pOutData = outData.Resize(0,sizeof(TXData));
			::memcpy( pOutData, data, sizeof(TXData) );   

		}break;
	}

	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus ToonixGetData_BeginEvaluate( ICENodeContext& in_ctxt )
{
	// Get User Data
	TXData* data = (TXData*)(CValue::siPtrType)in_ctxt.GetUserData( );
	CDataArrayBool cullingData(in_ctxt, ID_IN_CameraCulling);
	int dirtyState = GetToonixDataDirtyState(in_ctxt,cullingData[0]);
	
	if(!dirtyState)return CStatus::OK;

	// Get geometry object from the input port
	CICEGeometry geom( in_ctxt, ID_IN_Geometry );

	// Get Camera Datas
	CDataArrayMatrix4f matrixData(in_ctxt, ID_IN_CameraGlobal);
	CDataArrayFloat fovData(in_ctxt, ID_IN_CameraFov);
	CDataArrayFloat aspectData(in_ctxt, ID_IN_CameraAspect);
	CDataArrayFloat nearData(in_ctxt, ID_IN_CameraNear);
	CDataArrayFloat farData(in_ctxt, ID_IN_CameraFar);
	CDataArrayBool octreeData(in_ctxt, ID_IN_UseOctree);

	data->SetCulling(cullingData[0]);
	data->SetCamera(matrixData[0],fovData[0],aspectData[0],nearData[0],farData[0]);

	if(dirtyState == 1)
	{
		bool bTopologyDirtyState = geom.IsDirty( CICEGeometry::siTopologyDirtyState );
		geom.ClearState();

		data->Init(geom, bTopologyDirtyState);
	}

	data->Update(geom, octreeData[0]);

	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus ToonixGetData_Init( CRef& in_ctxt )
{
	Context ctxt(in_ctxt);
	TXData* data = new TXData();
	ctxt.PutUserData( (CValue::siPtrType)data );
	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus ToonixGetData_Term( CRef& in_ctxt )
{
	Context ctxt(in_ctxt);
	if(!ctxt.GetUserData().IsEmpty())
	{
		TXData* data = (TXData*)(CValue::siPtrType)ctxt.GetUserData( );
		delete data;
		ctxt.PutUserData((CValue)NULL);
	}
	return CStatus::OK;
}

