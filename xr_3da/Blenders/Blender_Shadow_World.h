#ifndef BLENDER_SHADOW_WORLD_H
#define BLENDER_SHADOW_WORLD_H
#pragma once

#include "Blender.h"

class ENGINE_API CBlender_ShWorld : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: shadow projecting";	}

	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_ShWorld();
	virtual ~CBlender_ShWorld();
};

#endif //BLENDER_SHADOW_WORLD_H
