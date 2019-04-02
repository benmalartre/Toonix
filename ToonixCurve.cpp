

//Toonix Silhouette
//------------------------------------------------
#include "ToonixRegister.h"
#include "ToonixGeometry.h"
#include "ToonixData.h"
#include "ToonixCommon.h"

// Defines port, group and map identifiers used for registering the ICENode
enum IDs
{
	ID_IN_Curves = 0,
	ID_IN_ViewPosition = 1,
	ID_IN_Width = 2,
	ID_IN_BreakAngle = 3,
	ID_IN_Extend = 4,
	ID_IN_FilterPoints = 5,
	ID_IN_Smooth = 6,
	ID_G_100 = 100,
	ID_OUT_ToonixLine = 201,
	ID_TYPE_CNS = 400,
	ID_STRUCT_CNS,
	ID_CTXT_CNS,
	ID_UNDEF = ULONG_MAX
};

CStatus RegisterToonixCurve(PluginRegistrar& in_reg)
{
	ICENodeDef nodeDef;
	nodeDef = Application().GetFactory().CreateICENodeDef(L"ToonixCurve", L"ToonixCurve");

	CStatus st;
	st = nodeDef.PutColor(ToonixNodeR, ToonixNodeG, ToonixNodeB);
	st.AssertSucceeded();

	st = nodeDef.PutThreadingModel(siICENodeSingleThreading);
	st.AssertSucceeded();

	// Add custom types definition.
	st = nodeDef.DefineCustomType(L"ToonixLine", L"ToonixLine", L"ToonixLine", ToonixLineR, ToonixLineG, ToonixLineB);
	st.AssertSucceeded();

	// Add input ports and groups.
	st = nodeDef.AddPortGroup(ID_G_100);
	st.AssertSucceeded();

	st = nodeDef.AddInputPort(ID_IN_Curves, ID_G_100, siICENodeDataGeometry, siICENodeStructureSingle, siICENodeContextSingleton, L"Curves", L"Curves");
	st.AssertSucceeded();

	st = nodeDef.AddInputPort(ID_IN_ViewPosition, ID_G_100, siICENodeDataVector3, siICENodeStructureAny, siICENodeContextSingleton, L"ViewPoint", L"ViewPoint");
	st.AssertSucceeded();

	st = nodeDef.AddInputPort(ID_IN_Width, ID_G_100, siICENodeDataFloat, siICENodeStructureSingle, siICENodeContextSingleton, L"Width", L"Width", 0.1);
	st.AssertSucceeded();

	st = nodeDef.AddInputPort(ID_IN_BreakAngle, ID_G_100, siICENodeDataFloat, siICENodeStructureSingle, siICENodeContextSingleton, L"BreakAngle", L"BreakAngle", 90.0);
	st.AssertSucceeded();

	st = nodeDef.AddInputPort(ID_IN_Extend, ID_G_100, siICENodeDataFloat, siICENodeStructureSingle, siICENodeContextSingleton, L"Extend", L"Extend", 0.0);
	st.AssertSucceeded();

	st = nodeDef.AddInputPort(ID_IN_FilterPoints, ID_G_100, siICENodeDataFloat, siICENodeStructureSingle, siICENodeContextSingleton, L"FilterPoints", L"FilterPoints", 0.0);
	st.AssertSucceeded();

	st = nodeDef.AddInputPort(ID_IN_Smooth, ID_G_100, siICENodeDataBool, siICENodeStructureSingle, siICENodeContextSingleton, L"SmoothSilhouette", L"SmoothSilhouette", true);
	st.AssertSucceeded();

	// Add output ports.
	CStringArray ToonixLineCustomType(1);
	ToonixLineCustomType[0] = L"ToonixLine";

	st = nodeDef.AddOutputPort(ID_OUT_ToonixLine, ToonixLineCustomType, siICENodeStructureSingle, siICENodeContextSingleton, L"ToonixLine", L"ToonixLine", ID_UNDEF, ID_UNDEF, ID_CTXT_CNS);
	st.AssertSucceeded();

	PluginItem nodeItem = in_reg.RegisterICENode(nodeDef);
	nodeItem.PutCategories(L"Toonix");

	return CStatus::OK;
}

SICALLBACK ToonixCurve_Evaluate(ICENodeContext& in_ctxt)
{
	// Get User Data
	TXLine* line = (TXLine*)(CValue::siPtrType)in_ctxt.GetUserData();

	// Get the output port array ...			
	CDataArrayCustomType outData(in_ctxt);

	XSI::CDataArrayCustomType::TData* pOutData = outData.Resize(0, sizeof(TXLine));
	::memcpy(pOutData, line, sizeof(TXLine));

	return CStatus::OK;
}

