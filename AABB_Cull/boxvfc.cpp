//===========================================================================
// boxvfc.cpp : AABB (axis-aligned bounding box) View Frustum Culler (VFC)
//===========================================================================

#include "stdafx.h"
#include "boxvfc.hpp"


//----------------------------------------------------------------------------
// STORAGE FOR THE VIEW-FRUSTUM (ASSUMED TO BE A SET OF FOUR "SIDE" PLANES
//----------------------------------------------------------------------------
Plane Planes[4];    // PLANES OF THE FRUSTUM
Vect3d ProjDirs[4]; // DIRECTIONS OF PROJECTORS ALONG CORNERS OF FRUSTUM
Vect3d COP;         // CENTER-OF-PROJECTION FOR THE FRUSTUM (WORLD COORDS)


//--------------------------------------------------------------------------
// SET THE CURRENT FRUSTUM : initializes the frustum for subsequent box
//   overlap tests. Yfov in degrees, Aspect=Width/Height.
//--------------------------------------------------------------------------
void SetFrustum(Vect3d& Eye, Vect3d& ViewDir, Vect3d& ViewUp, float Yfov, float Aspect)
{
  // CALC X-AXIS (ViewHoriz) AND STORE COP
  Vect3d ViewHoriz = ViewDir/ViewUp;   ViewHoriz.Normalize();
  COP = Eye;

  // CALC X AND Y FOV'S IN DEGREES
  Yfov *= 0.0174532f;
  float Xfov = Yfov*Aspect;

  // CALC WINDOW EXTENTS IN CAMERA COORDS
  float wR=tanf(Xfov*0.5f), wL=-wR, wT=tanf(Yfov*0.5f), wB=-wT;

  // CALCULATE THE CORNER VERTICES OF THE WINDOW
  Vect3d sPts[4];  // SILHOUETTE POINTS (CORNERS OF WINDOW)
  Vect3d Offset = ViewDir + COP;
  sPts[0] = ViewHoriz*wR + ViewUp*wT + Offset;
  sPts[1] = ViewHoriz*wL + ViewUp*wT + Offset;
  sPts[2] = ViewHoriz*wL + ViewUp*wB + Offset;
  sPts[3] = ViewHoriz*wR + ViewUp*wB + Offset;

  // FIND PROJECTOR DIRECTION VECTORS (FROM COP THROUGH SILHOUETTE Pts)
  ProjDirs[0] = sPts[0] - COP;
  ProjDirs[1] = sPts[1] - COP;
  ProjDirs[2] = sPts[2] - COP;
  ProjDirs[3] = sPts[3] - COP;

  // CALC FRUSTUM "SIDE" PLANE EQS (NEAR AND FAR ALREADY DEFINED)
  Planes[0].CalcCoeffs(COP, sPts[0], sPts[1]);  // TOP
  Planes[1].CalcCoeffs(COP, sPts[1], sPts[2]);  // LEFT
  Planes[2].CalcCoeffs(COP, sPts[2], sPts[3]);  // BOTTOM
  Planes[3].CalcCoeffs(COP, sPts[3], sPts[0]);  // RIGHT
}


