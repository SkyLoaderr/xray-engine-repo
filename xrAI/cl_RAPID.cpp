#include "stdafx.h"
#pragma hdrstop

#include "cl_RAPID.h"
#include "cl_defs.h"

ENGINE_API RAPID::XRCollide XRC;

namespace RAPID {
	
	Model::Model()
	{
		b					= 0;
		num_boxes_alloced	= 0;

		tris				= 0;
		num_tris			= 0;
		tri_index			= 0;

		verts				= 0;
		num_verts			= 0;

		// these default options should build the tree right to the bottom
		build_opt_max_levels_deep	= 100000;
		build_opt_num_tris_per_leaf = 2;
	}
	
	Model::~Model()
	{
		_DELETEARRAY	(b);
		_DELETEARRAY	(tris);
		_DELETEARRAY	(tri_index);
		_DELETEARRAY	(verts);
	}
	
	void Model::SetOption(RAPID_option opt, int val)
	{
		if (opt == RAPID_BUILD_NUM_TRIS_PER_LEAF){
			build_opt_num_tris_per_leaf = val;
		}else if (opt == RAPID_BUILD_MAX_LEVELS_DEEP){
			build_opt_max_levels_deep = val;
		}
	}
	
	void Model::UnsetOption	(RAPID_option opt)
	{
		if (opt == RAPID_BUILD_NUM_TRIS_PER_LEAF){
			build_opt_num_tris_per_leaf = 2;
		}else if (opt == RAPID_BUILD_MAX_LEVELS_DEEP){
			build_opt_max_levels_deep = 100000;
		}
	}
	
	XRCollide::XRCollide	()
	{
		ray_flags			= RAY_CULL;
		collide_flags		= 0;
		bbox_flags			= BBOX_TRITEST;
		
		model1              = 0;
		model2              = 0;
		
		min_raypick_id      = -1;
	}
	XRCollide::~XRCollide	()
	{

	}
};
