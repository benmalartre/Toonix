// ToonixHairs
//------------------------------------------------
#include "Register.h"
#include "Line.h"
#include "Common.h"

enum IDs
{
	ID_IN_HairPosition = 0,
	ID_IN_HairSize = 1,
	ID_IN_HairColor = 2,
	ID_G_100 = 100,
	ID_OUT_Valid = 200,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};


CStatus RegisterToonixHairs( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixHairs",L"ToonixHairs");

	CStatus st;
	st = nodeDef.PutColor(ToonixNodeR,ToonixNodeG,ToonixNodeB);
	st.AssertSucceeded( ) ;

	st = nodeDef.PutThreadingModel(siICENodeSingleThreading);
	st.AssertSucceeded( ) ;

	// Add input ports and groups.
	st = nodeDef.AddPortGroup(ID_G_100);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_HairPosition,ID_G_100,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextComponent0D,L"HairPosition",L"HairPosition",MATH::CVector3f(0.0,0.0,0.0),CValue(),CValue(),ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_HairSize,ID_G_100,siICENodeDataFloat,siICENodeStructureArray,siICENodeContextComponent0D,L"HairSize",L"HairSize",1.0,CValue(),CValue(),ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_HairColor,ID_G_100,siICENodeDataColor4,siICENodeStructureArray,siICENodeContextComponent0D,L"HairColor",L"HairColor",MATH::CColor4f(0.0,0.0,0.0,1.0),CValue(),CValue(),ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	// Add output ports.
	st = nodeDef.AddOutputPort(ID_OUT_Valid,siICENodeDataBool,siICENodeStructureSingle,siICENodeContextComponent0D,L"Valid",L"Valid",ID_UNDEF,ID_UNDEF,ID_UNDEF);
	st.AssertSucceeded( ) ;

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Toonix");

	return CStatus::OK;
}
//SICALLBACK ToonixHairs_BeginEvaluate( ICENodeContext& in_ctxt )

bool ToonixHairs_GetPortInfo(ICENodeContext& in_ctxt, ULONG portID)
{
	siICENodeDataType inPortType;
	siICENodeStructureType inPortStruct;
	siICENodeContextType inPortContext;

	in_ctxt.GetPortInfo( portID, inPortType, inPortStruct, inPortContext );
	if ( inPortStruct == XSI::siICENodeStructureSingle )
		return false;
	else if ( inPortStruct == XSI::siICENodeStructureArray )
		return true;

	return false;
}

SICALLBACK ToonixHairs_Evaluate( ICENodeContext& in_ctxt )
{
	CValue userData = in_ctxt.GetUserData();
	TXLine* line = (TXLine*)(CValue::siPtrType)in_ctxt.GetUserData( );

	if(line->m_chains.size()>0)line->ClearChains();
	
	// The current output port being evaluated...
	ULONG out_portID = in_ctxt.GetEvaluatedOutputPortID( );
  
	switch( out_portID )
	{		
		case ID_OUT_Valid :
		{
			// Get the output port array ...			
			CDataArrayBool outData( in_ctxt );

			// Get Port Info
			if(!ToonixHairs_GetPortInfo(in_ctxt, ID_IN_HairPosition))return CStatus::False;
			if(!ToonixHairs_GetPortInfo(in_ctxt, ID_IN_HairSize))return CStatus::False;
			if(!ToonixHairs_GetPortInfo(in_ctxt, ID_IN_HairColor))return CStatus::False;
			
 			// Get the input data buffers for each port
			CDataArray2DVector3f HairPositionData( in_ctxt, ID_IN_HairPosition );
			CDataArray2DFloat HairSizeData( in_ctxt, ID_IN_HairSize );
			CDataArray2DColor4f HairColorData( in_ctxt, ID_IN_HairColor );

			//Application().LogMessage(L"Nb Hairs : "+(CString)HairPositionData.GetCount());
			line->m_chains.resize(HairPositionData.GetCount());

 			// We need a CIndexSet to iterate over the data 		
			CIndexSet indexSet( in_ctxt );
			for(CIndexSet::Iterator it = indexSet.Begin(); it.HasNext(); it.Next())
			{
				TXChain* chain = new TXChain();

				CDataArray2DVector3f::Accessor HairPositionSubArray = HairPositionData[it];
				CDataArray2DFloat::Accessor HairSizeSubArray = HairSizeData[it];
				CDataArray2DColor4f::Accessor HairColorSubArray = HairColorData[it];

				// Add code to set output port...
				ULONG nbHairs = HairPositionSubArray.GetCount();
				//Application().LogMessage(L"Nb Points on Hair ID "+(CString)it.GetIndex()+L" : "+(CString)nbHairs);
					
				for (ULONG i=0; i<HairPositionSubArray.GetCount( ); i++)
				{
					TXPoint* pnt = new TXPoint();
					pnt->m_pos = HairPositionSubArray[i];
					pnt->m_radius = HairSizeSubArray[i];
					pnt->m_color = HairColorSubArray[i];
					chain->AddPoint(pnt);
					//Application().LogMessage( CString( HairPositionSubArray[i] ) );
				}
				chain->m_closed = false;
				
				line->m_chains[it.GetIndex()] = chain;
				
			}
		}
		break;
		
		// Other output ports...
	};
	
	return CStatus::OK;
}

SICALLBACK ToonixHairs_Init( CRef& in_ctxt )
{
	InitTXLineData(in_ctxt);
	return CStatus::OK;
}

SICALLBACK ToonixHairs_Term( CRef& in_ctxt )
{
	CleanTXLineData(in_ctxt);
	return CStatus::OK;
}

