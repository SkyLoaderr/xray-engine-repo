#include "stdafx.h"
#include "RAPID.h"
#include "RAPID_matvec.h"
#include "RAPID_overlap.h"
#include "RAPID_obb.h"

extern int RAPID_initialized;
void RAPID_initialize();

RAPID_model::RAPID_model()
{
  if (!RAPID_initialized) RAPID_initialize();

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
  if (!RAPID_initialized) RAPID_initialize();

  // the boxes pointed to should be deleted.
  _DELETEARRAY(b);

  // the triangles pointed to should be deleted.
  _DELETEARRAY(tris);

  _DELETEARRAY(tri_index);
}

int RAPID_initialized = 0;

void RAPID_initialize()
{
  RAPID_num_box_tests	= 0;
  RAPID_num_contacts	= 0;
  RAPID_contact			= 0;

  RAPID_initialized		= 1;
}

void RAPID_destroy()
{
  _DELETEARRAY(RAPID_contact);
}

int RAPID_model::SetOption(RAPID_option opt, int val)
{
  if (opt == RAPID_BUILD_NUM_TRIS_PER_LEAF)
    {
      build_opt_num_tris_per_leaf = val;
    }
  else if (opt == RAPID_BUILD_MAX_LEVELS_DEEP)
    {
      build_opt_max_levels_deep = val;
    }
  else
    {
      return RAPID_UNKNOWN_OPTION;
    }
  return RAPID_OK;
}

RAPID_model::UnsetOption(RAPID_option opt)
{
  if (opt == RAPID_BUILD_NUM_TRIS_PER_LEAF)
    {
      build_opt_num_tris_per_leaf = 1;
    }
  else if (opt == RAPID_BUILD_MAX_LEVELS_DEEP)
    {
      build_opt_max_levels_deep = 1000;
    }
  else
    {
      return RAPID_UNKNOWN_OPTION;
    }

  return RAPID_OK;
}

