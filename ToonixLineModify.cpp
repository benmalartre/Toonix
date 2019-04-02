//Toonix Line Modifier
//------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixCommon.h"
#include "ToonixData.h"
#include "ToonixLine.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_ToonixLine = 0,
	ID_IN_Position = 1,
	ID_IN_Radius = 2,
	ID_IN_Color = 3,
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

	//st = nodeDef.PutThreadingModel(siICENodeSingleThreading);
	//st.AssertSucceeded( ) ;

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

	st = nodeDef.AddInputPort(ID_IN_Color,ID_G_100,siICENodeDataColor4,siICENodeStructureArray,siICENodeContextSingleton,L"Color",L"Color");
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

	ULONG maxp, maxr, maxc;
	bool p2d,r2d,c2d;
	maxp = maxr = maxc = 0;

	CDataArray2DVector3f::Accessor aPosition;
	CDataArray2DFloat::Accessor aRadius;
	CDataArray2DColor4f::Accessor aColor;

	// Get Color Data
	c2d = IsInputArray(in_ctxt,ID_IN_Color);
	
	if(c2d)
	{
		CDataArray2DColor4f inColor( in_ctxt, ID_IN_Color );
		aColor = inColor[0];
		maxc = aColor.GetCount();
	}

	// Get Radius Data
	r2d = IsInputArray(in_ctxt,ID_IN_Radius);
	if(r2d)
	{
		CDataArray2DFloat inRadius( in_ctxt, ID_IN_Radius );
		aRadius = inRadius[0];
		maxr = aRadius.GetCount();
	}

	// Get Position Data
	p2d = IsInputArray(in_ctxt,ID_IN_Position);
	if(p2d)
	{
		CDataArray2DVector3f inPosition( in_ctxt, ID_IN_Position );
		aPosition = inPosition[0];
		maxp = aPosition.GetCount();
	}
	

	ULONG n =0;

	TXChain* chain = NULL;
	TXPoint* point = NULL;

	for(ULONG c=0;c<line->m_chains.size();c++)
	{
		chain = line->m_chains[c];
		for(ULONG p=0;p<chain->m_points.size();p++)
		{
			point = chain->m_points[p];
			if(p2d && n<maxp)point->m_pos = aPosition[n];
			if (r2d && n < maxr)point->m_radius = aRadius[n];
			if(c2d && n<maxc)point->m_color = aColor[n];
			n++;
		}
	}
	
	// Get the output port array ...			
	CDataArrayCustomType outData( in_ctxt );
	
	XSI::CDataArrayCustomType::TData* pOutData = outData.Resize(0,sizeof(TXLine));
	::memcpy( pOutData, line, sizeof(TXLine) );   
	
	return CStatus::OK;
};