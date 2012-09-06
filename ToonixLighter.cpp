//Toonix Lighter
//------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixData.h"
#include "ToonixLight.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_ToonixData = 0,
	ID_IN_ViewPosition = 1,
	ID_IN_LightPosition = 2,
	ID_IN_LightBias = 3,
	ID_IN_LightDistance = 4,
	ID_IN_Revert = 5,
	ID_IN_Push = 6,
	ID_IN_ByTriangle = 7,
	ID_G_100 = 100,
	ID_OUT_Vertices= 201,
	ID_OUT_Polygons=202,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};

void CleanUpLighterUserData(Context& in_ctxt)
{
	if(!in_ctxt.GetUserData().IsEmpty())
	{
		TXLight* light = (TXLight*)(CValue::siPtrType)in_ctxt.GetUserData( );
		delete light;
		in_ctxt.PutUserData((CValue::siPtrType)NULL);
	}
}

bool GetLighterDirtyState(ICENodeContext& in_ctxt )
{
	CICEPortState DataState( in_ctxt, ID_IN_ToonixData );
	bool dataDirty = DataState.IsDirty( CICEPortState::siAnyDirtyState );

	CICEPortState LightState( in_ctxt, ID_IN_LightPosition );
	bool lightDirty = LightState.IsDirty( CICEPortState::siAnyDirtyState );
	LightState.ClearState();

	CICEPortState ViewState( in_ctxt, ID_IN_ViewPosition );
	bool viewDirty = ViewState.IsDirty( CICEPortState::siAnyDirtyState );
	ViewState.ClearState();

	CICEPortState RevertState( in_ctxt, ID_IN_Revert );
	bool revertDirty = RevertState.IsDirty( CICEPortState::siAnyDirtyState );
	RevertState.ClearState();

	CICEPortState BiasState( in_ctxt, ID_IN_LightBias );
	bool biasDirty = BiasState.IsDirty( CICEPortState::siAnyDirtyState );
	BiasState.ClearState();

	CICEPortState DistanceState( in_ctxt, ID_IN_LightDistance );
	bool distanceDirty = DistanceState.IsDirty( CICEPortState::siAnyDirtyState );
	DistanceState.ClearState();

	CICEPortState PushState( in_ctxt, ID_IN_LightDistance );
	bool pushDirty = PushState.IsDirty( CICEPortState::siAnyDirtyState );
	PushState.ClearState();

	CICEPortState ByTriangleState( in_ctxt, ID_IN_ByTriangle );
	bool byTriangleDirty = ByTriangleState.IsDirty( CICEPortState::siAnyDirtyState );
	ByTriangleState.ClearState();


	return(dataDirty||viewDirty||lightDirty||revertDirty||biasDirty||distanceDirty||byTriangleDirty);
}


