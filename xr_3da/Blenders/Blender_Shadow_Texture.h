#ifndef BLENDER_SHADOW_TEX_H
#define BLENDER_SHADOW_TEX_H
#pragma once

#include "Blender.h"

class ENGINE_API CBlender_ShTex : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: shadow rendering";	}

	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_ShTex();
	virtual ~CBlender_ShTex();
};

#endif //BLENDER_SHADOW_TEX_H
