#ifndef BLENDER_SHADOW_WORLD_H
#define BLENDER_SHADOW_WORLD_H
#pragma once

class CBlender_ShWorld : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: shadow projecting";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_ShWorld();
	virtual ~CBlender_ShWorld();
};

#endif //BLENDER_SHADOW_WORLD_H
