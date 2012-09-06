// ToonixRegister.h
//-------------------------------------------------
#ifndef _TOONIX_REGISTER_H_
#define _TOONIX_REGISTER_H_

#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>

#include <xsi_icenodecontext.h>
#include <xsi_icenodedef.h>
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
#include <xsi_random.h>
#include <xsi_kinematicstate.h>
#include <xsi_kinematics.h>
#include <xsi_port.h>
#include <xsi_operator.h>
#include <xsi_customoperator.h>
#include <xsi_operatorcontext.h>
#include <xsi_point.h>
#include <xsi_nurbsdata.h>
#include <xsi_polygonmesh.h>
#include <xsi_facet.h>
#include <xsi_selection.h>
#include <xsi_x3dobject.h>
#include <xsi_primitive.h>
#include <xsi_iceattribute.h>
#include <xsi_iceattributedataarray.h>
#include <xsi_iceattributedataarray2D.h>
#include <xsi_boolarray.h>
#include <xsi_indexset.h>
#include <xsi_graphicsequencer.h>
#include <xsi_graphicsequencercontext.h>

#include <vector>
#include <algorithm>
#include <math.h>
#include <string.h>

using namespace XSI; 
using namespace MATH;

#define MAX(a,b)	((a)<(b) ? (b)  : (a))
#define MIN(a,b)	((a)>(b) ? (b)  : (a))
#define ABS(a)		((a)<(0) ? (-a) : (a))



#endif
