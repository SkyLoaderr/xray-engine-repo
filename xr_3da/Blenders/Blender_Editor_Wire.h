#ifndef BLENDER_EDITOR_WIRE_H
#define BLENDER_EDITOR_WIRE_H
#pragma once

#include "Blender.h"

class ENGINE_API CBlender_Editor_Wire : public CBlender  
{
	string64	oT_Factor;
public:
	virtual		LPCSTR		getComment()	{ return "editor wire";	}

	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS);

	virtual		void		Compile			(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices);

	CBlender_Editor_Wire();
	virtual ~CBlender_Editor_Wire();
};

#endif //BLENDER_EDITOR_WIRE_H
