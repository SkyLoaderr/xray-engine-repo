//===========================================================================
// boxvfc.hpp : AABB (axis-aligned bounding box) View Frustum Culler (VFC)
//===========================================================================

#include "vect3d.hpp"
#include "plane.hpp"

// POSSIBLE TYPES OF OVERLAP
#define COMPLETEIN -1
#define PARTIAL 0
#define COMPLETEOUT 1

void SetFrustum(Vect3d& Eye, Vect3d& ViewDir, Vect3d& ViewUp, float Yfov, float Aspect);
int BoxOverlapVF(Vect3d& Min, Vect3d& Max);

inline int OverlapPlane(Plane& P, Vect3d& m, Vect3d& M);
inline int RayIsectAABB(Vect3d& Start, Vect3d& Dir, Vect3d& Min, Vect3d& Max,
                        float& InT, float& OutT);
inline int EdgeIsectVF(Vect3d& Start, Vect3d& End);




