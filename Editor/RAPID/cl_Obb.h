#ifndef OBB_H
#define OBB_H

class box
{
public:

  // placement in parent's space
  // box to parent space: x_m = pR*x_b + pT
  // parent to box space: x_b = pR.T()*(x_m - pT)
  Fmatrix33 pR;
  Fvector pT;

  // dimensions
  Fvector d;        // this is "radius", that is,
                      // half the measure of a side length

  box *P;  // points to but does not "own".
  box *N;

  int *tri_index;     // points to an array of integers, m->tri_index, which refer to m->trp
  int num_tris;

  int leaf() { return (!P && !N); }
  float size() { return d.x; }

  int split_recurse(int *t, int n, int level);
  int split_recurse(int *t);               // specialized for leaf nodes
};

class ray
{
public:

  // placement in parent's space
  // box to parent space: x_m = pR*x_b + pT
  // parent to box space: x_b = pR.T()*(x_m - pT)
  Fmatrix33 pR;
  Fvector pT;
  // dimensions
  Fvector d;        // this is "radius", that is,
                    // half the measure of a side length
  Fvector C;
  Fvector D;
  float range;

  float size() { return d.x; }

  int set_ray(Fvector& C, Fvector& D, float range);  // specialized for leaf nodes
};

#define RAPID_BUILD_STATE_CONST		0		// "empty" state, after constructor
#define RAPID_BUILD_STATE_BEGIN		1		// after BeginModel()
#define RAPID_BUILD_STATE_ADDTRI	2		// after AddTri()
#define RAPID_BUILD_STATE_PROCESSED 3		// after EndModel()

#endif
