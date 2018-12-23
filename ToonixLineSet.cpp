//Toonix Line Setter
//------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixData.h"
#include "ToonixLine.h"
#include "ToonixCommon.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_Position = 0,
	ID_IN_Direction = 1,
	ID_IN_Normal = 2,
	ID_IN_Length = 3,
	ID_IN_Radius = 4,
	ID_IN_LineID = 5,
	ID_IN_SublineID = 6,
	ID_IN_Color = 7,
	ID_G_100 = 100,
	ID_OUT_ToonixLine = 200,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};

bool GetLineSetterDirtyState(ICENodeContext& in_ctxt )
{
	CICEPortState PositionState( in_ctxt, ID_IN_Position );
	bool positionDirty = PositionState.IsDirty( CICEPortState::siDataDirtyState );
	PositionState.ClearState();

	CICEPortState DirectionState( in_ctxt, ID_IN_Direction );
	bool directionDirty = DirectionState.IsDirty( CICEPortState::siDataDirtyState );
	DirectionState.ClearState();

	CICEPortState NormalState( in_ctxt, ID_IN_Normal );
	bool normalDirty = NormalState.IsDirty( CICEPortState::siDataDirtyState );
	NormalState.ClearState();

	CICEPortState LengthState( in_ctxt, ID_IN_Length );
	bool lengthDirty = LengthState.IsDirty( CICEPortState::siDataDirtyState );
	LengthState.ClearState();

	CICEPortState RadiusState( in_ctxt, ID_IN_Radius );
	bool radiusDirty = RadiusState.IsDirty( CICEPortState::siDataDirtyState );
	RadiusState.ClearState();

	CICEPortState LineIDState( in_ctxt, ID_IN_LineID );
	bool lineIDDirty = LineIDState.IsDirty( CICEPortState::siDataDirtyState );
	LineIDState.ClearState();

	CICEPortState SublineIDState( in_ctxt, ID_IN_SublineID );
	bool sublineIDDirty = SublineIDState.IsDirty( CICEPortState::siDataDirtyState );
	SublineIDState.ClearState();

	CICEPortState ColorState( in_ctxt, ID_IN_Color );
	bool colorDirty = ColorState.IsDirty( CICEPortState::siDataDirtyState );
	ColorState.ClearState();

	return(positionDirty||directionDirty||normalDirty||lengthDirty||radiusDirty||lineIDDirty||sublineIDDirty||colorDirty);
}


CStatus RegisterToonixLineSet( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixLineSet",L"ToonixLineSet");

	CStatus st;
	st = nodeDef.PutColor(ToonixNodeR,ToonixNodeG,ToonixNodeB);
	st.AssertSucceeded() ;

	st = nodeDef.PutThreadingModel(siICENodeSingleThreading);
	st.AssertSucceeded( ) ;

	// Define the ToonLineData custom type.
	st = nodeDef.DefineCustomType(	L"ToonixLine",L"ToonixLine", L"ToonixLine",ToonixLineR,ToonixLineG, ToonixLineB);

	// Add input ports and groups.
	st = nodeDef.AddPortGroup(ID_G_100);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Position,ID_G_100,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextSingleton,L"Position",L"Position");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Direction,ID_G_100,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextSingleton,L"Direction",L"Direction");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Normal,ID_G_100,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextSingleton,L"Normal",L"Normal");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Length,ID_G_100,siICENodeDataFloat,siICENodeStructureArray,siICENodeContextSingleton,L"Length",L"Length");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Radius,ID_G_100,siICENodeDataFloat,siICENodeStructureArray,siICENodeContextSingleton,L"Radius",L"Radius");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_LineID,ID_G_100,siICENodeDataLong,siICENodeStructureArray,siICENodeContextSingleton,L"LineID",L"LineID");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_SublineID,ID_G_100,siICENodeDataLong,siICENodeStructureArray,siICENodeContextSingleton,L"SublineID",L"SublineID");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Color,ID_G_100,siICENodeDataColor4,siICENodeStructureArray,siICENodeContextSingleton,L"Color",L"Color");
	st.AssertSucceeded( ) ;

	// Add OutputPort
	CStringArray ToonixLineCustomType(1);
	ToonixLineCustomType[0] = L"ToonixLine";

	st = nodeDef.AddOutputPort(ID_OUT_ToonixLine,ToonixLineCustomType,siICENodeStructureSingle,siICENodeContextSingleton,L"ToonixLine",L"ToonixLine",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Toonix");

	return CStatus::OK;
}