//--------------------------------------------------------------------------
// View-frustum/AABB overlap test: given the extents of the AABB returns
//  the type of overlap determined (complete in, partial, complete out)
//  m and M are the min and max extents of the AABB respectively.
//--------------------------------------------------------------------------
int BoxOverlapVF(Vect3d& m, Vect3d& M)
{
  // GO FOR TRIVIAL REJECTION OR ACCEPTANCE USING "FASTER OVERLAP TEST"
  int CompletelyIn=1;    // ASSUME COMPLETELY IN UNTIL ONE COUNTEREXAMPLE
  int R;                 // TEST RETURN VALUE
  R=OverlapPlane(Planes[0],m,M);
  if(R==COMPLETEOUT) return(COMPLETEOUT); else if(R==PARTIAL) CompletelyIn=0;
  R=OverlapPlane(Planes[1],m,M);
  if(R==COMPLETEOUT) return(COMPLETEOUT); else if(R==PARTIAL) CompletelyIn=0;
  R=OverlapPlane(Planes[2],m,M);
  if(R==COMPLETEOUT) return(COMPLETEOUT); else if(R==PARTIAL) CompletelyIn=0;
  R=OverlapPlane(Planes[3],m,M);
  if(R==COMPLETEOUT) return(COMPLETEOUT); else if(R==PARTIAL) CompletelyIn=0;

  if (CompletelyIn) return(COMPLETEIN);  // CHECK IF STILL COMPLETELY "IN"

  // TEST FOR VIEW-FRUSTUM EDGE PROTRUSION THROUGH AABB FACE (PROJECTORS ONLY)
  float InT, OutT;
  if (RayIsectAABB(COP,ProjDirs[0],m,M,InT,OutT)) return(PARTIAL);
  if (RayIsectAABB(COP,ProjDirs[1],m,M,InT,OutT)) return(PARTIAL);
  if (RayIsectAABB(COP,ProjDirs[2],m,M,InT,OutT)) return(PARTIAL);
  if (RayIsectAABB(COP,ProjDirs[3],m,M,InT,OutT)) return(PARTIAL);

  // TEST FOR PROTRUSION OF AABB EDGE THROUGH VIEW-FRUSTUM FACE
  if (EdgeIsectVF(Vect3d(M.x,M.y,M.z),Vect3d(M.x,M.y,m.z))) return(PARTIAL);
  if (EdgeIsectVF(Vect3d(m.x,M.y,M.z),Vect3d(m.x,M.y,m.z))) return(PARTIAL);
  if (EdgeIsectVF(Vect3d(m.x,m.y,M.z),Vect3d(m.x,m.y,m.z))) return(PARTIAL);
  if (EdgeIsectVF(Vect3d(M.x,m.y,M.z),Vect3d(M.x,m.y,m.z))) return(PARTIAL);
  if (EdgeIsectVF(Vect3d(M.x,M.y,M.z),Vect3d(M.x,m.y,M.z))) return(PARTIAL);
  if (EdgeIsectVF(Vect3d(M.x,m.y,M.z),Vect3d(m.x,m.y,M.z))) return(PARTIAL);
  if (EdgeIsectVF(Vect3d(m.x,m.y,M.z),Vect3d(m.x,M.y,M.z))) return(PARTIAL);
  if (EdgeIsectVF(Vect3d(m.x,M.y,M.z),Vect3d(M.x,M.y,M.z))) return(PARTIAL);
  if (EdgeIsectVF(Vect3d(M.x,M.y,m.z),Vect3d(M.x,m.y,m.z))) return(PARTIAL);
  if (EdgeIsectVF(Vect3d(M.x,m.y,m.z),Vect3d(m.x,m.y,m.z))) return(PARTIAL);
  if (EdgeIsectVF(Vect3d(m.x,m.y,m.z),Vect3d(m.x,M.y,m.z))) return(PARTIAL);
  if (EdgeIsectVF(Vect3d(m.x,M.y,m.z),Vect3d(M.x,M.y,m.z))) return(PARTIAL);

  // VF MUST BE COMPLETELY ENCLOSED SINCE PT IS NOT "OUT "OF ANY AABB PLANE.
  return(COMPLETEOUT);
};

//---------------------------------------------------------------------------
// Axis-Aligned Bounding Box / Plane overlap test (returns type of overlap)
// Finds the "closest" and "farthest" points from the plane with respect
// to the plane normal dir (the "extremes" of the aabb) and tests for overlap.
// m and M are the Min and Max extents of the AABB.
//---------------------------------------------------------------------------
inline int OverlapPlane(Plane& P, Vect3d& m, Vect3d& M)
{
  // CALC EXTREME PTS (Neg,Pos) ALONG NORMAL AXIS (Pos in dir of norm, etc.)
  Vect3d Neg, Pos;
  if(P.N.x>0)
    if(P.N.y>0) { if(P.N.z>0) { Pos.Set(M.x,M.y,M.z); Neg.Set(m.x,m.y,m.z); }
                         else { Pos.Set(M.x,M.y,m.z); Neg.Set(m.x,m.y,M.z); } }
           else { if(P.N.z>0) { Pos.Set(M.x,m.y,M.z); Neg.Set(m.x,M.y,m.z); }
                         else { Pos.Set(M.x,m.y,m.z); Neg.Set(m.x,M.y,M.z); } }
  else
    if(P.N.y>0) { if(P.N.z>0) { Pos.Set(m.x,M.y,M.z); Neg.Set(M.x,m.y,m.z); }
                         else { Pos.Set(m.x,M.y,m.z); Neg.Set(M.x,m.y,M.z); } }
           else { if(P.N.z>0) { Pos.Set(m.x,m.y,M.z); Neg.Set(M.x,M.y,m.z); }
                         else { Pos.Set(m.x,m.y,m.z); Neg.Set(M.x,M.y,M.z); } }

  // CHECK DISTANCE TO PLANE FROM EXTREMAL POINTS TO DETERMINE OVERLAP
  if (P.DistToPt(Neg) > 0) return(COMPLETEOUT);
  else if (P.DistToPt(Pos) <= 0) return(COMPLETEIN);
  else return(PARTIAL);
}


