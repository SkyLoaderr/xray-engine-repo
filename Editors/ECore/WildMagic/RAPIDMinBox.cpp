#include "stdafx.h"
#pragma hdrstop

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

