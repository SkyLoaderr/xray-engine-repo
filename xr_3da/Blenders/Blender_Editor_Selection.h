#ifndef BLENDER_EDITOR_SELECTION_H
#define BLENDER_EDITOR_SELECTION_H
#pragma once

#include "blender.h"

class ENGINE_API CBlender_Editor_Selection : public CBlender  
{
	string64	oT_Factor;
public:
	virtual		LPCSTR		getComment()	{ return "editor selection"; }
	
	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS);
	
	virtual		void		Compile			(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices);
	
	CBlender_Editor_Selection();
	virtual ~CBlender_Editor_Selection();

};

#endif //BLENDER_EDITOR_SELECTION_H
