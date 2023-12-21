// Toonix Points Custom Node
//------------------------------------------
#include "Register.h"
#include "Geometry.h"
#include "Line.h"

// Defines port, group and map identifiers used for registering the ICENode
enum PointsIDs
{
	ID_IN_ToonixLine = 0,
	ID_G_100 = 100,
	ID_OUT_Position = 200,
	ID_OUT_Orientation = 201,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};

struct ToonixPointsData
{
	std::vector<CVector3f> _pos;
	ULONG _nbp;
};

CStatus RegisterToonixPoints( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixPoints",L"ToonixPoints");

	CStatus st;
	st = nodeDef.PutColor(ToonixNodeR,ToonixNodeG,ToonixNodeB);
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
	st.AssertSucceeded();
	
	// Add output ports.
	st = nodeDef.AddOutputPort(ID_OUT_Position, siICENodeDataVector3,siICENodeStructureSingle,siICENodeContextElementGenerator,L"RootPosition",L"RootPosition");
	st.AssertSucceeded( ) ;

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Toonix");

	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus ToonixPoints_Evaluate( ICENodeContext& in_ctxt )
{
	// Get User Data
	ToonixPointsData* data = (ToonixPointsData*)(CValue::siPtrType)in_ctxt.GetUserData( );
	CDataArrayVector3f outData( in_ctxt );
	
	CIndexSet indexSet( in_ctxt );
	for(CIndexSet::Iterator it = indexSet.Begin(); it.HasNext(); it.Next())
	{
		outData[it] = data->_pos[it.GetAbsoluteIndex()]; 
	}

	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus ToonixPoints_BeginEvaluate( ICENodeContext& in_ctxt )
{
	ToonixPointsData* points = (ToonixPointsData*)(CValue::siPtrType)in_ctxt.GetUserData( );

	// Get the input ToonixLine
	CDataArrayCustomType ToonixLineData( in_ctxt, ID_IN_ToonixLine );

	CDataArrayCustomType::TData* pBufferMeshData;
	ULONG nSizeMeshData;
	ToonixLineData.GetData( 0,(const CDataArrayCustomType::TData**)&pBufferMeshData, nSizeMeshData );
	TXLine*  line = (TXLine*)pBufferMeshData;
	
	if(!line)
		in_ctxt.PutNumberOfElementsToProcess(0);

	else
	{
		points->_nbp = (ULONG)line->m_chains.size();
		points->_pos.resize(points->_nbp);

		for(ULONG i=0;i<points->_nbp;i++)
		{
			points->_pos[i] = line->m_chains[i]->m_points.front()->m_pos;
		}

		in_ctxt.PutNumberOfElementsToProcess(points->_nbp);
	}

	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus ToonixPoints_Init( CRef& in_ctxt )
{
	Context ctxt(in_ctxt);
	ToonixPointsData* data = new ToonixPointsData();
	ctxt.PutUserData((CValue::siPtrType)data );
	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus ToonixPoints_Term( CRef& in_ctxt )
{
	Context ctxt(in_ctxt);
	ToonixPointsData* data = (ToonixPointsData*)(CValue::siPtrType)ctxt.GetUserData( );
	delete data;
	ctxt.PutUserData((CValue)NULL);
	return CStatus::OK;
}



