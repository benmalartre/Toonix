//Toonix Intersection
//------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixData.h"
#include "ToonixCommon.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_ToonixData = 0,
	ID_IN_Width = 1,
	ID_IN_BreakAngle = 2,
	ID_IN_Extend = 3,
	ID_IN_FilterPoints = 4,
	ID_IN_OGLDraw = 5,
	ID_G_100 = 100,
	ID_OUT_ToonixLine = 201,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};

CStatus RegisterToonixIntersection( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixIntersection",L"ToonixIntersection");

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

	st = nodeDef.AddInputPort(ID_IN_Width,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"Width",L"Width",0.1);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_BreakAngle,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"BreakAngle",L"BreakAngle",90.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Extend,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"Extend",L"Extend",0.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_FilterPoints,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"FilterPoints",L"FilterPoints",0.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_OGLDraw,ID_G_100,siICENodeDataBool,siICENodeStructureSingle,siICENodeContextSingleton,L"OGLDraw",L"OGLDraw",true);
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

SICALLBACK ToonixIntersection_Evaluate( ICENodeContext& in_ctxt )
{
	// Get User Data
	TXLine* line = (TXLine*)(CValue::siPtrType)in_ctxt.GetUserData( );
	
	// Get the output port array ...			
	CDataArrayCustomType outData( in_ctxt );
	
	XSI::CDataArrayCustomType::TData* pOutData = outData.Resize(0,sizeof(TXLine));
	::memcpy( pOutData, line, sizeof(TXLine) );   

	return CStatus::OK;
}

SICALLBACK ToonixIntersection_BeginEvaluate( ICENodeContext& in_ctxt )
{
	CValue userData = in_ctxt.GetUserData();
	TXLine* line = (TXLine*)(CValue::siPtrType)in_ctxt.GetUserData( );

	// Get the input TXData
	CDataArrayCustomType ToonixData( in_ctxt, ID_IN_ToonixData );

	CDataArrayCustomType::TData* pBufferToonixData;
	ULONG nSizeToonixData;
	ToonixData.GetData( 0,(const CDataArrayCustomType::TData**)&pBufferToonixData, nSizeToonixData );
	TXData* data = (TXData*)pBufferToonixData;

	//Empty Out if input is invalid
	if(!data){line->EmptyData();return CStatus::OK;}
	
	// Get underlying TXGeometry
	line->m_geom = data->m_geom;
	
	// Get the parameters value
	CDataArrayFloat widthData(in_ctxt, ID_IN_Width);
	CDataArrayFloat breakData(in_ctxt, ID_IN_BreakAngle);
	CDataArrayFloat filterData(in_ctxt, ID_IN_FilterPoints);
	CDataArrayFloat extendData(in_ctxt, ID_IN_Extend);
	CDataArrayBool ogldrawData(in_ctxt,ID_IN_OGLDraw);

	// Pass them to line object
	line->m_width = widthData[0];
	line->m_break = (float)DegreesToRadians(breakData[0]);
	line->m_filterpoints = filterData[0];
	line->m_extend = extendData[0];
	line->m_ogldraw = ogldrawData[0];

	line->ClearChains();
	Application().LogMessage(L"Intersection Geometry Is Dirty...");
	line->GetIntersections();
	line->Build(INTERSECTION);

	return CStatus::OK;
}

SICALLBACK ToonixIntersection_Init( CRef& in_ctxt )
{
	InitTXLineData(in_ctxt);
	return CStatus::OK;
}

SICALLBACK ToonixIntersection_Term( CRef& in_ctxt )
{
	CleanTXLineData(in_ctxt);
	return CStatus::OK;
}
