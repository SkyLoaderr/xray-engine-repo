#include "stdafx.h"
#pragma hdrstop

#include "cl_RAPID.h"
#include "cl_moments.h"
#include "cl_obb.h"

int RAPID_model::BeginModel()
{
  int bs = build_state;

  // free whatever storage we had.  Remember, it's okay to delete null
  // pointers in C++, so we don't have to check them first.
  _DELETEARRAY(b);
  num_boxes_alloced = 0;
  _DELETEARRAY(tris);
  num_tris = 0;
  num_tris_alloced = 0;

  build_state = RAPID_BUILD_STATE_BEGIN;

  if (bs == RAPID_BUILD_STATE_CONST) return RAPID_OK;

  if (bs == RAPID_BUILD_STATE_PROCESSED) return RAPID_OK;

  if (bs == RAPID_BUILD_STATE_BEGIN)
    return RAPID_ERR_BUILD_OUT_OF_SEQUENCE;

  if (bs == RAPID_BUILD_STATE_ADDTRI)
    return RAPID_ERR_BUILD_OUT_OF_SEQUENCE;

  return RAPID_OK;
}

int RAPID_model::EndModel()
{
    if (num_tris == 0)  return RAPID_ERR_BUILD_EMPTY_MODEL;

    int myrc = build_hierarchy();
    if (myrc == RAPID_OK){
        // only change to processed state if successful.
        build_state = RAPID_BUILD_STATE_PROCESSED;
        return RAPID_OK;
    }   else return myrc;

}

int RAPID_model::AddAllTris( int count, void *p )
{
	if (build_state == RAPID_BUILD_STATE_PROCESSED){
		// client forgot to call BeginModel() before calling AddTri().
		return RAPID_ERR_BUILD_OUT_OF_SEQUENCE;
	}

	if( !count )			return RAPID_BAD_PARAMS;

	if ( num_tris ){
		_DELETEARRAY		( tris );
		num_tris_alloced	= 0;
		num_tris			= 0;
	}

	// decide on new size -- accounting for first time, where none are
	// allocated
	// make new array, and copy the old one to it
	tris					= new tri[count];

	// if we can't get any more space, return an error
	if (!tris){
		// we are leaving the model unchanged.
		return RAPID_ERR_MODEL_OUT_OF_MEMORY;
	}

	num_tris_alloced		= count;
	num_tris				= count;

	CopyMemory				( tris, p, count*sizeof(tri) );

	return RAPID_OK;
}

int RAPID_model::AddTri(tri& one)
{
	int myrc = RAPID_OK; // we'll return this unless a problem is found

	if (build_state == RAPID_BUILD_STATE_PROCESSED){
		// client forgot to call BeginModel() before calling AddTri().
		return RAPID_ERR_BUILD_OUT_OF_SEQUENCE;
	}

	// first make sure that we haven't filled up our allocation.
	// if we have, allocate a new array of twice the size, and copy
	// the old data to it.

	if (num_tris == num_tris_alloced){
		// decide on new size -- accounting for first time, where none are
		// allocated
		int n = num_tris_alloced*2;
		if (n == 0) n = 1;

		// make new array, and copy the old one to it
		tri *t = new tri[n];

		// if we can't get any more space, return an error
		if (!t){
			// we are leaving the model unchanged.
			return RAPID_ERR_MODEL_OUT_OF_MEMORY;
		}

		int i;
		for(i=0; i<num_tris; i++) t[i] = tris[i];

		// free the old array and reassign.
		delete [] tris;
		tris = t;

		// update the allocation counter.
		num_tris_alloced = n;
	}

	// now copy the new tri into the array
	CopyMemory( &tris[num_tris], &one, sizeof(tri) );
	
	// update the counter
	num_tris++;

	return myrc;
}

