//-------------------------------------------------
// ToonixRegister.h
//-------------------------------------------------
#ifndef TOONIX_REGISTER_H
#define TOONIX_REGISTER_H

#include <sstream>
#include <string>
// we can safely ignore this warning, since we know that the 
// implementation of these functions will be done in C++
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
#include <xsi_plane.h>
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
#include <xsi_shader.h>
#include <xsi_model.h>

#include <vector>
#include <algorithm>
#include <math.h>
#include <string.h>

using namespace XSI; 
using namespace MATH;

#define MAX(a,b)	((a)<(b) ? (b)  : (a))
#define MIN(a,b)	((a)>(b) ? (b)  : (a))
#define ABS(a)		((a)<(0) ? (-a) : (a))

//-------------------------------------------------
// Constants
//-------------------------------------------------
static unsigned char ToonixDataR = 222;
static unsigned char ToonixDataG = 222;
static unsigned char ToonixDataB = 222;

static unsigned char ToonixLineR = 200;
static unsigned char ToonixLineG = 200;
static unsigned char ToonixLineB = 200;

static unsigned char ToonixNodeR = 188;
static unsigned char ToonixNodeG = 188;
static unsigned char ToonixNodeB = 188;

// templated useful functions
template <typename T>
std::string to_string(T const& value) {
	std::stringstream sstr;
	sstr << value;
	return sstr.str();
}

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

// a=target variable, b=bit number to act upon 0-n 
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

// x=target variable, y=mask
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK(x,y) (((x) & (y)) == (y))

#define RESCALE(value, inmin, inmax, outmin, outmax) (value -inmin)*(outmax-outmin)/(inmax-inmin)+outmin

#define F32_MAX 3.402823466e+38
#define F32_MIN 1.175494351e-38
#define F32_EPS 1e-6

#define LOG(msg) Application().LogMessage(CString(msg));
#define LOGWITHSEVERITY(msg, severity) Application().LogMessage(CString(msg), severity);

#endif // TOONIX_REGISTER_H

