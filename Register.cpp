// Toonix Register.cpp
//--------------------------------------------

#include "Register.h"
#include "Data.h"

#ifdef _WIN32
#include <windows.h>
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved )
{
    switch (ul_reason_for_call)
    {
      case DLL_PROCESS_ATTACH:
      case DLL_THREAD_ATTACH:
      case DLL_THREAD_DETACH:
      case DLL_PROCESS_DETACH:
	    break;
    }
    return TRUE;
}
#endif

XSI::CStatus RegisterToonixGetData( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixBVH( XSI::PluginRegistrar& in_reg);
XSI::CStatus RegisterToonixBoundary( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixCrease( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixCluster( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixSilhouette( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixIntersection( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixMesher( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixLineGet( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixLineSet( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixLineModify( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixLineMerge( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixLighter( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixPoints( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixStrands( XSI::PluginRegistrar& in_reg );
XSI::CStatus RegisterToonixHairs( XSI::PluginRegistrar& in_reg);

SICALLBACK XSILoadPlugin( PluginRegistrar& in_reg )
{
  in_reg.PutAuthor(L"benmalartre");
  in_reg.PutName(L"Toonix");
  in_reg.PutVersion(1,0);
	
  RegisterToonixGetData(in_reg);
  RegisterToonixBVH(in_reg);
  RegisterToonixBoundary(in_reg);
  RegisterToonixCrease(in_reg);
  RegisterToonixCluster(in_reg);
  RegisterToonixSilhouette(in_reg);
  RegisterToonixIntersection(in_reg);
  RegisterToonixMesher(in_reg);
  RegisterToonixLineGet(in_reg);
  RegisterToonixLineSet(in_reg);
  RegisterToonixLineModify(in_reg);
  RegisterToonixLineMerge(in_reg);
  RegisterToonixLighter(in_reg);
  RegisterToonixStrands(in_reg);
  RegisterToonixPoints(in_reg);
  RegisterToonixHairs(in_reg);

  return CStatus::OK;
}

SICALLBACK XSIUnloadPlugin( const PluginRegistrar& in_reg )
{
  CString strPluginName;
  strPluginName = in_reg.GetName();
  Application().LogMessage(strPluginName + L" has been unloaded.",siVerboseMsg);
  return CStatus::OK;
}