int RAPID_model::AddTri(Fvector& p1, Fvector& p2, Fvector& p3, DWORD link)
{
	int myrc = RAPID_OK; // we'll return this unless a problem is found

	if (build_state == RAPID_BUILD_STATE_PROCESSED){
		// client forgot to call BeginModel() before calling AddTri().
		return RAPID_ERR_BUILD_OUT_OF_SEQUENCE;
	}

	// first make sure that we haven't filled up our allocation.
	// if we have, allocate a new array of twice the size, and copy
	// the old data to it.

	if (num_tris == num_tris_alloced){
		// decide on new size -- accounting for first time, where none are
		// allocated
		int n = num_tris_alloced*2;
		if (n == 0) n = 1;

		// make new array, and copy the old one to it
		tri *t = new tri[n];

		int i;
		for(i=0; i<num_tris; i++) t[i] = tris[i];

		// free the old array and reassign.
		delete [] tris;
		tris = t;

		// update the allocation counter.
		num_tris_alloced = n;
	}

	// now copy the new tri into the array
	tris[num_tris].p1.set(p1);
	tris[num_tris].p2.set(p2);
	tris[num_tris].p3.set(p3);
	tris[num_tris].link = link;

	// update the counter
	num_tris++;

	return myrc;
}


// not a full sort -- just makes column 1 the largest
int eigen_and_sort1(Fmatrix33& evecs, Fmatrix33& cov)
{
	float t;
	Fvector evals;
	int n;

	n = evecs.Meigen( evals, cov);

	if (evals.z > evals.x){
		if (evals.z > evals.y){
			// 2 is largest, swap with column 0
			t = evecs.m[0][2];
			evecs.m[0][2] = evecs.m[0][0];
			evecs.m[0][0] = t;
			t = evecs.m[1][2];
			evecs.m[1][2] = evecs.m[1][0];
			evecs.m[1][0] = t;
			t = evecs.m[2][2];
			evecs.m[2][2] = evecs.m[2][0];
			evecs.m[2][0] = t;
		}else{
			// 1 is largest, swap with column 0
			t = evecs.m[0][1];
			evecs.m[0][1] = evecs.m[0][0];
			evecs.m[0][0] = t;
			t = evecs.m[1][1];
			evecs.m[1][1] = evecs.m[1][0];
			evecs.m[1][0] = t;
			t = evecs.m[2][1];
			evecs.m[2][1] = evecs.m[2][0];
			evecs.m[2][0] = t;
		}
	}else{
		if (evals.x > evals.y){
			// 0 is largest, do nothing
		}else{
  			// 1 is largest
			t = evecs.m[0][1];
			evecs.m[0][1] = evecs.m[0][0];
			evecs.m[0][0] = t;
			t = evecs.m[1][1];
			evecs.m[1][1] = evecs.m[1][0];
			evecs.m[1][0] = t;
			t = evecs.m[2][1];
			evecs.m[2][1] = evecs.m[2][0];
			evecs.m[2][0] = t;
		}
	}

	// we are returning the number of iterations Meigen took.
	// too many iterations means our chosen orientation is bad.
	return n;
}


__forceinline void minmax(float &mn, float &mx, float v)
{
	if (v < mn) mn = v;
	else if (v > mx) mx = v;
}


static moment *RAPID_moment = 0;
static tri *RAPID_tri = 0;
static box *RAPID_boxes = 0;
static int RAPID_boxes_inited = 0;
static RAPID_model *RAPID_building_model = 0;

/*

  There are <n> tri structures in an array starting at <t>.

  We are told that the mean point is <mp> and the orientation
  for the parent box will be <or>.  The split axis is to be the
  vector given by <ax>.

  <or>, <ax>, and <mp> are model space coordinates.

*/



