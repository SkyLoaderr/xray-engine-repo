#ifndef OBB_H
#define OBB_H

#pragma pack(push)
#pragma pack(8)
union ENGINE_API tri
{
	struct {
		float n [3];
		float p1[3], p2[3], p3[3];
	};
	struct {
		Fvector N,P1,P2,P3;
	};
};
#pragma pack(pop)


class ENGINE_API box
{
public:

  // placement in parent's space
  // box to parent space: x_m = pR*x_b + pT
  // parent to box space: x_b = pR.T()*(x_m - pT)
  float pR[3][3];
  float pT[3];

  // dimensions
  float d[3];			// this is "radius", that is,
						// half the measure of a side length

  box *P;				// points to but does not "own".
  box *N;

  int *tri_index;		// points to an array of integers, m->tri_index, which refer to m->trp
  int num_tris;

  int leaf()	{ return (!P && !N); }
  float size()	{ return d[0]; }

  int split_recurse(int *t, int n, int level);
  int split_recurse(int *t);               // specialized for leaf nodes
};

#define RAPID_BUILD_STATE_CONST		0		// "empty" state, after constructor
#define RAPID_BUILD_STATE_BEGIN		1		// after BeginModel()
#define RAPID_BUILD_STATE_ADDTRI	2		// after AddTri()
#define RAPID_BUILD_STATE_PROCESSED 3		// after EndModel()

#endif
