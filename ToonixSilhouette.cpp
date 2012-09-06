//Toonix Silhouette
//------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixData.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_ToonixData = 0,
	ID_IN_ViewPosition = 1,
	ID_IN_ViewBias = 2,
	ID_IN_Width = 3,
	ID_IN_BreakAngle = 4,
	ID_IN_Extend = 5,
	ID_IN_FilterPoints = 6,
	ID_IN_SmoothSilhouette = 7,
	ID_G_100 = 100,
	ID_OUT_ToonixLine = 201,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};

CStatus RegisterToonixSilhouette( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixSilhouette",L"ToonixSilhouette");

	CStatus st;
	st = nodeDef.PutColor(ToonixNodeR,ToonixNodeG,ToonixNodeB);
	st.AssertSucceeded( ) ;

	st = nodeDef.PutThreadingModel(siICENodeSingleThreading);
	st.AssertSucceeded( ) ;

	// Add custom types definition.
	st = nodeDef.DefineCustomType(L"ToonixData",L"ToonixData",L"ToonixData",ToonixDataR,ToonixDataG,ToonixDataB);
	st.AssertSucceeded( ) ;
	st = nodeDef.DefineCustomType(L"ToonixLine",L"ToonixLine",L"ToonixLine",ToonixLineR,ToonixLineG,ToonixLineB);
	st.AssertSucceeded( ) ;

	// Add input ports and groups.
	st = nodeDef.AddPortGroup(ID_G_100);
	st.AssertSucceeded( ) ;

	CStringArray ToonixDataCustomType(1);
	ToonixDataCustomType[0] = L"ToonixData";

	st = nodeDef.AddInputPort(ID_IN_ToonixData,ID_G_100,ToonixDataCustomType,siICENodeStructureSingle,siICENodeContextSingleton,L"ToonixData",L"ToonixData",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_ViewPosition,ID_G_100,siICENodeDataVector3,siICENodeStructureAny,siICENodeContextSingleton,L"ViewPoint",L"ViewPoint");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_ViewBias,ID_G_100,siICENodeDataFloat,siICENodeStructureAny,siICENodeContextSingleton,L"ViewBias",L"ViewBias",0.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Width,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"Width",L"Width",0.1);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_BreakAngle,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"BreakAngle",L"BreakAngle",90.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Extend,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"Extend",L"Extend",0.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_FilterPoints,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"FilterPoints",L"FilterPoints",0.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_SmoothSilhouette,ID_G_100,siICENodeDataBool,siICENodeStructureSingle,siICENodeContextSingleton,L"SmoothSilhouette",L"SmoothSilhouette",true);
	st.AssertSucceeded( ) ;

	// Add output ports.
	CStringArray ToonixLineCustomType(1);
	ToonixLineCustomType[0] = L"ToonixLine";

	st = nodeDef.AddOutputPort(ID_OUT_ToonixLine,ToonixLineCustomType,siICENodeStructureSingle,siICENodeContextSingleton,L"ToonixLine",L"ToonixLine",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Toonix");

	return CStatus::OK;
}

SICALLBACK ToonixSilhouette_Evaluate( ICENodeContext& in_ctxt )
{
	// Get User Data
	TXLine* line = (TXLine*)(CValue::siPtrType)in_ctxt.GetUserData( );
	
	// Get the output port array ...			
	CDataArrayCustomType outData( in_ctxt );
	
	XSI::CDataArrayCustomType::TData* pOutData = outData.Resize(0,sizeof(TXLine));
	::memcpy( pOutData, line, sizeof(TXLine) );   

	return CStatus::OK;
}