int RAPID_model::build_hierarchy()
{
  // allocate the boxes and set the box list globals

	num_boxes_alloced = num_tris * 2;
	b = new box[num_boxes_alloced];
	if (b == 0) return RAPID_ERR_MODEL_OUT_OF_MEMORY;
	RAPID_boxes = b;
	RAPID_boxes_inited = 1;        // we are in process of initializing b[0].
	RAPID_building_model = this;   // this is the model under construction

  // Determine initial orientation, mean point, and splitting axis.

	int i;
	accum M;

  //  float F1[3];
  //  float S1[6];
	Fmatrix33 C;

	RAPID_moment = new moment[num_tris];
	if (RAPID_moment == 0){
		_DELETEARRAY(b);
		return RAPID_ERR_MODEL_OUT_OF_MEMORY;
    }
	compute_moments(RAPID_moment, tris, num_tris);

	clear_accum(M);
	for(i=0; i<num_tris; i++)
		accum_moment(M, RAPID_moment[i]);

	mean_from_accum(b[0].pT, M);
	covariance_from_accum(C, M);

	eigen_and_sort1(b[0].pR, C);

	// create the index list
	tri_index = new int[num_tris];
	if (tri_index == 0){
		_DELETEARRAY(b);
		_DELETEARRAY(RAPID_moment);
		return RAPID_ERR_MODEL_OUT_OF_MEMORY;
	}
	for(i=0; i<num_tris; i++) tri_index[i] = i;

	// set the tri pointer
	RAPID_tri = tris;

	// do the build
	int rc = b[0].split_recurse(tri_index, num_tris, 0);
	if (rc != RAPID_OK){
		_DELETEARRAY(b);
		_DELETEARRAY(RAPID_moment);
		_DELETEARRAY(tri_index);
		return RAPID_ERR_MODEL_OUT_OF_MEMORY;
	}

	// free the moment list
	_DELETEARRAY(RAPID_moment);

	// null the tri pointer
	RAPID_tri = 0;

	// null the model pointer
	RAPID_building_model = 0;

	return RAPID_OK;
}



__forceinline void reaccum_moments(accum &A, int *t, int n)
{
	clear_accum(A);
	for(int i=0; i<n; i++)
		accum_moment(A, RAPID_moment[t[i]]);
}


