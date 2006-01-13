#ifndef D_TRI_COLLIDER_COMMON
#define D_TRI_COLLIDER_COMMON

#include "../Level.h"
#include "../ode_include.h"
#include "../ExtendedGeom.h"
extern xr_vector<bool>				ignored_tries	;
extern xr_vector<int>::iterator		I,E,B			;
void AddToIgnoredTries(u32 v);
void dSetTriWallMark(dContactGeom* c,CDB::TRI* T);
#define CONTACT(Ptr, Stride) ((dContactGeom*) (((byte*)Ptr) + (Stride)))
#define SURFACE(Ptr, Stride) ((dSurfaceParameters*) (((byte*)Ptr) + (Stride-sizeof(dSurfaceParameters))))
#define NUMC_MASK (0xffff)

#define M_SIN_PI_3		REAL(0.8660254037844386467637231707529362)
#define M_COS_PI_3		REAL(0.5000000000000000000000000000000000)
#endif
