//============================================================================
// aabbtree.cpp
//============================================================================

#include "stdafx.h"
#include "aabbtree.hpp"

Tri* AABBtree::Tris;               // THE GLOBAL MASTER LIST OF TRIANGLES
int AABBtree::MINTRISLIMIT=10;     // MIN # OF TRIS ALLOWED TO BE IN AABB (>=0)

inline static float max(float a, float b, float c, float d)
  { if (b>a) a=b;  if (c>a) a=c;  if (d>a) a=d;  return a; }
inline static float min(float a, float b, float c, float d)
  { if (b<a) a=b;  if (c<a) a=c;  if (d<a) a=d;  return a; }
inline static float max(float a, float b, float c)
  { if (b>a) a=b;  if (c>a) a=c;  return a; }

void AABBtree::ReadInTris(char* FileName)
{
  FILE* fp = fopen(FileName,"r"); // OPEN THE "TRIANGLE" FILE
  if (fp==NULL) { printf("ERROR: Cannot open tris file [%s]!\n", FileName); exit(1); }
  fscanf(fp, "%d", &NumTris);     // READ IN # OF TRIS (GLOBAL NumTris)
  Tris = new Tri[NumTris];        // ALLOCATE GLOBAL TRIS ARRAY
  for (int i=0; i<NumTris; i++)   // READ IN THE TRIS
    fscanf(fp,"%f %f %f %f %f %f %f %f %f",
      &Tris[i][0].x,&Tris[i][0].y,&Tris[i][0].z,
      &Tris[i][1].x,&Tris[i][1].y,&Tris[i][1].z,
      &Tris[i][2].x,&Tris[i][2].y,&Tris[i][2].z);
  fclose(fp);
  MyTris=&(Tris[0]);  // "CONNECT" TOP-LEVEL AABB TO GLOBAL TRI ARRAY
}

void AABBtree::BuildHierarchy()
{
  FindTightFit();
  if (Subdivide()) { P->BuildHierarchy(); N->BuildHierarchy(); }
}

int AABBtree::Subdivide() // RETURNS "1" IF SUCCESSFUL SUBDIVISION, "0" IF NOT.
{
  // STOP SUBDIVIDING IF NOT "ENOUGH" TRIANGLES IN AABB.
  if (NumTris <= MINTRISLIMIT) return(0);

  // FIND LARGEST AXIS TO SPLIT ALONG (LONGEST DIM)
  Vect3d Dim = MaxExt-MinExt;           // CALC AABB DIMENSIONS
  float MaxD = max(Dim.x,Dim.y,Dim.z);  // RETURN LARGEST AXIS #
  if (MaxD == 0.0) return(0);           // NO SUBDIVISION POSSIBLE
  int sAxis = ( (MaxD==Dim.x) ? 0 : ((MaxD==Dim.y) ? 1 : 2) );

  // CALC THE SPLIT POINT AS THE BOX CENTER (SPLIT VALUE ALONG SPLIT AXIS)
  float SplitVal = MinExt[sAxis] + (Dim[sAxis] * 0.5f);

  // PARTITION OBJECTS INTO CHILDREN ACCORDING TO SPLITPT AND SPLIT AXIS
  int PnumTris=0;                       // COUNTER FOR P-CHILD TRIS
  Vect3d T;                             // TEMPORARY SWAP VARIABLE
  float Mid;                            // MIDDLE VALUE OF OBJECT ALONG SPLIT AXIS
  for (int i=0; i<NumTris; i++)         // PARTITION EACH OBJECT IN OBJ LIST
  {
    Mid = (MyTris[i][0][sAxis]+MyTris[i][1][sAxis]+MyTris[i][2][sAxis]) / 3.0f;
    if (Mid < SplitVal) // IF SO, SWAP TRIS
    {
      T=MyTris[PnumTris][0]; MyTris[PnumTris][0]=MyTris[i][0]; MyTris[i][0]=T;
      T=MyTris[PnumTris][1]; MyTris[PnumTris][1]=MyTris[i][1]; MyTris[i][1]=T;
      T=MyTris[PnumTris][2]; MyTris[PnumTris][2]=MyTris[i][2]; MyTris[i][2]=T;
      PnumTris++;
    }
  }
  if (PnumTris==0 || PnumTris==NumTris) // CHECK IF EITHER CHILD IS EMPTY
    return(0);                          // IF SO, DO NOT SUBDIVIDE!
  P = new AABBtree();                   // OTHERWISE, CREATE CHILD AABBtrees
  N = new AABBtree();
  P->MyTris = MyTris;                   // P-CHILD CONNECTS TO FRONT OF OBJ LIST
  P->NumTris = PnumTris;                // P-CHILD CONTAINS COUNTER # TRIS
  N->MyTris = &(MyTris[PnumTris]);      // N-CHILD CONNECTS TO "END" OF P
  N->NumTris = NumTris - PnumTris;      // N-CHILD CONTAINS REMAINING OBJS
  return(1);                            // SUBDIVISION SUCCESSFUL
}

void AABBtree::FindTightFit()
{
  Vect3d A, B, C;
  MinExt = MaxExt = MyTris[0][0];   // USE FIRST PT AS START MIN AND MAX
  for(int i=0; i<NumTris; i++)
  {
    A=MyTris[i][0];  B=MyTris[i][1];  C=MyTris[i][2];
    MaxExt.Set(max(MaxExt.x,A.x,B.x,C.x),max(MaxExt.y,A.y,B.y,C.y),max(MaxExt.z,A.z,B.z,C.z));
    MinExt.Set(min(MinExt.x,A.x,B.x,C.x),min(MinExt.y,A.y,B.y,C.y),min(MinExt.z,A.z,B.z,C.z));
  }
}

