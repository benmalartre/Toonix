//------------------------------------------------
// ToonixCommon
// common functions for Custom ICE Nodes
//------------------------------------------------
#include "ToonixCommon.h"
#include "ToonixGLManager.h"


//------------------------------------------------
// Line Data
//------------------------------------------------
void InitTXLineData(CRef& in_ctxt)
{
	//Get Context from CRef
	Context ctxt(in_ctxt);

	// Build a new empty line object
	TXLine* line = new TXLine();

	// Push GL Data
	TXGLLine* glline = new TXGLLine(line);
	line->SetUniqueID(G_TXGLShapeManager.GetUniqueID());
	G_TXGLShapeManager.AddShape(glline);

	// Store Line in User Data
	ctxt.PutUserData((CValue::siPtrType)line);
}

void CleanTXLineData(CRef& in_ctxt)
{
	//Get Context from CRef
	Context ctxt(in_ctxt);

	if(!ctxt.GetUserData().IsEmpty())
	{
		TXLine* line = (TXLine*)(CValue::siPtrType)ctxt.GetUserData( );

		// Remove GL Data
		G_TXGLShapeManager.RemoveShape(line->m_oglid);

		delete line;
		ctxt.PutUserData((CValue::siPtrType)NULL);
	}
}
