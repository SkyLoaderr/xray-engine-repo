#ifndef BLENDER_SHADOW_TEX_H
#define BLENDER_SHADOW_TEX_H
#pragma once

#include "Blender.h"

class CBlender_ShTex : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: shadow rendering";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  FS);
	virtual		void		Load			(IReader&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_ShTex();
	virtual ~CBlender_ShTex();
};

#endif //BLENDER_SHADOW_TEX_H
