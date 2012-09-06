//Toonix Line Modifier
//------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixData.h"
#include "ToonixLine.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_ToonixLine = 0,
	ID_IN_Position = 1,
	ID_IN_Radius = 2,
	ID_G_100 = 100,
	ID_OUT_ToonixLine = 200,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};


CStatus RegisterToonixLineModify( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixLineModify",L"ToonixLineModify");

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

	CStringArray ToonixLineCustomType(1);
	ToonixLineCustomType[0] = L"ToonixLine";

	st = nodeDef.AddInputPort(ID_IN_ToonixLine,ID_G_100,ToonixLineCustomType,siICENodeStructureSingle,siICENodeContextSingleton,L"ToonixLine",L"ToonixLine",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Position,ID_G_100,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextSingleton,L"Position",L"Position");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Radius,ID_G_100,siICENodeDataFloat,siICENodeStructureArray,siICENodeContextSingleton,L"Radius",L"Radius");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddOutputPort(ID_OUT_ToonixLine,ToonixLineCustomType,siICENodeStructureSingle,siICENodeContextSingleton,L"ModifiedLine",L"ModifiedLine",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Toonix");

	return CStatus::OK;
}

SICALLBACK ToonixLineModify_Evaluate( ICENodeContext& in_ctxt )
{
	// Get the input TXLine
	CDataArrayCustomType ToonixLine( in_ctxt, ID_IN_ToonixLine );

	CDataArrayCustomType::TData* pBufferToonixLine;
	ULONG nSizeToonixLine;
	ToonixLine.GetData( 0,(const CDataArrayCustomType::TData**)&pBufferToonixLine, nSizeToonixLine );

	if(nSizeToonixLine != sizeof(TXLine))return CStatus::OK;

	TXLine* line = (TXLine*)pBufferToonixLine;

	ULONG maxn;
	// Get Position Data
	CDataArray2DVector3f inPosition( in_ctxt, ID_IN_Position );
	CDataArray2DVector3f::Accessor aPosition = inPosition[0];
	maxn = aPosition.GetCount();

	CDataArray2DFloat inRadius( in_ctxt, ID_IN_Radius );
	CDataArray2DFloat::Accessor aRadius = inRadius[0];
	maxn = MAX(maxn,aRadius.GetCount());

	ULONG n =0;

	TXChain* chain = NULL;
	TXPoint* point = NULL;

	for(ULONG c=0;c<line->_chains.size();c++)
	{
		chain = line->_chains[c];
		for(ULONG p=0;p<chain->_points.size();p++)
		{
			if(n>=maxn)
			{
				Application().LogMessage(L"ToonixLineModify ---> Invalid Position or Radius Input Count!!",siErrorMsg);
				break;
			}
			point = chain->_points[p];
			point->_pos = aPosition[n];
			point->_radius = aRadius[n];
			n++;
		}
	}

	// Get the output port array ...			
	CDataArrayCustomType outData( in_ctxt );
	
	XSI::CDataArrayCustomType::TData* pOutData = outData.Resize(0,sizeof(TXLine));
	::memcpy( pOutData, line, sizeof(TXLine) );   
	
	return CStatus::OK;
};