int box::split_recurse(int *t, int n, int level)
{
	// t refers to some subarray of length n of RAPID_model->tri_index

	// The orientation for this box is already assigned to this->pR.
	// The axis along which to split it will be column 0 of this->pR.
	// The mean point is passed in on this->pT.

	// When this routine completes, the position and orientation in model
	// space will be established, as well as its dimensions.  Child boxes
	// will be constructed and placed in the parent's CS.

	if (n == 1){
		return split_recurse(t);
    }

	// walk along the tris for the box, and do the following:
	//   1. collect the max and min of the vertices along the axes of <or>.
	//   2. decide which group the triangle goes in, performing appropriate swap.
	//   3. accumulate the mean point and covariance data for that triangle.

	accum M1, M2;
	Fmatrix33 C;
	Fvector c;
	Fvector minval, maxval;

	P = N = 0;
	tri_index = t;
	num_tris = n;

	int rc;   // for return code on procedure calls.
	int in;
	tri *ptr;
	int i;
	float axdmp;
	int n1 = 0;  // The number of tris in group 1.
	// Group 2 will have n - n1 tris.

	// project approximate mean point onto splitting axis, and get coord.
	axdmp = (pR.m[0][0] * pT.m[0] + pR.m[1][0] * pT.m[1] + pR.m[2][0] * pT.m[2]);

	clear_accum(M1);
	clear_accum(M2);

	c.MTxV(pR, RAPID_tri[t[0]].p1);
    minval.set(c);
    maxval.set(c);
	for(i=0; i<n; i++){
		in = t[i];
		ptr = RAPID_tri + in;

		c.MTxV(pR, ptr->p1);
		minmax(minval.x, maxval.x, c.x);
		minmax(minval.y, maxval.y, c.y);
		minmax(minval.z, maxval.z, c.z);

		c.MTxV(pR, ptr->p2);
		minmax(minval.x, maxval.x, c.x);
		minmax(minval.y, maxval.y, c.y);
		minmax(minval.z, maxval.z, c.z);

		c.MTxV(pR, ptr->p3);
		minmax(minval.x, maxval.x, c.x);
		minmax(minval.y, maxval.y, c.y);
		minmax(minval.z, maxval.z, c.z);

		// grab the mean point of the in'th triangle, project
		// it onto the splitting axis (1st column of pR) and
		// see where it lies with respect to axdmp.
		mean_from_moment(c, RAPID_moment[in]);

		if (((pR.m[0][0]*c.m[0] + pR.m[1][0]*c.m[1] + pR.m[2][0]*c.m[2]) < axdmp) && ((n!=2)) || ((n==2) && (i==0))){
			// accumulate first and second order moments for group 1
			accum_moment(M1, RAPID_moment[in]);

			// put it in group 1 by swapping t[i] with t[n1]
			int temp = t[i];
			t[i] = t[n1];
			t[n1] = temp;
			n1++;
		}else{
			// accumulate first and second order moments for group 2
			accum_moment(M2, RAPID_moment[in]);

			// leave it in group 2
			// do nothing...it happens by default
		}
	}

	// done using this->pT as a mean point.


	// error check!
	if ((n1 == 0) || (n1 == n)){
		// our partitioning has failed: all the triangles fell into just
		// one of the groups.  So, we arbitrarily partition them into
		// equal parts, and proceed.

		n1 = n/2;

		// now recompute accumulated stuff
		reaccum_moments(M1, t, n1);
		reaccum_moments(M2, t + n1, n - n1);
	}

	// With the max and min data, determine the center point and dimensions
	// of the parent box.

    c.add(minval,maxval);
    c.mul(0.5f);

	pT.x = c.m[0] * pR.m[0][0] + c.m[1] * pR.m[0][1] + c.m[2] * pR.m[0][2];
	pT.y = c.m[0] * pR.m[1][0] + c.m[1] * pR.m[1][1] + c.m[2] * pR.m[1][2];
	pT.z = c.m[0] * pR.m[2][0] + c.m[1] * pR.m[2][1] + c.m[2] * pR.m[2][2];
    d.sub(maxval,minval);
    d.mul(0.5f);

	// here, if we exit, we don't create the children, but this box is
	// complete (we just finished computing position, orientation, and size,
	// but the children are still null pointers).

	if (RAPID_building_model->build_opt_max_levels_deep <= (level+1)){
		return RAPID_OK;
	}
	if (RAPID_building_model->build_opt_num_tris_per_leaf > n){
		return RAPID_OK;
    }

	// get new boxes for children
	P = RAPID_boxes + RAPID_boxes_inited++;
	N = RAPID_boxes + RAPID_boxes_inited++;

	// Compute the orienations for the child boxes (eigenvectors of
	// covariance matrix).  Select the direction of maximum spread to be
	// the split axis for each child.

	Fmatrix33 tR;

	// do the P child

	if (n1 > 1){
		mean_from_accum(P->pT, M1);
		covariance_from_accum(C, M1);

		if (eigen_and_sort1(tR, C) > 30){
			// unable to find an orientation.  We'll just pick identity.
            tR.identity();
		}

		P->pR.set(tR);
		if ((rc = P->split_recurse(t, n1, level+1)) != RAPID_OK) return rc;
	}else{
		if ((rc = P->split_recurse(t)) != RAPID_OK) return rc;
    }
	C.set(P->pR);  P->pR.MTxM(pR, C);   // and F1
	c.sub(P->pT, pT);  P->pT.MTxV(pR, c);

	// do the N child

	if ((n-n1) > 1){
		mean_from_accum(N->pT, M2);
		covariance_from_accum (C, M2);

		if (eigen_and_sort1(tR, C) > 30){
			// unable to find an orientation.  We'll just pick identity.
			tR.identity();
		}

		N->pR.set(tR);
		if ((rc = N->split_recurse(t + n1, n - n1, level+1)) != RAPID_OK) return rc;
	}else{
		if ((rc = N->split_recurse(t+n1)) != RAPID_OK) return rc;
    }
	C.set(N->pR);       N->pR.MTxM(pR, C);
	c.sub(N->pT, pT);   N->pT.MTxV(pR, c);

	return RAPID_OK;
}

