#ifndef D_TRI_COLLIDER_COMMON
#define D_TRI_COLLIDER_COMMON

#include "../Level.h"
#include "../ode_include.h"
#include "../ExtendedGeom.h"
#include "dTriColliderMath.h"
enum{
	fl_engaged_v0 =1<<0,
	fl_engaged_v1 =1<<1,
	fl_engaged_v2 =1<<2,
	fl_engaged_s0 =1<<3,
	fl_engaged_s1 =1<<4,
	fl_engaged_s2 =1<<5,
};

extern xr_vector< flags8 >			gl_cl_tries_state	;
extern xr_vector<int>::iterator		I,E,B				;
void VxToGlClTriState(u32 v);
void SideToGlClTriState(u32 v0,u32 v1);
void dSetTriWallMark(dContactGeom* c,CDB::TRI* T);
#define CONTACT(Ptr, Stride) ((dContactGeom*) (((byte*)Ptr) + (Stride)))
#define SURFACE(Ptr, Stride) ((dSurfaceParameters*) (((byte*)Ptr) + (Stride-sizeof(dSurfaceParameters))))
#define NUMC_MASK (0xffff)

#define M_SIN_PI_3		REAL(0.8660254037844386467637231707529362)
#define M_COS_PI_3		REAL(0.5000000000000000000000000000000000)
#endif
