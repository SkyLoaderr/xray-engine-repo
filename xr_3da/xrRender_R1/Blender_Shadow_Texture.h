#ifndef BLENDER_SHADOW_TEX_H
#define BLENDER_SHADOW_TEX_H
#pragma once

class CBlender_ShTex : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: shadow rendering";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_ShTex();
	virtual ~CBlender_ShTex();
};

#endif //BLENDER_SHADOW_TEX_H