SICALLBACK ToonixSilhouette_BeginEvaluate( ICENodeContext& in_ctxt )
{
	CValue userData = in_ctxt.GetUserData();
	TXLine* line = (TXLine*)(CValue::siPtrType)in_ctxt.GetUserData( );;

	// Get the input TXData
	CDataArrayCustomType ToonixData( in_ctxt, ID_IN_ToonixData );

	CDataArrayCustomType::TData* pBufferToonixData;
	ULONG nSizeToonixData;
	ToonixData.GetData( 0,(const CDataArrayCustomType::TData**)&pBufferToonixData, nSizeToonixData );
	TXData* data = (TXData*)pBufferToonixData;

	//Empty Out if input is invalid
	if(!data){line->EmptyData();return CStatus::OK;}
	
	// Get underlying TXGeometry
	line->_geom = data->_geom;
	
	// Get View Position and optionnal lights positions used for silhouette detection
	line->_eye.clear();
	
	siICENodeDataType inPortType;
	siICENodeStructureType inPortStruct;
	siICENodeContextType inPortContext;

	in_ctxt.GetPortInfo( ID_IN_ViewPosition, inPortType, inPortStruct, inPortContext );
	if ( inPortStruct == XSI::siICENodeStructureSingle )
	{
		CDataArrayVector3f viewPointData( in_ctxt, ID_IN_ViewPosition );
		line->_eye.push_back(viewPointData[0]);
		line->_nbv = 1;
	}
	else if ( inPortStruct == XSI::siICENodeStructureArray )
	{
		CDataArray2DVector3f viewPointsData( in_ctxt, ID_IN_ViewPosition );
		CDataArray2DVector3f::Accessor viewPointData = viewPointsData[0];
		line->_nbv = viewPointData.GetCount();

		for(ULONG v=0;v<line->_nbv;v++)
		{
			line->_eye.push_back(viewPointData[v]);
		}
	}

	// Get View Bias used for silhouette detection
	line->_bias.Resize(line->_nbv);
	in_ctxt.GetPortInfo( ID_IN_ViewBias, inPortType, inPortStruct, inPortContext );
	if ( inPortStruct == XSI::siICENodeStructureSingle )
	{
		CDataArrayFloat viewBias( in_ctxt, ID_IN_ViewBias );
		for(ULONG v=0;v<line->_nbv;v++)
		{
			line->_bias[v] = viewBias[0];
		}
		
	}
	else if ( inPortStruct == XSI::siICENodeStructureArray )
	{
		CDataArray2DFloat viewBiasData( in_ctxt, ID_IN_ViewBias );
		CDataArray2DFloat::Accessor viewBias = viewBiasData[0];
		for(ULONG v=0;v<line->_nbv;v++)
		{
			line->_bias[v] = viewBias[v];
		}
	}
	// Get the parameters value
	CDataArrayFloat widthData(in_ctxt, ID_IN_Width);
	CDataArrayFloat breakData(in_ctxt, ID_IN_BreakAngle);
	CDataArrayFloat filterData(in_ctxt, ID_IN_FilterPoints);
	CDataArrayFloat extendData(in_ctxt, ID_IN_Extend);
	CDataArrayBool smoothData(in_ctxt,ID_IN_SmoothSilhouette);

	// Pass them to line object
	line->_width = widthData[0];
	line->_break = (float)DegreesToRadians(breakData[0]);
	line->_filterpoints = filterData[0];
	line->_extend = extendData[0];
	line->_smoothsilhouette = smoothData[0];

	line->ClearChains();

	for(int v=0;v<line->_eye.size();v++)
	{
		line->GetSilhouettes(v);
		line->Build(v);
	}

	return CStatus::OK;
}

SICALLBACK ToonixSilhouette_Init( CRef& in_ctxt )
{
	Context ctxt(in_ctxt);
	// Build a new Line Object
	TXLine* line = new TXLine();
	ctxt.PutUserData((CValue::siPtrType)line);
	return CStatus::OK;
}

SICALLBACK ToonixSilhouette_Term( CRef& in_ctxt )
{
	Context ctxt(in_ctxt);
	if(!ctxt.GetUserData().IsEmpty())
	{
		TXLine* line = (TXLine*)(CValue::siPtrType)ctxt.GetUserData( );
		delete line;
		ctxt.PutUserData((CValue::siPtrType)NULL);
	}
	return CStatus::OK;
}
