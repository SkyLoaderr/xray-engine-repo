#ifndef BLENDER_EDITOR_WIRE_H
#define BLENDER_EDITOR_WIRE_H
#pragma once

#include "Blender.h"

class ENGINE_API CBlender_Editor_Wire : public CBlender  
{
	string64	oT_Factor;
public:
	virtual		LPCSTR		getComment()	{ return "EDITOR: wire";	}

	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_Editor_Wire();
	virtual ~CBlender_Editor_Wire();
};

#endif //BLENDER_EDITOR_WIRE_H
