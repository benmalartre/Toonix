// ToonixStrands Custom Node
//------------------------------------------
#include "ToonixRegister.h"
#include "ToonixGeometry.h"
#include "ToonixLine.h"

// Defines port, group and map identifiers used for registering the ICENode
enum ToonixStrandsIDs
{
	ID_IN_ToonixLine = 1,
	ID_IN_PointID = 2,
	ID_G_100 = 100,
	ID_OUT_Position = 200,
	ID_OUT_Tangent = 201,
	ID_OUT_Normal = 202,
	ID_OUT_Size = 203,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};

CStatus RegisterToonixStrands( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixStrands",L"ToonixStrands");

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

	st = nodeDef.AddInputPort(ID_IN_ToonixLine,ID_G_100,ToonixLineCustomType,siICENodeStructureSingle,siICENodeContextSingleton,L"ToonixLine",L"ToonixLine",ID_UNDEF,ID_UNDEF,ID_UNDEF);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_PointID,ID_G_100,siICENodeDataLong,siICENodeStructureSingle,siICENodeContextComponent0D,L"PointID",L"PointID",0,ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	// Add output ports.
	st = nodeDef.AddOutputPort(ID_OUT_Position,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextComponent0D,L"Position",L"Position",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddOutputPort(ID_OUT_Tangent,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextComponent0D,L"Tangent",L"Tangent",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddOutputPort(ID_OUT_Normal,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextComponent0D,L"Normal",L"Normal",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddOutputPort(ID_OUT_Size,siICENodeDataFloat,siICENodeStructureArray,siICENodeContextComponent0D,L"Size",L"Size",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Toonix");

	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus ToonixStrands_Evaluate( ICENodeContext& in_ctxt )
{
	// Get the input ELIHairMeshData
	CDataArrayCustomType ToonixLineData( in_ctxt, ID_IN_ToonixLine );

	CDataArrayCustomType::TData* pBufferMeshData;
	ULONG nSizeMeshData;
	ToonixLineData.GetData( 0,(const CDataArrayCustomType::TData**)&pBufferMeshData, nSizeMeshData );
	TXLine*  line = (TXLine*)pBufferMeshData;
	if(!line)return CStatus::OK;

	// The current output port being evaluated...
	LONG out_portID = in_ctxt.GetEvaluatedOutputPortID( );
	CDataArrayLong PointIDData(in_ctxt,ID_IN_PointID);

	TXChain* chain;
	ULONG id, ti;
	bool first;
	CVector3f p;
	float s;

	switch( out_portID )
	{	
		case ID_OUT_Position :
		{
			CDataArray2DVector3f outData( in_ctxt );
			
			// We need a CIndexSet to iterate over the data
			CIndexSet indexSet( in_ctxt);
			for(CIndexSet::Iterator it = indexSet.Begin(); it.HasNext(); it.Next())
			{
				// Creates an Accessor object to iterate over the 2D array data
				// Output arrays must always be initialized first
				id = PointIDData[it];
				
				chain = line->m_chains[id];
				ti = 0;
				first = true;
			
				ULONG u = (ULONG)chain->m_points.size()-1;
				
				CDataArray2DVector3f::Accessor outAccessor = outData.Resize( it, u);
				std::vector<TXPoint*>::iterator v;
				for( v = chain->m_points.begin(); v < chain->m_points.end(); ++v)
				{
					//Skip first segment first point
					if(first){first=false;continue;}
					else
					{
						p = (*v)->m_pos;
						outAccessor[ti].Set(p.GetX(),p.GetY(),p.GetZ());
						ti++;
					}
				}
			}
			
		}break;

		case ID_OUT_Tangent :
		{
			CDataArray2DVector3f outData( in_ctxt );
			
			// We need a CIndexSet to iterate over the data
			CIndexSet indexSet( in_ctxt);
			for(CIndexSet::Iterator it = indexSet.Begin(); it.HasNext(); it.Next())
			{
				// Creates an Accessor object to iterate over the 2D array data
				// Output arrays must always be initialized first
				id = PointIDData[it];
				
				chain = line->m_chains[id];
				ti = 0;
				first = true;
			
				ULONG u = (ULONG)chain->m_points.size()-1;
				
				CDataArray2DVector3f::Accessor outAccessor = outData.Resize( it, u);
				std::vector<TXPoint*>::iterator v;
				for( v = chain->m_points.begin(); v < chain->m_points.end(); ++v)
				{
					//Skip first segment first point
					if(first){first=false;continue;}
					else
					{
						p = (*v)->m_dir;
						outAccessor[ti].Set(p.GetX(),p.GetY(),p.GetZ());
						ti++;
					}
				}
			}
			
		}break;

		case ID_OUT_Normal :
		{
			CDataArray2DVector3f outData( in_ctxt );
			
			// We need a CIndexSet to iterate over the data
			CIndexSet indexSet( in_ctxt);
			for(CIndexSet::Iterator it = indexSet.Begin(); it.HasNext(); it.Next())
			{
				// Creates an Accessor object to iterate over the 2D array data
				// Output arrays must always be initialized first
				id = PointIDData[it];
				
				chain = line->m_chains[id];
				ti = 0;
				first = true;
			
				ULONG u = (ULONG)chain->m_points.size()-1;
				
				CDataArray2DVector3f::Accessor outAccessor = outData.Resize( it, u);
				std::vector<TXPoint*>::iterator v;
				for( v = chain->m_points.begin(); v < chain->m_points.end(); ++v)
				{
					//Skip first segment first point
					if(first){first=false;continue;}
					else
					{
						p = (*v)->m_norm;
						outAccessor[ti].Set(p.GetX(),p.GetY(),p.GetZ());
						ti++;
					}
				}
			}
			
		}break;

		case ID_OUT_Size :
		{
			CDataArray2DFloat outData( in_ctxt );
			
			// We need a CIndexSet to iterate over the data
			CIndexSet indexSet( in_ctxt);
			for(CIndexSet::Iterator it = indexSet.Begin(); it.HasNext(); it.Next())
			{
				// Creates an Accessor object to iterate over the 2D array data
				// Output arrays must always be initialized first
				id = PointIDData[it];
				
				chain = line->m_chains[id];
				ti = 0;
				first = true;
			
				ULONG u = (ULONG)chain->m_points.size()-1;
				
				CDataArray2DFloat::Accessor outAccessor = outData.Resize( it, u);
				std::vector<TXPoint*>::iterator v;
				for( v = chain->m_points.begin(); v < chain->m_points.end(); ++v)
				{
					//Skip first segment first point
					if(first){first=false;continue;}
					else
					{
						s = (*v)->m_radius;
						outAccessor[ti] = s;
						ti++;
					}
				}
			}
			
		}break;
	}

	return CStatus::OK;
}



