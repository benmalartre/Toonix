//Toonix BVH
//------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixData.h"
#include "ToonixCommon.h"
#include "ToonixKDTree.h"
#include "ToonixGLManager.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	//ID_IN_ToonixData = 0,
	ID_IN_Geometry = 0,
	ID_IN_Depth = 1,
	ID_IN_MaxNbPoints = 2,
	ID_G_100 = 100,
	ID_OUT_Scale = 201,
	ID_OUT_Orientation = 202,
	ID_OUT_Position = 203,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};


/*
bool GetBVHDirtyState(ICENodeContext& in_ctxt )
{
	CICEPortState DataState( in_ctxt, ID_IN_ToonixData );
	bool dataDirty = DataState.IsDirty( CICEPortState::siAnyDirtyState );

	CICEPortState WidthState( in_ctxt, ID_IN_Width );
	bool widthDirty = WidthState.IsDirty( CICEPortState::siAnyDirtyState );
	WidthState.ClearState();

	return(widthDirty||dataDirty);
}
*/

CStatus RegisterToonixBVH( PluginRegistrar& in_reg )
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixBVH",L"ToonixBVH");

	CStatus st;
	st = nodeDef.PutColor(ToonixNodeR,ToonixNodeG,ToonixNodeB);
	st.AssertSucceeded( ) ;

	st = nodeDef.PutThreadingModel(siICENodeSingleThreading);
	st.AssertSucceeded( ) ;

	/*
	// Add custom types definition.
	st = nodeDef.DefineCustomType(L"ToonixData",L"ToonixData",L"ToonixData",ToonixDataR,ToonixDataG,ToonixDataB);
	st.AssertSucceeded( ) ;
	st = nodeDef.DefineCustomType(L"ToonixLine",L"ToonixLine",L"ToonixLine",ToonixLineR,ToonixLineG,ToonixLineB);
	st.AssertSucceeded( ) ;
	*/

	// Add input ports and groups.
	st = nodeDef.AddPortGroup(ID_G_100);
	st.AssertSucceeded( ) ;

	CStringArray ToonixDataCustomType(1);
	ToonixDataCustomType[0] = L"ToonixData";

	st = nodeDef.AddInputPort(ID_IN_Geometry,ID_G_100,siICENodeDataGeometry,siICENodeStructureSingle,siICENodeContextSingleton,L"Geometry",L"Geometry");
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_Depth,ID_G_100,siICENodeDataLong,siICENodeStructureSingle,siICENodeContextSingleton,L"Depth",L"Depth",4);
	st.AssertSucceeded( ) ;

	st = nodeDef.AddInputPort(ID_IN_MaxNbPoints,ID_G_100,siICENodeDataLong,siICENodeStructureSingle,siICENodeContextSingleton,L"MaxNbPoints",L"MaxNbPoints",1000);
	st.AssertSucceeded( ) ;

	/*
	// Add output ports.
	CStringArray ToonixLineCustomType(1);
	ToonixLineCustomType[0] = L"ToonixLine";
	*/

	st = nodeDef.AddOutputPort(ID_OUT_Scale,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextSingleton,L"Scale",L"Scale",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;
	st = nodeDef.AddOutputPort(ID_OUT_Orientation,siICENodeDataRotation,siICENodeStructureArray,siICENodeContextSingleton,L"Orientation",L"Orientation",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;
	st = nodeDef.AddOutputPort(ID_OUT_Position,siICENodeDataVector3,siICENodeStructureArray,siICENodeContextSingleton,L"Position",L"Position",ID_UNDEF,ID_UNDEF,ID_CTXT_CNS);
	st.AssertSucceeded( ) ;

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Toonix");

	return CStatus::OK;
}

SICALLBACK ToonixBVH_BeginEvaluate( ICENodeContext& in_ctxt )
{
	Application().LogMessage(L" ToonixBVH BeginEValuate Called...");
	// Get KDTree from User Data
	CValue userData = in_ctxt.GetUserData();
	TXGeometry* geom = (TXGeometry*)(CValue::siPtrType)in_ctxt.GetUserData( );

	// Get Datas
	CDataArrayLong depthData(in_ctxt, ID_IN_Depth);
	CDataArrayLong maxData(in_ctxt, ID_IN_MaxNbPoints);

	// Get geometry object from the input port
	CICEGeometry icegeom( in_ctxt, ID_IN_Geometry );

	geom->GetPointPosition(icegeom);

	if(geom->m_init == false)
		geom->GetCachedMeshData(icegeom);

	geom->GetRuntimeMeshData(icegeom);
	/*
	CDoubleArray positions;
	geom.GetPointPositions(positions);
	ULONG nbp = geom.GetPointPositionCount();
	float3* fPos = new float3[nbp];
	CVector3f vmin(10000,10000,10000);
	CVector3f vmax(-10000,-10000,-10000);
	for(int i=0;i<nbp;i++)
	{
		fPos[i].x = (float)positions[i*3];
		fPos[i].y = (float)positions[i*3+1];
		fPos[i].z = (float)positions[i*3+2];
		if(vmin.GetX()>fPos[i].x)vmin.PutX(fPos[i].x);
		if(vmin.GetY()>fPos[i].y)vmin.PutY(fPos[i].y);
		if(vmin.GetZ()>fPos[i].z)vmin.PutZ(fPos[i].z);
		if(vmax.GetX()<fPos[i].x)vmax.PutX(fPos[i].x);
		if(vmax.GetY()<fPos[i].y)vmax.PutY(fPos[i].y);
		if(vmax.GetZ()<fPos[i].z)vmax.PutZ(fPos[i].z);
		fPos[i].id = i;
	}
	tree->_min = vmin;
	tree->_max = vmax;
	tree->_geom = geom;

	tree->Build(fPos,nbp,tree,depthData[0],maxData[0],0);
	*/

	return CStatus::OK;
}

SICALLBACK ToonixBVH_Evaluate( ICENodeContext& in_ctxt )
{
	/*
	CValue userData = in_ctxt.GetUserData();
	TXLine* line = (TXLine*)(CValue::siPtrType)in_ctxt.GetUserData( );;

	// Get the input TXGeometry
	CDataArrayCustomType ToonixData( in_ctxt, ID_IN_ToonixData );

	CDataArrayCustomType::TData* pBufferToonixData;
	ULONG nSizeToonixData;
	ToonixData.GetData( 0,(const CDataArrayCustomType::TData**)&pBufferToonixData, nSizeToonixData );
	TXData* data = (TXData*)pBufferToonixData;

	//Empty Out if input is invalid
	if(!data){line->EmptyData();return CStatus::OK;}

	line->_geom = data->_geom;

	if(GetBoundaryDirtyState(in_ctxt)||line->_geom->_culledges)
	{
		// Get the parameters value
		CDataArrayFloat widthData(in_ctxt, ID_IN_Width);
		CDataArrayFloat breakData(in_ctxt, ID_IN_BreakAngle);
		CDataArrayFloat filterData(in_ctxt, ID_IN_FilterPoints);
		CDataArrayFloat extendData(in_ctxt, ID_IN_Extend);

		// pass it to TXLine object
		line->_width = widthData[0];
		line->_break = (float)DegreesToRadians(breakData[0]);
		line->_filterpoints = filterData[0];
		line->_extend = extendData[0];

		// build line
		line->GetBoundaries();
		line->Build(BOUNDARY);
	}

	// Get the output port array ...			
	CDataArrayCustomType outData( in_ctxt );
	
	XSI::CDataArrayCustomType::TData* pOutData = outData.Resize(0,sizeof(TXLine));
	::memcpy( pOutData, line, sizeof(TXLine) );   
	*/
	return CStatus::OK;
}

SICALLBACK ToonixBVH_Init( CRef& in_ctxt )
{	
	//Get Context from CRef
	Context ctxt(in_ctxt);

	// Build a new empty KDTree object
	//TXKDTree* tree = new TXKDTree(CVector3f(0,0,0),CVector3f(0,0,0));
	TXGeometry* geom = new TXGeometry();

	// Push GL Data
	TXGLKDTree* gltree = new TXGLKDTree(geom->m_kdtree);
	gltree->SetID(G_TXGLShapeManager.GetUniqueID());
	G_TXGLShapeManager.AddShape(gltree);

	// Store Line in User Data
	ctxt.PutUserData((CValue::siPtrType)geom);
	return CStatus::OK;
}

SICALLBACK ToonixBVH_Term( CRef& in_ctxt )
{
	//Get Context from CRef
	Context ctxt(in_ctxt);

	if(!ctxt.GetUserData().IsEmpty())
	{
		//TXKDTree* tree = (TXKDTree*)(CValue::siPtrType)ctxt.GetUserData( );
		TXGeometry* geom = (TXGeometry*)(CValue::siPtrType)ctxt.GetUserData();

		// Remove GL Data
		//_TXGLShapeManager.RemoveShape(geom->_oglid);

		delete geom;
		ctxt.PutUserData((CValue::siPtrType)NULL);
	}
	return CStatus::OK;
}
