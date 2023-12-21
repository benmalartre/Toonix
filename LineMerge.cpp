//Toonix Line Merge
//------------------------------------------------
#include "Register.h"
#include "Data.h"
#include "Line.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_ToonixLines = 0,
	ID_G_100 = 100,
	ID_OUT_ToonixLine = 200,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};


CStatus RegisterToonixLineMerge( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixLineMerge",L"ToonixLineMerge");

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

	st = nodeDef.AddInputPort(ID_IN_ToonixLines,ID_G_100,ToonixLineCustomType,siICENodeStructureAny,siICENodeContextSingleton,L"ToonixLines",L"ToonixLines",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddOutputPort(ID_OUT_ToonixLine,ToonixLineCustomType,siICENodeStructureSingle,siICENodeContextSingleton,L"MergedLine",L"MergedLine",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Toonix");

	return CStatus::OK;
}

SICALLBACK ToonixLineMerge_Evaluate( ICENodeContext& in_ctxt )
{
	TXLine* out = (TXLine*)(CValue::siPtrType)in_ctxt.GetUserData( );
	TXLine* line = NULL;

	// Get the output port array ...			
	CDataArrayCustomType outData( in_ctxt );

	XSI::siICENodeDataType inPortType;
	XSI::siICENodeStructureType inPortStruct;
	XSI::siICENodeContextType inPortContext;
	in_ctxt.GetPortInfo( ID_IN_ToonixLines, inPortType, inPortStruct, inPortContext );

	CDataArrayCustomType::TData* data;
	ULONG sz;

	if ( inPortStruct == XSI::siICENodeStructureSingle )
	{
		CDataArrayCustomType ToonixLineData( in_ctxt, ID_IN_ToonixLines );
		CDataArrayCustomType::TData* data;
		ToonixLineData.GetData( 0,(const CDataArrayCustomType::TData**)&data, sz );
		line = (TXLine*)data;
		CDataArrayCustomType::TData* pOutData = outData.Resize(0,sizeof(TXLine));
		::memcpy( pOutData, line, sizeof(TXLine) );
	}
	else
	{
		out->ClearChains();

		CDataArray2DCustomType ToonixLine( in_ctxt, ID_IN_ToonixLines );
		CDataArray2DCustomType::Accessor accessor = ToonixLine[0];

		for(ULONG i=0;i<accessor.GetCount();i++)
		{
			accessor.GetData( i,(const CDataArrayCustomType::TData**)&data, sz );
			line = (TXLine*)data;
			out->Append(line);
		}

		CDataArrayCustomType::TData* pOutData = outData.Resize(0,sizeof(TXLine));
		::memcpy( pOutData, out, sizeof(TXLine) ); 
	}
	
	return CStatus::OK;
};

SICALLBACK ToonixLineMerge_Init( CRef& in_ctxt )
{
	Context ctxt(in_ctxt);
	// Build a new Line Object
	TXLine* line = new TXLine();
	ctxt.PutUserData((CValue::siPtrType)line);
	return CStatus::OK;
}


SICALLBACK ToonixLineMerge_Term( CRef& in_ctxt )
{
	Context ctxt(in_ctxt);
	//Delete Line Object
	if(!ctxt.GetUserData().IsEmpty())
	{
		TXLine* line = (TXLine*)(CValue::siPtrType)ctxt.GetUserData( );
		delete line;
		ctxt.PutUserData((CValue::siPtrType)NULL);
	}
	return CStatus::OK;
}
