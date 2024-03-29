//Toonix Crease
//------------------------------------------------
#include "Line.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_ToonixData = 0,
	ID_IN_Width = 1,
	ID_IN_Crease = 2,
	ID_IN_BreakAngle = 3,
	ID_IN_Extend = 4,
	ID_IN_FilterPoints = 5,
	ID_G_100 = 100,
	ID_OUT_ToonixLine = 201,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};

bool GetCreaseDirtyState(ICENodeContext& in_ctxt )
{
	CICEPortState DataState( in_ctxt, ID_IN_ToonixData );
	bool dataDirty = DataState.IsDirty( CICEPortState::siAnyDirtyState );

	CICEPortState WidthState( in_ctxt, ID_IN_Width );
	bool widthDirty = WidthState.IsDirty( CICEPortState::siAnyDirtyState );
	WidthState.ClearState();

	CICEPortState CreaseState( in_ctxt, ID_IN_Crease );
	bool creaseDirty = CreaseState.IsDirty( CICEPortState::siAnyDirtyState );
	CreaseState.ClearState();

	Application().LogMessage(L"Get Crease Dirty State : "+(CString)dataDirty+L","+(CString)widthDirty);;
	return(	 dataDirty||widthDirty||creaseDirty);
}


CStatus RegisterToonixCrease( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixCrease",L"ToonixCrease");

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

	st = nodeDef.AddInputPort(ID_IN_Crease,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"Crease",L"Crease",45.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_BreakAngle,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"BreakAngle",L"BreakAngle",90.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Extend,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"Extend",L"Extend",0.0);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_FilterPoints,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"FilterPoints",L"FilterPoints",0.0);
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

SICALLBACK ToonixCrease_Evaluate( ICENodeContext& in_ctxt )
{
	CValue userData = in_ctxt.GetUserData();
	TXLine* line = (TXLine*)(CValue::siPtrType)in_ctxt.GetUserData( );

	// Get the input TXGeometry
	CDataArrayCustomType ToonixData( in_ctxt, ID_IN_ToonixData );

	CDataArrayCustomType::TData* pBufferToonixData;
	ULONG nSizeToonixData;
	ToonixData.GetData( 0,(const CDataArrayCustomType::TData**)&pBufferToonixData, nSizeToonixData );
	TXData* data = (TXData*)pBufferToonixData;

	//Empty Out if input is invalid
	if(!data){line->EmptyData();return CStatus::OK;}

	//Get underlying TXGeometry
	line->m_geom = data->m_geom;

	if(GetCreaseDirtyState(in_ctxt))
	{
		line->ClearChains();
		// Get the parameters value
		CDataArrayFloat widthData(in_ctxt, ID_IN_Width);
		CDataArrayFloat creaseData(in_ctxt, ID_IN_Crease);
		CDataArrayFloat breakData(in_ctxt, ID_IN_BreakAngle);
		CDataArrayFloat filterData(in_ctxt, ID_IN_FilterPoints);
		CDataArrayFloat extendData(in_ctxt, ID_IN_Extend);

		//pass it to TXLine object
		line->m_width = widthData[0];
		line->m_crease = (float)DegreesToRadians(creaseData[0]);
		line->m_break = (float)DegreesToRadians(breakData[0]);
		line->m_filterpoints = filterData[0];
		line->m_extend = extendData[0];

		line->GetCreases();
		line->Build(CREASE);
	}

	// The current output port being evaluated...
	LONG out_portID = in_ctxt.GetEvaluatedOutputPortID( );

	switch( out_portID )
	{	
		case ID_OUT_ToonixLine :
		{
			// Get the output port array ...			
			CDataArrayCustomType outData( in_ctxt );
			
			XSI::CDataArrayCustomType::TData* pOutData = outData.Resize(0,sizeof(TXLine));
			::memcpy( pOutData, line, sizeof(TXLine) );   
		}
		break;
	}
	//in_ctxt.PutUserData((CValue::siPtrType)line);
	return CStatus::OK;
}

SICALLBACK ToonixCrease_Init( CRef& in_ctxt )
{
	InitTXLineData(in_ctxt);
	return CStatus::OK;
}

SICALLBACK ToonixCrease_Term( CRef& in_ctxt )
{
	CleanTXLineData(in_ctxt);
	return CStatus::OK;
}
