#include "stdafx.h"
#pragma hdrstop

#include "cl_RAPID.h"
#include "cl_obb.h"

XRCollide XRC;

RAPID_model::RAPID_model()
{
  b = 0;
  num_boxes_alloced = 0;
  tri_index = 0;

  tris = 0;
  num_tris = 0;
  num_tris_alloced = 0;
  build_state = RAPID_BUILD_STATE_CONST;

  // these default options should build the tree right to the bottom
  build_opt_max_levels_deep = 1000;
  build_opt_num_tris_per_leaf = 1;
}

RAPID_model::~RAPID_model()
{
  // the boxes pointed to should be deleted.
  _DELETEARRAY(b);
  // the triangles pointed to should be deleted.
  _DELETEARRAY(tris);
  _DELETEARRAY(tri_index);
}

int RAPID_model::SetOption(RAPID_option opt, int val)
{
    if (opt == RAPID_BUILD_NUM_TRIS_PER_LEAF){
        build_opt_num_tris_per_leaf = val;
    }else if (opt == RAPID_BUILD_MAX_LEVELS_DEEP){
        build_opt_max_levels_deep = val;
    }else{
        return RAPID_UNKNOWN_OPTION;
    }
    return RAPID_OK;
}

RAPID_model::UnsetOption(RAPID_option opt)
{
    if (opt == RAPID_BUILD_NUM_TRIS_PER_LEAF){
        build_opt_num_tris_per_leaf = 1;
    }else if (opt == RAPID_BUILD_MAX_LEVELS_DEEP){
        build_opt_max_levels_deep = 1000;
    }else{
        return RAPID_UNKNOWN_OPTION;
    }
    return RAPID_OK;
}

XRCollide::XRCollide()
{
    bFirstContact       = 0;
    model1              = 0;
    model2              = 0;

    num_box_tests       = 0.f;
    num_tri_tests       = 0.f;
    min_raypick_id      = -1;
}

