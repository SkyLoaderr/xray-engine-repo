#ifndef BLENDER_BLUR_H
#define BLENDER_BLUR_H
#pragma once

class CBlender_Blur : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: blur";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  FS);
	virtual		void		Load			(IReader&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_Blur();
	virtual ~CBlender_Blur();
};

#endif //BLENDER_BLUR_H
