//============================================================================
// plane.hpp : infinite plane class definition. Defines a plane in point-normal
//  form (N dot P = D OR Ax+By+Cz=D where (A,B,C) is the normal to the plane).
//  1) Calcs plane eq coefficients given 3 pts on the plane.
//  2) Performs "inside-outside" test given a pt ("outside" is the half-space
//     in the direction of the normal, "inside" is the opposite half-space
//     and the plane itself; includes the boundary for "inside").
//  3) Calculates the intersection of a ray with the plane given ray's start
//     point and direction vector.
//============================================================================

#ifndef PLANE
#define PLANE

#include <stdio.h>
#include "vect3d.hpp"

class Plane
{
 public:

  Vect3d N; // NORMALIZED NORMAL TO THE PLANE
  float D;  // DIST ALONG NORMAL TO THE PLANE (COULD BE NEG. DIST)

  // DEFAULT CONSTRUCTOR
  Plane() {};

  // CONSTRUCTOR 2: CALCS PLANE EQUATION COEFFICIENTS
  Plane(Vect3d A, Vect3d B, Vect3d C) { CalcCoeffs(A,B,C); };

  // ALLOWS ASSIGNMENT OF THE NORMAL AND D-VALUE
  void SetNandD(Vect3d n, float d) { N=n; D=d; };

  // CALCULATES PLANE EQUATION COEFFICIENTS GIVEN THREE VERTICES ON THE PLANE
  void CalcCoeffs(Vect3d A, Vect3d B, Vect3d C)
  {
    N = (B-A) / (C-A);  // NORMAL = AB X AC
    N.Normalize();      // REQUIRES NORMALIZED NORMAL
    D = N*A;            // EVALUATE EQUATION WITH NORM AND PT ON PLANE TO GET D
  };

  // CALCS PLANE EQ COEFFS GIVEN THE NORMALIZED NORMAL TO THE PLANE AND PT ON PLANE
  void CalcCoeffsGivenNormal(Vect3d Normal, Vect3d PtOnPlane)
  {
    N = Normal;      // STORE THE NORMALIZED NORMAL
    D = N*PtOnPlane; // EVALUATE EQUATION WITH NORM AND PT ON PLANE TO GET D
  };

  // RETURNS THE DIST TO PT FROM PLANE (+ in dir of normal, - in opposite dir)
  float DistToPt(Vect3d P) const { return( (N*P)-D ); };

  // PERFORMS "INSIDE-OUTSIDE" TEST, RETURNS "TRUE" IF "OUT" AND
  // "FALSE" IF IT IS "OUT" (OR "ON"). (ACTUALLY AND "OUT" TEST).
  int InOutTest(Vect3d P) const { return( (N*P)>D ); };

  // CALCULATES INTERSECTION OF THE GIVEN RAY WITH THE PLANE. RETURNS
  // "TRUE" IF THEY INTERSECT (ALONG WITH HITPT) AND "FALSE" IF THEY ARE PARALLEL.
  int RayIntersection(Vect3d Start, Vect3d Dir, Vect3d& HitPt) const
  {
    float Denom = N*Dir;               // DOT-PROD BETWEEN NORMAL AND RAY DIR
    if (Denom==(float)0) return(0);    // RAY AND PLANE ARE PARALLEL
    float Tval = (D-(N*Start))/Denom;  // CALC PARAMETRIC T-VAL
    if (Tval>=0)                       // IF INTERSECTION NOT "BEHIND" RAY
    {
      HitPt = Start + Dir*Tval;        // CALC HIT POINT
      return(1);                       // INTERSECTION FOUND!
    }
    return(0);
  };

  // CALCS INTERSECTION OF AN EDGE WITH THE PLANE. RETURNS "TRUE" IF
  // THEY INTERSECT (ALONG WITH HITPT) AND "FALSE" IF THEY DO NOT INTERSECT.
  int EdgeIntersection(Vect3d A, Vect3d B, Vect3d &HitPt) const
  {
    Vect3d Dir = B-A;  // CALC RAY DIRECTION
    float Denom = N*Dir;              // DOT-PROD BETWEEN NORMAL AND RAY DIR
    if (Denom==(float)0) return(0);   // RAY AND PLANE ARE PARALLEL
    float Tval = (D-(N*A))/Denom;     // CALC PARAMETRIC T-VAL
    if (Tval>=0 && Tval<=1)           // IF INTERSECTION WITHIN EDGE ENDPTS!
    {
      HitPt = A + Dir*Tval;           // CALC HITPOINT
      return(1);                      // INTERSECTION FOUND!
    }
    return(0);
  };

  void Print()
  {
    printf("N=(%.2f,%.2f,%.2f) D=(%.2f)\n", N.x,N.y,N.z,D);
  };
};

#endif
