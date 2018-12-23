//Toonix Cluster
//------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixData.h"
#include "ToonixCommon.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_ToonixData = 0,
	ID_IN_Cluster = 1,
	ID_IN_Width = 2,
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

bool GetClusterDirtyState(ICENodeContext& in_ctxt )
{
	CICEPortState DataState( in_ctxt, ID_IN_ToonixData );
	bool dataDirty = DataState.IsDirty( CICEPortState::siAnyDirtyState );

	CICEPortState WidthState( in_ctxt, ID_IN_Width );
	bool widthDirty = WidthState.IsDirty( CICEPortState::siAnyDirtyState );
	WidthState.ClearState();

	CICEPortState ClusterState( in_ctxt, ID_IN_Cluster );
	bool clusterDirty = ClusterState.IsDirty( CICEPortState::siAnyDirtyState );
	ClusterState.ClearState();

	return(	 dataDirty||widthDirty||clusterDirty);
}


CStatus RegisterToonixCluster( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixCluster",L"ToonixCluster");

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

	st = nodeDef.AddInputPort(ID_IN_Cluster,ID_G_100,siICENodeDataBool,siICENodeStructureArray,siICENodeContextSingleton,L"Cluster",L"Cluster");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Width,ID_G_100,siICENodeDataFloat,siICENodeStructureSingle,siICENodeContextSingleton,L"Width",L"Width",0.1);
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

SICALLBACK ToonixCluster_Evaluate( ICENodeContext& in_ctxt )
{
	 // Get User Data
	TXLine* line = (TXLine*)(CValue::siPtrType)in_ctxt.GetUserData( );;

	// Get the input TXGeometry
	CDataArrayCustomType ToonixData( in_ctxt, ID_IN_ToonixData );

	CDataArrayCustomType::TData* pBufferToonixData;
	ULONG nSizeToonixData;
	ToonixData.GetData( 0,(const CDataArrayCustomType::TData**)&pBufferToonixData, nSizeToonixData );
	TXData* data = (TXData*)pBufferToonixData;
	line->m_geom = data->m_geom;

	if(GetClusterDirtyState(in_ctxt))
	{
		line->ClearChains();
		// Get the parameters value
		CDataArray2DBool clusterData( in_ctxt, ID_IN_Cluster );
		CDataArray2DBool::Accessor cluster = clusterData[0];
		ULONG nbc = cluster.GetCount();
		
		CDataArrayFloat widthData(in_ctxt, ID_IN_Width);
		line->m_width = widthData[0];

		CDataArrayFloat breakData(in_ctxt, ID_IN_BreakAngle);
		line->m_break = (float)DegreesToRadians(breakData[0]);

		CDataArrayFloat filterData(in_ctxt, ID_IN_FilterPoints);
		line->m_filterpoints = filterData[0];

		CDataArrayFloat extendData(in_ctxt, ID_IN_Extend);
		line->m_extend = extendData[0];
		
		TXEdge* pe;
		line->m_geom->m_clusteredges.clear();
		for(ULONG e=0;e<line->m_geom->m_edges.size();e++)
		{
			pe = line->m_geom->m_edges[e];
			if(pe->m_id>=nbc)
			{
				pe->m_isclusteredge = false;
				continue;
			}
			if(cluster[pe->m_id])
			{
				pe->m_isclusteredge = true;
				line->m_geom->m_clusteredges.push_back(pe);
			}
			else
			{
				pe->m_isclusteredge = false;
			}
		}
		line->Build(CLUSTER);
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
	
	return CStatus::OK;
}

SICALLBACK ToonixCluster_Init( CRef& in_ctxt )
{
	InitTXLineData(in_ctxt);
	return CStatus::OK;
}


SICALLBACK ToonixCluster_Term( CRef& in_ctxt )
{
	CleanTXLineData(in_ctxt);
	return CStatus::OK;
}
