//============================================================================
// camera.hpp : camera class definition
//              used to define a perspective viewing frustum to cull against.
//  1) Defines and stores a "camera" composed of 3 coord axes, an origin for
//     these axes, a center-of-projection (in Cam coords), a near and far
//     plane (in Cam coords), and the viewplane window extents (projection
//     window defined in Cam coords on the Cam XY-plane).
//  2) Accomodates "off-axis" COPs and "off-center" viewplane window extents.
//  3) Gives users several options in "constructing" a camera and altering
//     its parameters. During animation, it is recommended to set the params
//     directly (they are public).
//============================================================================

#ifndef CAMERA
#define CAMERA

#include <stdio.h>
#include "vect3d.hpp"

class Camera
{
 public:

  Vect3d X, Y, Z;           // NORMALIZED CAMERA COORDINATE-AXIS VECTORS
  Vect3d Orig;              // LOCATION OF ORIGIN OF CAMERA SYSTEM IN WORLD COORDS
  Vect3d COP;               // CENTER-OF-PROJ (EYEPOINT) IN CAM COORDS
  float NearZ, FarZ, ViewZ; // NEAR, FAR, AND VIEW PLANE Z-VALUES IN CAM COORDS
  float wL, wR, wT, wB;     // VIEWPLANE WINDOW EXTENTS (DEFINED AS RECT ON CAM XY-plane)

  //----------------------------------------------------------------------------
  // CONSTRUCTOR: defines a default camera system defined as follows:
  //  Origin=(0,0,0), Cam Axes={(1,0,0),(0,1,0),(0,0,1)}, COP=(0,0,2) (45 DEG FOV),
  //  NF=(1,-1), LRTB=(-1,1,1,-1). User must "Set" the appropriate values.
  //----------------------------------------------------------------------------
  Camera()
  {
    X.Set(1,0,0); Y.Set(0,1,0); Z.Set(0,0,1);
    Orig.Set(0,0,0); COP.Set(0,0,2.4142);
    NearZ=1; FarZ=-1; ViewZ=0; wL=-1; wR=1; wT=1; wB=-1;
  }

  //----------------------------------------------------------------------------
  // OpenGL CAMERA ORIENTATION ROUTINE (glLookAt)
  //----------------------------------------------------------------------------
  void LookAt(Vect3d Eye, Vect3d ViewRefPt, Vect3d ViewUp)
  {
    Z = Eye-ViewRefPt;  Z.Normalize(); // CALC CAM AXES ("/" IS CROSS-PROD)
    X = ViewUp/Z;       X.Normalize();
    Y = Z/X;            Y.Normalize();
    Orig = Eye;
  }

  //----------------------------------------------------------------------------
  // GENERALIZED OpenGL PERSPECTIVE FRUSTUM DEFINITION ROUTINE (fov in radians)
  //----------------------------------------------------------------------------
  void GenPerspective(float Xfov, float Yfov, float Ndist, float Fdist)
  {
    COP.Set(0,0,0);
    NearZ=-Ndist;  FarZ=-Fdist;  ViewZ=-1;
    wR=tan(Xfov*0.5);  wL=-wR;  wT=tan(Yfov*0.5);  wB=-wT;
  }

  //----------------------------------------------------------------------------
  // GENERALIZED OpenGL PERSPECTIVE FRUSTUM DEFINITION ROUTINE (glPerspective)
  // Aspect = Width/Height; Yfov in degrees
  //----------------------------------------------------------------------------
  void Perspective(float Yfov, float Aspect, float Ndist, float Fdist)
  {
    Yfov *= 0.0174532;
    float Xfov = Yfov*Aspect;
    GenPerspective(Xfov,Yfov,Ndist,Fdist);
  }

  //----------------------------------------------------------------------------
  // OpenGL PERSPECTIVE FRUSTUM DEFINITION ROUTINE (glFrustum)
  //----------------------------------------------------------------------------
  void Frustum(float l, float r, float b, float t, float Ndist, float Fdist)
  {
    COP.Set(0,0,0);
    NearZ=-Ndist;  FarZ=-Fdist;  ViewZ=-Ndist;
    wR=r;  wL=l;  wB=b;  wT=t;
  }