SICALLBACK ToonixCurve_BeginEvaluate(ICENodeContext& in_ctxt)
{
	CValue userData = in_ctxt.GetUserData();
	TXLine* line = (TXLine*)(CValue::siPtrType)in_ctxt.GetUserData();

	// Get geometry object from the input port
	CICEGeometry curve(in_ctxt, ID_IN_Curves);
	if (!curve.GetGeometryType() != CICEGeometry::siNurbsCurveType)
		LOGWITHSEVERITY("TOONIXCURVE : ONLY CURVE INPUT VALID", XSI::siSeverityType::siErrorMsg);

	// Get num sub curves
	ULONG subCurvesCount = curve.GetSubGeometryCount();
	LOG("NUM CURVES : " + CString(subCurvesCount));

	for (ULONG i = 0; i < subCurvesCount; i++) {
		CICEGeometry subCurve = curve.GetSubGeometry(i);
		LOG("SUB CURVE : " + CString(i));
	}
	
	/*
	//CDataArrayCustomType ToonixData(in_ctxt, ID_IN_ToonixData);

	CDataArrayCustomType::TData* pBufferToonixData;
	ULONG nSizeToonixData;
	ToonixData.GetData(0, (const CDataArrayCustomType::TData**)&pBufferToonixData, nSizeToonixData);
	TXData* data = (TXData*)pBufferToonixData;

	//Empty Out if input is invalid
	if (!data) { line->EmptyData(); return CStatus::OK; }

	// Get underlying TXGeometry
	line->m_geom = data->m_geom;

	// Get View Position and optionnal lights positions used for silhouette detection
	line->m_eye.clear();

	siICENodeDataType inPortType;
	siICENodeStructureType inPortStruct;
	siICENodeContextType inPortContext;

	in_ctxt.GetPortInfo(ID_IN_ViewPosition, inPortType, inPortStruct, inPortContext);
	if (inPortStruct == XSI::siICENodeStructureSingle)
	{
		CDataArrayVector3f viewPointData(in_ctxt, ID_IN_ViewPosition);
		line->m_eye.push_back(viewPointData[0]);
		line->m_nbv = 1;
	}
	else if (inPortStruct == XSI::siICENodeStructureArray)
	{
		CDataArray2DVector3f viewPointsData(in_ctxt, ID_IN_ViewPosition);
		CDataArray2DVector3f::Accessor viewPointData = viewPointsData[0];
		line->m_nbv = viewPointData.GetCount();

		for (ULONG v = 0; v<line->m_nbv; v++)
		{
			line->m_eye.push_back(viewPointData[v]);
		}
	}

	// Get View Bias used for silhouette detection
	line->m_bias.Resize(line->m_nbv);
	in_ctxt.GetPortInfo(ID_IN_ViewBias, inPortType, inPortStruct, inPortContext);
	if (inPortStruct == XSI::siICENodeStructureSingle)
	{
		CDataArrayFloat viewBias(in_ctxt, ID_IN_ViewBias);
		for (ULONG v = 0; v<line->m_nbv; v++)
		{
			line->m_bias[v] = viewBias[0];
		}

	}
	else if (inPortStruct == XSI::siICENodeStructureArray)
	{
		CDataArray2DFloat viewBiasData(in_ctxt, ID_IN_ViewBias);
		CDataArray2DFloat::Accessor viewBias = viewBiasData[0];
		for (ULONG v = 0; v<line->m_nbv; v++)
		{
			line->m_bias[v] = viewBias[v];
		}
	}
	// Get the parameters value
	CDataArrayFloat widthData(in_ctxt, ID_IN_Width);
	CDataArrayFloat breakData(in_ctxt, ID_IN_BreakAngle);
	CDataArrayFloat filterData(in_ctxt, ID_IN_FilterPoints);
	CDataArrayFloat extendData(in_ctxt, ID_IN_Extend);
	CDataArrayBool smoothData(in_ctxt, ID_IN_SmoothSilhouette);
	CDataArrayBool ogldrawData(in_ctxt, ID_IN_OGLDraw);

	// Pass them to line object
	line->m_width = widthData[0];
	line->m_break = (float)DegreesToRadians(breakData[0]);
	line->m_filterpoints = filterData[0];
	line->m_extend = extendData[0];
	line->m_smoothsilhouette = smoothData[0];
	line->m_ogldraw = ogldrawData[0];

	line->ClearChains();
	
	bool dual = data->_useoctree;
	if(dual)
	{
	line->SetDualMesh(data->_dualmesh);
	for(int v=0;v<line->_eye.size();v++)
	{
	line->GetDualSilhouettes((LONG)v);
	line->Build(v);
	}
	}

	else
	{
	
	for (int v = 0; v<line->m_eye.size(); v++)
	{
		line->GetSilhouettes(v);
		line->Build(v);
		//Application().LogMessage(L"Nb Chains : "+(CString)(ULONG)line->_chains.size());
	}
	//}
	*/
	return CStatus::OK;
}

SICALLBACK ToonixCurve_Init(CRef& in_ctxt)
{
	InitTXLineData(in_ctxt);
	return CStatus::OK;
}

SICALLBACK ToonixCurve_Term(CRef& in_ctxt)
{
	CleanTXLineData(in_ctxt);
	return CStatus::OK;
}
