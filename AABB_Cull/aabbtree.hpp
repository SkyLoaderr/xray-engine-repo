//============================================================================
// aabbtree.hpp
//============================================================================

#ifndef AABBTREE
#define AABBTREE

#include "vect3d.hpp"

#ifndef TRITYPE
#define TRITYPE
  typedef Vect3d Tri[3];
#endif

class AABBtree // AABB-TREE class
{
 private:

  static Tri* Tris;                // THE GLOBAL MASTER LIST OF TRIANGLES
  static int MINTRISLIMIT;         // MIN # OF TRIS ALLOWED TO BE IN AABB (>0)

  int Subdivide();                 // USED TO BUILD AABB TREE
  void FindTightFit();
  void ReadInTris(char* FileName); // LOAD AABBtree WITH TRIS FROM A "TRIANGLE" FILE
  void BuildHierarchy();           // CONSTRUCT THE AABBtree HIERARCHY

 public:

  Vect3d MinExt, MaxExt;           // MIN/MAX EXTENTS OF AABB (IN WORLD COORDS)
  AABBtree *P,*N;                  // POINTERS TO CHILDREN (Pos,Neg)

  // ARRAY OF TRIANGLES CONTAINED IN AABB (ACTUALLY PTR INTO GLOBAL ARRAY OF TRIS)
  Tri* MyTris;
  int NumTris;

  AABBtree() { NumTris=0; P=N=NULL; };     // CHILD AABB CONSTRUCTOR
  AABBtree(char* FileName, int mintris=10) // TOP-LEVEL AABB CONSTRUCTOR
  {
    NumTris=0; P=N=NULL; MINTRISLIMIT=mintris; // INITIALIZATION
    printf("loading tris [%s]. . .", FileName); ReadInTris(FileName); printf("DONE!\n");
    printf("building AABBtree. . ."); BuildHierarchy(); printf("DONE!\n");
  };
  ~AABBtree(){ delete P;  delete N;  delete[] Tris; };
  int IsLeaf() { return (!P && !N); }
};

#endif