  //----------------------------------------------------------------------------
  // Routines to set cam parameters.
  //----------------------------------------------------------------------------
  void SetAxes(Vect3d x, Vect3d y, Vect3d z) { X=x; Y=y; Z=z; };
  void SetOrig(Vect3d orig) { Orig=orig; };
  void SetCOP(Vect3d cop) { COP=cop; };
  void SetNearFar(float n, float f) { NearZ=n; FarZ=f; };
  void SetViewPlane(float v) { ViewZ=v; };
  void SetWindowExtents(float l, float r, float t, float b)
    { wL=l; wR=r; wT=t; wB=b; };
  void SetCOP(float Xfov)
    { COP.z = ViewZ + ( ((wR-wL)/2.0) / tan(Xfov/2.0) ); };

  //----------------------------------------------------------------------------
  // RETURNS THE COP IN WORLD COORDS
  //----------------------------------------------------------------------------
  Vect3d wCOP() const
  {
     return( X*COP.x + Y*COP.y + Z*COP.z + Orig );
  }

  //----------------------------------------------------------------------------
  // CAMERA animation routines (6DOF system). All movement is relative to
  // the current position. Rotations (Pitch,Yaw,Roll) are about the specified 
  // Axis passing thru the COP (EyePt) in world coords. These routines can
  // be thought of as occurring in "camera space", since a Pitch rotates
  // the camera about its own X-axis. Also, a translation of (1,0,0) does NOT
  // move the camera in world coords right by 1, but rather in camera coords;
  // so the corresponding move in world coords has to be calculated.
  //----------------------------------------------------------------------------
  void Pitch(float Rads)  // ABOUT CAM X
  {
    Vect3d WorldCOP = wCOP();  // STORE WORLD COP BEFORE AXES CHANGED
    float cR=(float)cos(Rads), sR=(float)sin(Rads);  // ADJUST AXES
    Vect3d NewY( cR*Y.x - sR*Z.x,  cR*Y.y - sR*Z.y,  cR*Y.z - sR*Z.z );
    Vect3d NewZ( sR*Y.x + cR*Z.x,  sR*Y.y + cR*Z.y,  sR*Y.z + cR*Z.z );
    Y=NewY;
    Z=NewZ;
    Orig = WorldCOP - ( X*COP.x + Y*COP.y + Z*COP.z );   // ADJUST ORIGIN
  }

  void Yaw(float Rads)    // ABOUT CAM Y
  {
    Vect3d WorldCOP = wCOP();  // STORE WORLD COP BEFORE AXES CHANGED
    float cR=(float)cos(Rads), sR=(float)sin(Rads);  // ADJUST AXES
    Vect3d NewX( cR*X.x - sR*Z.x,  cR*X.y - sR*Z.y,  cR*X.z - sR*Z.z );
    Vect3d NewZ( sR*X.x + cR*Z.x,  sR*X.y + cR*Z.y,  sR*X.z + cR*Z.z );
    X=NewX;
    Z=NewZ;
    Orig = WorldCOP - ( X*COP.x + Y*COP.y + Z*COP.z );   // ADJUST ORIGIN
  }

  void Roll(float Rads)   // ABOUT CAM Z
  {
    Vect3d WorldCOP = wCOP();  // STORE WORLD COP BEFORE AXES CHANGED
    float cR=(float)cos(Rads), sR=(float)sin(Rads);  // ADJUST AXES
    Vect3d NewX( cR*X.x + sR*Y.x,  cR*X.y + sR*Y.y,  cR*X.z + sR*Y.z );
    Vect3d NewY( cR*Y.x - sR*X.x,  cR*Y.y - sR*X.y,  cR*Y.z - sR*X.z );
    X=NewX;
    Y=NewY;
    Orig = WorldCOP - ( X*COP.x + Y*COP.y + Z*COP.z );   // ADJUST ORIGIN
  }

  void Move(Vect3d Trans) // RightLeft, UpDown, ForwBack RESPECTIVELY in Cam Coords
  {
    // FIND TRANSLATION IN WORLD COORDS AND UPDATE CAM ORIGIN
    Orig += ( X*Trans.x + Y*Trans.y + Z*Trans.z );
  }

  //----------------------------------------------------------------------------
  // ROTATE CAMERA ABOUT CENTER-OF-ROTATION IN WORLD COORDS (COR). 
  // cAxis={0,1,2}. EFFECTIVELY USED TO ROTATE "WORLDA" AS IN "TRACKBALL" MODE.
  //----------------------------------------------------------------------------
  void TrackBallRotate(Vect3d COR, float Rads, int cAxis)
  {
    // FIND COR IN CAM COORDS BEFORE ORIENTATION XFORM
    Vect3d cCORbefore = WorldToCam(COR);

    // DO OPPOSITE CAMERA ROTATION TO GET CAM IN PROPER ORIENTATION
    if (cAxis==0) Pitch(-Rads); else if (cAxis==1) Yaw(-Rads); else Roll(-Rads);

    // FIND COR IN CAM COORDS AFTER ORIENTATION XFORM
    Vect3d cCORafter = WorldToCam(COR);

    // NOW "FIX" THE ORIGIN OF THE CAM SO THAT COR IS IN SAME PLACE IN BOTH CAMS
    Vect3d Trans = cCORafter-cCORbefore;  // MOVEMENT IN CAM COORDS!
    Move(Trans);
  }