//--------------------------------------------------------------------------
// Ray-AABB intersection test. returns In-Out HitTimes
//--------------------------------------------------------------------------
inline int RayIsectAABB(Vect3d& Start, Vect3d& Dir, Vect3d& Min, Vect3d& Max,
                        float& InT, float& OutT)
{
  InT=-99999, OutT=99999;    // INIT INTERVAL T-VAL ENDPTS TO -/+ "INFINITY"
  float NewInT, NewOutT;     // STORAGE FOR NEW T VALUES

  // X-SLAB (PARALLEL PLANES PERPENDICULAR TO X-AXIS) INTERSECTION (Xaxis is Normal)
  if (Dir.x == 0)            // CHECK IF RAY IS PARALLEL TO THE SLAB PLANES
  { if ((Start.x < Min.x) || (Start.x > Max.x)) return(0); }
  else
  {
    NewInT = (Min.x-Start.x)/Dir.x;  // CALC Tval ENTERING MIN PLANE
    NewOutT = (Max.x-Start.x)/Dir.x; // CALC Tval ENTERING MAX PLANE
    if (NewOutT>NewInT) { if (NewInT>InT) InT=NewInT;  if (NewOutT<OutT) OutT=NewOutT; }
                   else { if (NewOutT>InT) InT=NewOutT;  if (NewInT<OutT) OutT=NewInT; }
    if (InT>OutT) return(0);
  }

  // Y-SLAB (PARALLEL PLANES PERPENDICULAR TO Y-AXIS) INTERSECTION (Yaxis is Normal)
  if (Dir.y == 0)                // CHECK IF RAY IS PARALLEL TO THE SLAB PLANES
  { if ((Start.y < Min.y) || (Start.y > Max.y)) return(0); }
  else
  {
    NewInT = (Min.y-Start.y)/Dir.y;  // CALC Tval ENTERING MIN PLANE
    NewOutT = (Max.y - Start.y)/Dir.y; // CALC Tval ENTERING MAX PLANE
    if (NewOutT>NewInT) { if (NewInT>InT) InT=NewInT;  if (NewOutT<OutT) OutT=NewOutT; }
                   else { if (NewOutT>InT) InT=NewOutT;  if (NewInT<OutT) OutT=NewInT; }
    if (InT>OutT) return(0);
  }

  // Z-SLAB (PARALLEL PLANES PERPENDICULAR TO Z-AXIS) INTERSECTION (Zaxis is Normal)
  if (Dir.z == 0)                // CHECK IF RAY IS PARALLEL TO THE SLAB PLANES
  { if ((Start.z < Min.z) || (Start.z > Max.z)) return(0); }
  else
  {
    NewInT = (Min.z-Start.z)/Dir.z;  // CALC Tval ENTERING MIN PLANE
    NewOutT = (Max.z-Start.z)/Dir.z; // CALC Tval ENTERING MAX PLANE
    if (NewOutT>NewInT) { if (NewInT>InT) InT=NewInT;  if (NewOutT<OutT) OutT=NewOutT; }
                   else { if (NewOutT>InT) InT=NewOutT;  if (NewInT<OutT) OutT=NewInT; }
    if (InT>OutT) return(0);
  }

  // CHECK IF INTERSECTIONS ARE "AT OR BEYOND" THE START OF THE RAY
  if (InT>=0 || OutT>=0) return(1); else return(0);
}


//--------------------------------------------------------------------------
// CHECKS IF AN EDGE AB INTERSECTS THE VIEW-FRUSTUM (JUST A SET OF 4 PLANES)
//--------------------------------------------------------------------------
inline int EdgeIsectVF(Vect3d& Start, Vect3d& End)
{
  Vect3d Dir = End-Start;            // CALC DIRECTION VECTOR OF EDGE
  float InT=-99999, OutT=99999;      // INIT INTERVAL T-VAL ENDPTS TO -/+ INFINITY
  float NdotDir, NdotStart;          // STORAGE FOR REPEATED CALCS NEEDED FOR NewT CALC
  float NewT;
  for (int i=0; i<4; i++)            // CHECK INTERSECTION AGAINST EACH VF PLANE
  {
    NdotDir = Planes[i].N * Dir;
    NdotStart = Planes[i].N * Start;
    if (NdotDir == 0)                // CHECK IF RAY IS PARALLEL TO THE SLAB PLANES
    {
      if (NdotStart > Planes[i].D) return(0); // IF STARTS "OUTSIDE", NO INTERSECTION
    }
    else
    {
      NewT = (Planes[i].D - NdotStart) / NdotDir;      // FIND HIT "TIME" (DISTANCE)
      if (NdotDir < 0) { if (NewT > InT) InT=NewT; }   // IF "FRONTFACING", MUST BE NEW IN "TIME"
                  else { if (NewT < OutT) OutT=NewT; } // IF "BACKFACING", MUST BE NEW OUT "TIME"
    }
    if (InT > OutT) return(0);   // CHECK FOR EARLY EXITS (INTERSECTION INTERVAL "OUTSIDE")
  }

  // IF AT LEAST ONE THE Tvals ARE IN THE INTERVAL [0,1] WE HAVE INTERSECTION
  if (InT>=0 && InT<=1) return(1);
  else if (OutT>=0 && OutT<=1) return(1);
  else return(0);
}

