// TXClusterEdges Plugin
// Initial code generated by Softimage SDK Wizard
// Executed Thu Dec 13 09:59:56 UTC+0100 2012 by benjamin.malartre
// 
// 
// Tip: You need to compile the generated code before you can load the plug-in.
// After you compile the plug-in, you can load it by clicking Update All in the Plugin Manager.
#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>

#include <xsi_icenodecontext.h>
#include <xsi_icenodedef.h>
#include <xsi_indexset.h>
#include <xsi_command.h>
#include <xsi_factory.h>
#include <xsi_longarray.h>
#include <xsi_doublearray.h>
#include <xsi_math.h>
#include <xsi_vector2f.h>
#include <xsi_vector3f.h>
#include <xsi_vector4f.h>
#include <xsi_matrix3f.h>
#include <xsi_matrix4f.h>
#include <xsi_rotationf.h>
#include <xsi_quaternionf.h>
#include <xsi_color4f.h>
#include <xsi_shape.h>
#include <xsi_icegeometry.h>
#include <xsi_iceportstate.h>
#include <xsi_indexset.h>
#include <xsi_dataarray.h>
#include <xsi_dataarray2D.h>

#include <vector>

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_Geometry = 0,
	ID_IN_EdgeCluster = 1,
	ID_G_100 = 100,
	ID_OUT_ToonixEdges = 200,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};

XSI::CStatus RegisterTXClusterEdges( XSI::PluginRegistrar& in_reg );
std::vector<ULONG> _edges;

using namespace XSI; 

SICALLBACK XSILoadPlugin( PluginRegistrar& in_reg )
{
	in_reg.PutAuthor(L"benjamin.malartre");
	in_reg.PutName(L"TXClusterEdges Plugin");
	in_reg.PutVersion(1,0);

	RegisterTXClusterEdges( in_reg );

	//RegistrationInsertionPoint - do not remove this line

	return CStatus::OK;
}

SICALLBACK XSIUnloadPlugin( const PluginRegistrar& in_reg )
{
	CString strPluginName;
	strPluginName = in_reg.GetName();
	Application().LogMessage(strPluginName + L" has been unloaded.",siVerboseMsg);
	return CStatus::OK;
}

CStatus RegisterTXClusterEdges( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"TXClusterEdges",L"TXClusterEdges");

	CStatus st;
	st = nodeDef.PutColor(154,188,102);
	st.AssertSucceeded( ) ;

	st = nodeDef.PutThreadingModel(XSI::siICENodeSingleThreading);
	st.AssertSucceeded( ) ;


	// Add input ports and groups.
	st = nodeDef.AddPortGroup(ID_G_100);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Geometry,ID_G_100,siICENodeDataGeometry,siICENodeStructureAny,siICENodeContextSingleton,L"Geometry",L"Geometry",CValue(),CValue(),CValue(),ID_UNDEF,ID_UNDEF,ID_UNDEF);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_EdgeCluster,ID_G_100,siICENodeDataBool,siICENodeStructureAny,siICENodeContextComponent1D,L"EdgeCluster",L"EdgeCluster",true,CValue(),CValue(),ID_UNDEF,ID_UNDEF,ID_UNDEF);
	st.AssertSucceeded( ) ;

	// Add output ports.
	st = nodeDef.AddOutputPort(ID_OUT_ToonixEdges,siICENodeDataLong,siICENodeStructureArray,siICENodeContextSingleton,L"ToonixEdges",L"ToonixEdges",ID_UNDEF,ID_UNDEF,ID_UNDEF);
	st.AssertSucceeded( ) ;

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Custom ICENode");

	return CStatus::OK;
}


SICALLBACK TXClusterEdges_Evaluate( ICENodeContext& in_ctxt )
{
	_edges.clear();
	
	siICENodeDataType GeomType;
	siICENodeStructureType GeomStruct;
	siICENodeContextType GeomContext;
	in_ctxt.GetPortInfo( ID_IN_EdgeCluster, GeomType, GeomStruct, GeomContext );

	if ( GeomStruct == XSI::siICENodeStructureSingle )
		Application().LogMessage(L"Geometry Structure Type : siICENodeStructureSingle");

	else if ( GeomStruct == XSI::siICENodeStructureArray )
		Application().LogMessage(L"Geometry Structure Type : siICENodeStructureArray");

	siICENodeDataType ClusterPortType;
	siICENodeStructureType ClusterPortStruct;
	siICENodeContextType ClusterPortContext;
	in_ctxt.GetPortInfo( ID_IN_EdgeCluster, ClusterPortType, ClusterPortStruct, ClusterPortContext );

	if ( ClusterPortStruct == XSI::siICENodeStructureSingle )
	{
		Application().LogMessage(L"siICENodeStructureSingle");
		CDataArrayBool ClusterData( in_ctxt, ID_IN_EdgeCluster );
		for(ULONG a=0;a<ClusterData.GetCount();a++)
		{
			if(ClusterData[a])_edges.push_back(a);
		}
	}
	else if ( ClusterPortStruct == XSI::siICENodeStructureArray )
	{
		Application().LogMessage(L"siICENodeStructureArray");
		CDataArray2DBool ClusterData( in_ctxt, ID_IN_EdgeCluster );
		ULONG base = 0;
		CDataArray2DBool::Accessor inAccess = ClusterData[0];
		Application().LogMessage(L"ClusterData Nb "+(CString)(ULONG)ClusterData[0].GetCount());
		Application().LogMessage(L"ClusterData Nb "+(CString)(ULONG)ClusterData[1].GetCount());
		Application().LogMessage(L"ClusterData Nb "+(CString)(ULONG)ClusterData[2].GetCount());
		Application().LogMessage(L"ClusterData Nb "+(CString)(ULONG)ClusterData.GetCount());
		/*
		Application().LogMessage(L"ClusterData Nb "+(CString)(ULONG)ClusterData.GetCount()); 
		
		for(ULONG a=0;a<ClusterData.GetCount();a++)
		{
			CDataArray2DBool::Accessor inAccess = ClusterData[a];
			ULONG offset = inAccess.GetCount();
			Application().LogMessage(L"Nb Segments in Geometry ID "+(CString)a+L" : "+(CString)offset); 
			for(ULONG c=0;c<offset;c++)
			{
				if(inAccess[c])_edges.push_back(c+base);
			}
			base += offset;
		}
		*/
	}
	
	CString out = L"Marked Edges : ";
	for(ULONG e=0;e<_edges.size();e++)
	{
		out += (CString)_edges[e]+L",";
	}
	Application().LogMessage(L" NB Edges : "+ (CString)(ULONG)_edges.size());
	Application().LogMessage(out);

	CDataArray2DLong OutData( in_ctxt );
	CDataArray2DLong::Accessor OutAccess = OutData.Resize(0,(ULONG)_edges.size());

	for ( ULONG e=0; e<_edges.size(); e++ )
	{
		OutAccess[e] = _edges[e];
	}
	
	return CStatus::OK;
}
/*
SICALLBACK TXClusterEdges_Evaluate( ICENodeContext& in_ctxt )
{
	// The current output port being evaluated...
	ULONG out_portID = in_ctxt.GetEvaluatedOutputPortID( );
  
	switch( out_portID )
	{		
		case ID_OUT_ToonixEdges :
		{
			// Get the output port array ...			
			CDataArray2DBool outData( in_ctxt );
			
 			// Get the input data buffers for each port
			CDataArray2DBool EdgeClusterData( in_ctxt, ID_IN_EdgeCluster );

		}
		break;
		
		// Other output ports...
	};
	
	return CStatus::OK;
}
*/
