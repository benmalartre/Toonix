//------------------------------------------------
// ToonixCommon
// common functions for Custom ICE Nodes
//------------------------------------------------
#include "Line.h"


//------------------------------------------------
// Line Data
//------------------------------------------------
void InitTXLineData(CRef& in_ctxt)
{
  //Get Context from CRef
  Context ctxt(in_ctxt);

  // Build a new empty line object
  TXLine* line = new TXLine();

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
    delete line;
    ctxt.PutUserData((CValue::siPtrType)NULL);
  }
}