SICALLBACK ToonixLineSet_Evaluate( ICENodeContext& in_ctxt )
{
	TXLine* line = NULL;

	if(in_ctxt.GetUserData().IsEmpty())
	{
		//Application().LogMessage(L"ToonixLineSet ---> UserData is Empty :: We create New TXLine...");
		line = new TXLine();
		in_ctxt.PutUserData((CValue::siPtrType)line);
	}

	line =(TXLine*)(CValue::siPtrType)in_ctxt.GetUserData();
	line->ClearChains();

	// The current output port being evaluated...
	ULONG out_portID = in_ctxt.GetEvaluatedOutputPortID( );

	switch( out_portID )
	{		
		// only one output...
		case ID_OUT_ToonixLine :
		{
			// Get Position Data
			CDataArray2DVector3f inPosition( in_ctxt, ID_IN_Position );
			CDataArray2DVector3f::Accessor aPosition = inPosition[0];

			CDataArray2DVector3f inDirection( in_ctxt, ID_IN_Direction );
			CDataArray2DVector3f::Accessor aDirection = inDirection[0];

			CDataArray2DVector3f inNormal( in_ctxt, ID_IN_Normal );
			CDataArray2DVector3f::Accessor aNormal = inNormal[0];

			CDataArray2DFloat inLength( in_ctxt, ID_IN_Length );
			CDataArray2DFloat::Accessor aLength = inLength[0];

			CDataArray2DFloat inRadius( in_ctxt, ID_IN_Radius );
			CDataArray2DFloat::Accessor aRadius = inRadius[0];

			CDataArray2DLong inLineID( in_ctxt, ID_IN_LineID );
			CDataArray2DLong::Accessor aLineID = inLineID[0];

			CDataArray2DLong inSublineID( in_ctxt, ID_IN_SublineID );
			CDataArray2DLong::Accessor aSublineID = inSublineID[0];

			CDataArray2DColor4f inColor( in_ctxt, ID_IN_Color );
			CDataArray2DColor4f::Accessor aColor = inColor[0];

			ULONG nbp = aPosition.GetCount();

			LONG currentline, currentsubline;
			currentline = aLineID[0];
			currentsubline = aSublineID[0];
			TXChain* chain = new TXChain();
			
			for(ULONG n=0;n<nbp;n++)
			{
				TXPoint* pnt = new TXPoint();
				pnt->m_pos = aPosition[n];
				pnt->m_dir = aDirection[n];
				pnt->m_norm = aNormal[n];
				pnt->m_length = aLength[n];
				pnt->m_radius = aRadius[n];
				pnt->m_lineid = aLineID[n];
				pnt->m_sublineid = aSublineID[n];
				pnt->m_color = aColor[n];
				chain->AddPoint(pnt);

				if(currentline != pnt->m_lineid && currentsubline != pnt->m_sublineid)
				{
					line->m_chains.push_back(chain);
					chain = new TXChain();
					
				}
				
				currentline = pnt->m_lineid;
				currentsubline = pnt->m_sublineid;
			}
			line->m_chains.push_back(chain);

			// Get the output port array ...			
			CDataArrayCustomType outData( in_ctxt );
			
			XSI::CDataArrayCustomType::TData* pOutData = outData.Resize(0,sizeof(TXLine));
			::memcpy( pOutData, line, sizeof(TXLine) );   
		}
		break;
	}
	
	return CStatus::OK;
};

SICALLBACK ToonixLineSet_Term( CRef& in_ctxt )
{
	CleanTXLineData(in_ctxt);
	return CStatus::OK;
};