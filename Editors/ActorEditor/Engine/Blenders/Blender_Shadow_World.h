#ifndef BLENDER_SHADOW_WORLD_H
#define BLENDER_SHADOW_WORLD_H
#pragma once

#include "Blender.h"

class ENGINE_API CBlender_ShWorld : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: shadow projecting";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  FS);
	virtual		void		Load			(IReader&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_ShWorld();
	virtual ~CBlender_ShWorld();
};

#endif //BLENDER_SHADOW_WORLD_H
