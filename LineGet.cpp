//Toonix Line Getter
//------------------------------------------------
#include "Register.h"
#include "Data.h"
#include "Line.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_ToonixLine = 0,
	ID_G_100 = 100,
	ID_OUT_Position = 200,
	ID_OUT_Direction = 201,
	ID_OUT_Normal = 202,
	ID_OUT_Length = 203,
	ID_OUT_Radius = 204,
	ID_OUT_LineID = 205,
	ID_OUT_SublineID = 206,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};

bool GetLineGetterDirtyState(ICENodeContext& in_ctxt )
{
	CICEPortState LineState( in_ctxt, ID_IN_ToonixLine );
	bool lineDirty = LineState.IsDirty( CICEPortState::siAnyDirtyState );
	LineState.ClearState();

	return(lineDirty);
}


CStatus RegisterToonixLineGet( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixLineGet",L"ToonixLineGet");

	CStatus st;
	st = nodeDef.PutColor(ToonixNodeR,ToonixNodeG,ToonixNodeB);
	st.AssertSucceeded( ) ;

	st = nodeDef.PutThreadingModel(siICENodeSingleThreading);
	st.AssertSucceeded( ) ;

	// Add custom types definition.
	st = nodeDef.DefineCustomType(L"ToonixLine",L"ToonixLine",L"ToonixLine",ToonixLineR,ToonixLineG,ToonixLineB);
	st.AssertSucceeded( ) ;

	// Add input ports and groups.
	st = nodeDef.AddPortGroup(ID_G_100);
	st.AssertSucceeded( ) ;

	CStringArray ToonixLineCustomType(1);
	ToonixLineCustomType[0] = L"ToonixLine";

	st = nodeDef.AddInputPort(ID_IN_ToonixLine,ID_G_100,ToonixLineCustomType,siICENodeStructureSingle,siICENodeContextSingleton,L"ToonixLine",L"ToonixLine",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	// Add output ports.
	st = nodeDef.AddOutputPort(ID_OUT_Position,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextSingleton,L"Position",L"Position");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddOutputPort(ID_OUT_Direction,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextSingleton,L"Direction",L"Direction");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddOutputPort(ID_OUT_Normal,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextSingleton,L"Normal",L"Normal");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddOutputPort(ID_OUT_Length,siICENodeDataFloat,siICENodeStructureArray,siICENodeContextSingleton,L"Length",L"Length");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddOutputPort(ID_OUT_Radius,siICENodeDataFloat,siICENodeStructureArray,siICENodeContextSingleton,L"Radius",L"Radius");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddOutputPort(ID_OUT_LineID,siICENodeDataLong,siICENodeStructureArray,siICENodeContextSingleton,L"LineID",L"LineID");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddOutputPort(ID_OUT_SublineID,siICENodeDataLong,siICENodeStructureArray,siICENodeContextSingleton,L"SublineID",L"SublineID");
	st.AssertSucceeded( ) ;

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Toonix");

	return CStatus::OK;
}

SICALLBACK ToonixLineGet_Evaluate( ICENodeContext& in_ctxt )
{
	// Get the input TXLine
	CDataArrayCustomType ToonixLine( in_ctxt, ID_IN_ToonixLine );

	CDataArrayCustomType::TData* pBufferToonixLine;
	ULONG nSizeToonixLine;
	ToonixLine.GetData( 0,(const CDataArrayCustomType::TData**)&pBufferToonixLine, nSizeToonixLine );
	if(nSizeToonixLine != sizeof(TXLine))return CStatus::OK;

	TXLine* line = (TXLine*)pBufferToonixLine;

	TXChain* chain = NULL;
	TXPoint* point = NULL;

	// The current output port being evaluated...
	LONG out_portID = in_ctxt.GetEvaluatedOutputPortID( );

	switch( out_portID )
	{	
		case ID_OUT_Position :
		{
			// Get the output port array ...			
			CDataArray2DVector3f outData( in_ctxt );

			CDataArray2DVector3f::Accessor outDataSubArray = outData.Resize(0,line->m_nbp);
			ULONG n = 0;
			for(ULONG c=0;c<line->m_chains.size();c++)
			{
				chain = line->m_chains[c];
				for(ULONG p=0;p<chain->m_points.size();p++)
				{
					point = chain->m_points[p];
					outDataSubArray[n] = point->m_pos;
					n++;
				}
			}
		}

		break;
		
		case ID_OUT_Direction :
		{	
			// Get the output port array ...					
			CDataArray2DVector3f outData( in_ctxt );

			CDataArray2DVector3f::Accessor outDataSubArray = outData.Resize(0,line->m_nbp);
			ULONG n = 0;
			for(ULONG c=0;c<line->m_chains.size();c++)
			{
				chain = line->m_chains[c];
				for(ULONG p=0;p<chain->m_points.size();p++)
				{
					point = chain->m_points[p];
					outDataSubArray[n] = point->m_dir;
					n++;
				}
			}
		}
		break;

		case ID_OUT_Normal:
		{	
			// Get the output port array ...					
			CDataArray2DVector3f outData( in_ctxt );

			CDataArray2DVector3f::Accessor outDataSubArray = outData.Resize(0,line->m_nbp);
			ULONG n = 0;
			for(ULONG c=0;c<line->m_chains.size();c++)
			{
				chain = line->m_chains[c];
				for(ULONG p=0;p<chain->m_points.size();p++)
				{
					point = chain->m_points[p];
					outDataSubArray[n] = point->m_norm;
					n++;
				}
			}
		}
		break;

		case ID_OUT_Length :
		{	
			// Get the output port array ...					
			CDataArray2DFloat outData( in_ctxt );

			CDataArray2DFloat::Accessor outDataSubArray = outData.Resize(0,line->m_nbp);
			ULONG n = 0;
			for(ULONG c=0;c<line->m_chains.size();c++)
			{
				chain = line->m_chains[c];
				for(ULONG p=0;p<chain->m_points.size();p++)
				{
					point = chain->m_points[p];
					outDataSubArray[n] = point->m_length;
					n++;
				}
			}
		}
		break;

		case ID_OUT_Radius :
		{
			// Get the output port array ...			
			CDataArray2DFloat outData( in_ctxt );

			CDataArray2DFloat::Accessor outDataSubArray = outData.Resize(0,line->m_nbp);
			ULONG n = 0;
			for(ULONG c=0;c<line->m_chains.size();c++)
			{
				chain = line->m_chains[c];
				for(ULONG p=0;p<chain->m_points.size();p++)
				{
					point = chain->m_points[p];
					outDataSubArray[n] = point->m_radius;
					n++;
				}
			}
		}
		break;

		case ID_OUT_LineID :
		{	
			// Get the output port array ...					
			CDataArray2DLong outData( in_ctxt );

			CDataArray2DLong::Accessor outDataSubArray = outData.Resize(0,line->m_nbp);
			ULONG n = 0;
			for(ULONG c=0;c<line->m_chains.size();c++)
			{
				chain = line->m_chains[c];
				for(ULONG p=0;p<chain->m_points.size();p++)
				{
					point = chain->m_points[p];
					outDataSubArray[n] = point->m_lineid;
					n++;
				}
			}
		}
		break;

		case ID_OUT_SublineID :
		{	
			// Get the output port array ...					
			CDataArray2DLong outData( in_ctxt );

			CDataArray2DLong::Accessor outDataSubArray = outData.Resize(0,line->m_nbp);
			ULONG n = 0;
			for(ULONG c=0;c<line->m_chains.size();c++)
			{
				chain = line->m_chains[c];
				for(ULONG p=0;p<chain->m_points.size();p++)
				{
					point = chain->m_points[p];
					outDataSubArray[n] = point->m_sublineid;
					n++;
				}
			}
		}
		break;
	};
	return CStatus::OK;
}
