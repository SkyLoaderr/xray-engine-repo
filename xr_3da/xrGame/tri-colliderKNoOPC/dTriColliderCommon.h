#ifndef D_TRI_COLLIDER_COMMON
#define D_TRI_COLLIDER_COMMON

#include "../ode_include.h"

void dSetTriWallMark(dContactGeom* c,CDB::TRI* T);
#include "../ExtendedGeom.h"
#define CONTACT(Ptr, Stride) ((dContactGeom*) (((byte*)Ptr) + (Stride)))
#define SURFACE(Ptr, Stride) ((dSurfaceParameters*) (((byte*)Ptr) + (Stride-sizeof(dSurfaceParameters))))
#define NUMC_MASK (0xffff)

#define M_SIN_PI_3		REAL(0.8660254037844386467637231707529362)
#define M_COS_PI_3		REAL(0.5000000000000000000000000000000000)
#endif