CStatus RegisterToonixLighter( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixLighter",L"ToonixLighter");

	CStatus st;
	st = nodeDef.PutColor(ToonixNodeR,ToonixNodeG,ToonixNodeB);
	st.AssertSucceeded( ) ;


	// Add custom types definition.
	st = nodeDef.DefineCustomType(L"ToonixData",L"ToonixData",L"ToonixData",ToonixLineR,ToonixLineG,ToonixLineB);
	st.AssertSucceeded( ) ;

	// Add input ports and groups.
	st = nodeDef.AddPortGroup(ID_G_100);
	st.AssertSucceeded( ) ;

	CStringArray ToonixLineCustomType(1);
	ToonixLineCustomType[0] = L"ToonixData";

	st = nodeDef.AddInputPort(ID_IN_ToonixData,ID_G_100,ToonixLineCustomType,siICENodeStructureSingle,siICENodeContextSingleton,L"ToonixData",L"ToonixData",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_ViewPosition,ID_G_100,siICENodeDataVector3,siICENodeStructureSingle,siICENodeContextSingleton,L"ViewPosition",L"ViewPosition");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_LightPosition,ID_G_100,siICENodeDataVector3,siICENodeStructureAny,siICENodeContextSingleton,L"LightPosition",L"LightPosition");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_LightBias,ID_G_100,siICENodeDataFloat,siICENodeStructureAny,siICENodeContextSingleton,L"LightBias",L"LightBias",0.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_LightDistance,ID_G_100,siICENodeDataFloat,siICENodeStructureAny,siICENodeContextSingleton,L"LightDistance",L"LightDistance",100.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Revert,ID_G_100,siICENodeDataBool,siICENodeStructureSingle,siICENodeContextSingleton,L"Revert",L"Revert",false);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Push,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"Push",L"Push",0.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_ByTriangle,ID_G_100,siICENodeDataBool,siICENodeStructureSingle,siICENodeContextSingleton,L"ByTriangle",L"ByTriangle",true);
	st.AssertSucceeded( ) ;

	// Add output ports.
	st = nodeDef.AddOutputPort(ID_OUT_Vertices,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextSingleton,L"Vertices",L"Vertices",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddOutputPort(ID_OUT_Polygons,siICENodeDataLong,siICENodeStructureArray,siICENodeContextSingleton,L"Polygons",L"Polygons",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Toonix");

	return CStatus::OK;
}

SICALLBACK ToonixLighter_BeginEvaluate( ICENodeContext& in_ctxt )
{
	TXLight* light = NULL;
	CValue userData = in_ctxt.GetUserData();

	if(userData.IsEmpty())
	{
		// Build a new Light Object
		light = new TXLight();

		// Get the input TXGeometry
		CDataArrayCustomType ToonixData( in_ctxt, ID_IN_ToonixData );

		CDataArrayCustomType::TData* pBufferToonixData;
		ULONG nSizeToonixData;
		ToonixData.GetData( 0,(const CDataArrayCustomType::TData**)&pBufferToonixData, nSizeToonixData );
		TXData* data = (TXData*)pBufferToonixData;
		light->_geom = data->_geom;


		in_ctxt.PutUserData((CValue::siPtrType)light);
	}
	else
	{
		light = (TXLight*)(CValue::siPtrType)in_ctxt.GetUserData( );
	}

	if(GetLighterDirtyState(in_ctxt))
	{
		//Application().LogMessage(L"Toonix Lighter >State Dirty...");
		// Get lights positions used for light detection
		light->_lights.clear();
		
		siICENodeDataType inPortType;
		siICENodeStructureType inPortStruct;
		siICENodeContextType inPortContext;

		in_ctxt.GetPortInfo( ID_IN_LightPosition, inPortType, inPortStruct, inPortContext );
		if ( inPortStruct == XSI::siICENodeStructureSingle )
		{
			//Application().LogMessage(L"ONE Light");
			CDataArrayVector3f lightPointData( in_ctxt, ID_IN_LightPosition );
			light->_lights.push_back(lightPointData[0]);
			light->_nbl = 1;
		}
		else if ( inPortStruct == XSI::siICENodeStructureArray )
		{
			//Application().LogMessage(L"MULTI Lights");
			CDataArray2DVector3f lightPointsData( in_ctxt, ID_IN_LightPosition );
			CDataArray2DVector3f::Accessor lightPointData = lightPointsData[0];
			light->_nbl = lightPointData.GetCount();

			for(LONG l=0;l<light->_nbl;l++)
			{
				light->_lights.push_back(lightPointData[l]);
			}
		}

		light->_bias.resize(light->_nbl);

		in_ctxt.GetPortInfo( ID_IN_LightBias, inPortType, inPortStruct, inPortContext );
		if ( inPortStruct == XSI::siICENodeStructureSingle )
		{
			CDataArrayFloat lightBiasData( in_ctxt, ID_IN_LightBias );
			light->_bias[0] = lightBiasData[0];
		}
		else if ( inPortStruct == XSI::siICENodeStructureArray )
		{
			CDataArray2DFloat lightPointsData( in_ctxt, ID_IN_LightBias );
			CDataArray2DFloat::Accessor lightPointData = lightPointsData[0];
			ULONG nbl = lightPointData.GetCount();

			for(LONG l=0;l<nbl&&l<light->_nbl;l++)
			{
				light->_bias[l] = lightPointData[l];
			}
		}

		light->_distance.resize(light->_nbl);

		in_ctxt.GetPortInfo( ID_IN_LightDistance, inPortType, inPortStruct, inPortContext );
		if ( inPortStruct == XSI::siICENodeStructureSingle )
		{
			CDataArrayFloat lightDistanceData( in_ctxt, ID_IN_LightDistance );
			light->_distance[0] = lightDistanceData[0];
		}
		else if ( inPortStruct == XSI::siICENodeStructureArray )
		{
			CDataArray2DFloat lightDistancesData( in_ctxt, ID_IN_LightDistance );
			CDataArray2DFloat::Accessor lightDistanceData = lightDistancesData[0];
			ULONG nbl = lightDistanceData.GetCount();

			for(LONG l=0;l<nbl&&l<light->_nbl;l++)
			{
				light->_distance[l] = lightDistanceData[l];
			}
		}

		CDataArrayBool revertData( in_ctxt, ID_IN_Revert );
		light->_reverse = revertData[0];
		CDataArrayFloat pushData( in_ctxt, ID_IN_Push );
		light->_push = pushData[0];

		CDataArrayVector3f viewData( in_ctxt, ID_IN_ViewPosition );
		light->_view = viewData[0];

		light->Build();

		in_ctxt.PutUserData((CValue::siPtrType)light);
	}
	return CStatus::OK;
}

SICALLBACK ToonixLighter_Evaluate( ICENodeContext& in_ctxt )
{
	// Get User Data
	TXLight* light = (TXLight*)(CValue::siPtrType)in_ctxt.GetUserData( );

	// The current output port being evaluated...
	LONG out_portID = in_ctxt.GetEvaluatedOutputPortID( );

	switch( out_portID )
	{	
		case ID_OUT_Vertices:
		{
			// Get the output port array ...			
			CDataArray2DVector3f outData( in_ctxt );

			CDataArray2DVector3f::Accessor outDataSubArray = outData.Resize(0,light->_nbv);
			for(ULONG n=0;n<light->_nbv;n++)
			{
				outDataSubArray[n].Set(light->_vertices[n].GetX(),light->_vertices[n].GetY(),light->_vertices[n].GetZ());
			}
			
		}
		break;

		case ID_OUT_Polygons:
		{
			// Get the output port array ...			
			CDataArray2DLong outData( in_ctxt );	

			CDataArray2DLong::Accessor outDataSubArray = outData.Resize(0,light->_nbp);
			for(ULONG n=0;n<light->_nbp;n++)
			{
				outDataSubArray[n] = light->_polygons[n];
			}
		}
		break;
	}
	return CStatus::OK;
}

SICALLBACK ToonixLighter_Term( CRef& in_ctxt )
{
	Context ctxt(in_ctxt);
	CleanUpLighterUserData(ctxt);
	return CStatus::OK;
}