int box::split_recurse(int *t)
{
	// For a single triangle, orientation is easily determined.
	// The major axis is parallel to the longest edge.
	// The minor axis is normal to the triangle.
	// The in-between axis is determine by these two.

	// this->pR, this->d, and this->pT are set herein.

	P = N = 0;
	tri_index = t;

	tri *ptr = RAPID_tri + t[0];

	// Find the major axis: parallel to the longest edge.
	Fvector u12, u23, u31;

	// First compute the squared-lengths of each edge
	u12.sub(ptr->p1, ptr->p2);
	float d12 = u12.dotproduct(u12);
	u23.sub(ptr->p2, ptr->p3);
	float d23 = u23.dotproduct(u23);
	u31.sub(ptr->p3, ptr->p1);
	float d31 = u31.dotproduct(u31);

	// Find the edge of longest squared-length, normalize it to
	// unit length, and put result into a0.
	Fvector a0;
	float l;
	if (d12 > d23){
		if (d12 > d31){
			l = 1.0f / sqrtf(d12);
            a0.mul(u12,l);
		}else{
			l = 1.0f / sqrtf(d31);
            a0.mul(u31,l);
		}
	}else{
		if (d23 > d31){
			l = 1.0f / sqrtf(d23);
            a0.mul(u23,l);
		}else{
			l = 1.0f / sqrtf(d31);
            a0.mul(u31,l);
		}
	}

	// Now compute unit normal to triangle, and put into a2.
	Fvector a2;
	a2.crossproduct(u12, u23);
	l = 1.0f / a2.magnitude();  a2.mul(l);

	// a1 is a2 cross a0.
	Fvector a1;
	a1.crossproduct(a2, a0);

	// Now make the columns of this->pR the vectors a0, a1, and a2.
	pR.m[0][0] = a0.x;  pR.m[0][1] = a1.x;  pR.m[0][2] = a2.x;
	pR.m[1][0] = a0.y;  pR.m[1][1] = a1.y;  pR.m[1][2] = a2.y;
	pR.m[2][0] = a0.z;  pR.m[2][1] = a1.z;  pR.m[2][2] = a2.z;

	// Now compute the maximum and minimum extents of each vertex
	// along each of the box axes.  From this we will compute the
	// box center and box dimensions.
	Fvector minval, maxval;
	Fvector c;

	c.MTxV(pR, ptr->p1);
    minval.set(c);
    maxval.set(c);

	c.MTxV(pR, ptr->p2);
	minmax(minval.x, maxval.x, c.x);
	minmax(minval.y, maxval.y, c.y);
	minmax(minval.z, maxval.z, c.z);

	c.MTxV(pR, ptr->p3);
	minmax(minval.x, maxval.x, c.x);
	minmax(minval.y, maxval.y, c.y);
	minmax(minval.z, maxval.z, c.z);

	// With the max and min data, determine the center point and dimensions
	// of the box
    c.add(minval,maxval);
    c.mul(0.5f);

	pT.x = c.m[0] * pR.m[0][0] + c.m[1] * pR.m[0][1] + c.m[2] * pR.m[0][2];
	pT.y = c.m[0] * pR.m[1][0] + c.m[1] * pR.m[1][1] + c.m[2] * pR.m[1][2];
	pT.z = c.m[0] * pR.m[2][0] + c.m[1] * pR.m[2][1] + c.m[2] * pR.m[2][2];

    d.sub(maxval, minval);
    d.mul(0.5f);

	// Assign the one triangle to this box
	num_tris = 1;

	return RAPID_OK;
}

int ray::set_ray(Fvector& start, Fvector& dir, float _range)
{
    C.set(start);
	D.set(dir);
	range = _range;

    // make axis (a0-long, a1-right, a2-normal)
	Fvector a0, a1, a2;
    a0.set(D);
	if (1-fabsf(a0.y)<EPS)	a2.set(1,0,0);
	else            		a2.set(0,1,0);

    a1.crossproduct(a2,a0);
    a2.crossproduct(a0,a1);

	// Now make the columns of this->pR the vectors a0, a1, and a2.
	pR.m[0][0] = a0.x;  pR.m[0][1] = a1.x;  pR.m[0][2] = a2.x;
	pR.m[1][0] = a0.y;  pR.m[1][1] = a1.y;  pR.m[1][2] = a2.y;
	pR.m[2][0] = a0.z;  pR.m[2][1] = a1.z;  pR.m[2][2] = a2.z;

	// Now compute the maximum and minimum extents of each vertex
	// along each of the box axes.  From this we will compute the
	// box center and box dimensions.
    pT.set( D.x*range*.5f+C.x,
            D.y*range*.5f+C.y,
            D.z*range*.5f+C.z);
    d.set(range/2, 0.001f, 0.001f);

	return RAPID_OK;
}