  //----------------------------------------------------------------------------
  // Vect3d WORLD-TO-CAM AND CAM-TO-WORLD ROUTINES
  //----------------------------------------------------------------------------
  Vect3d WorldToCam(Vect3d wP)
  {
    wP-=Orig;  Vect3d cP(X*wP,Y*wP,Z*wP);  return(cP);
  }

  Vect3d CamToWorld(Vect3d cP)
  {
    Vect3d wP = X*cP.x + Y*cP.y + Z*cP.z + Orig;  return(wP);
  }

  //----------------------------------------------------------------------------
  // "GENERAL" 4x3 matrix transformation to the camera. RowVector/PostMult
  // The 4x3 matrix is composed of only rotations and translations.
  //----------------------------------------------------------------------------
  void TransformM43(float M[4][3])
  {
    // APPLY ONLY 3x3 "ROTATIONS" TO AXES
    X.Set( X.x*M[0][0] + X.y*M[1][0] + X.z*M[2][0],
           X.x*M[0][1] + X.y*M[1][1] + X.z*M[2][1],
           X.x*M[0][2] + X.y*M[1][2] + X.z*M[2][2] );
    Y.Set( Y.x*M[0][0] + Y.y*M[1][0] + Y.z*M[2][0],
           Y.x*M[0][1] + Y.y*M[1][1] + Y.z*M[2][1],
           Y.x*M[0][2] + Y.y*M[1][2] + Y.z*M[2][2] );
    Z.Set( Z.x*M[0][0] + Z.y*M[1][0] + Z.z*M[2][0],
           Z.x*M[0][1] + Z.y*M[1][1] + Z.z*M[2][1],
           Z.x*M[0][2] + Z.y*M[1][2] + Z.z*M[2][2] );

    // APPLY ENTIRE TRANSFORMATION TO THE CAMERA ORIGIN
    Orig.Set( Orig.x*M[0][0] + Orig.y*M[1][0] + Orig.z*M[2][0] + M[3][0],
              Orig.x*M[0][1] + Orig.y*M[1][1] + Orig.z*M[2][1] + M[3][1],
              Orig.x*M[0][2] + Orig.y*M[1][2] + Orig.z*M[2][2] + M[3][2] );
  };

  //----------------------------------------------------------------------------
  // RETURNS AN OPENGL STYLE CAMERA VIEWING MATRIX (PREMULT/ROW VECT 4x4 MATRIX)
  //----------------------------------------------------------------------------
  float* GetViewMatrix(float* M)
  {
    Vect3d Tr = -Orig;
    M[0]=X.x;  M[4]=X.y;   M[8]=X.z; M[12]=X*Tr;
    M[1]=Y.x;  M[5]=Y.y;   M[9]=Y.z; M[13]=Y*Tr;
    M[2]=Z.x;  M[6]=Z.y;  M[10]=Z.z; M[14]=Z*Tr;
    M[3]=0;    M[7]=0;    M[11]=0;   M[15]=1;
    return(M);
  }

  //----------------------------------------------------------------------------
  // PRINT ROUTINE
  //----------------------------------------------------------------------------
  void Print()
  {
	  printf("Camera System Parameters:\n");
	  printf("       X: (%.3f, %.3f, %.3f)\n", X.x, X.y, X.z);
	  printf("       Y: (%.3f, %.3f, %.3f)\n", Y.x, Y.y, Y.z);
	  printf("       Z: (%.3f, %.3f, %.3f)\n", Z.x, Z.y, Z.z);
	  printf("  Origin: (%.3f, %.3f, %.3f)\n", Orig.x, Orig.y, Orig.z);
	  printf("     COP: (%.3f, %.3f, %.3f)\n", COP.x, COP.y, COP.z);
	  printf(" NFVLRTB: (%.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f)\n",
                 NearZ,FarZ,ViewZ,wL,wR,wT,wB);
  };
};

#endif
