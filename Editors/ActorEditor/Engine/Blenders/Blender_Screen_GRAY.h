// Blender_Screen_GRAY.h: interface for the CBlender_Screen_GRAY class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_SCREEN_GRAY_H__483E49EF_23EC_4810_9231_7EE4BD72CC3B__INCLUDED_)
#define AFX_BLENDER_SCREEN_GRAY_H__483E49EF_23EC_4810_9231_7EE4BD72CC3B__INCLUDED_
#pragma once

#include "blender.h"

class ENGINE_API CBlender_Screen_GRAY : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: gray-scale effect"; }
	
	virtual		void		Save			(IWriter&  FS);
	virtual		void		Load			(IReader&	FS, WORD version);
	
	virtual		void		Compile			(CBlender_Compile& C);
	
	CBlender_Screen_GRAY();
	virtual ~CBlender_Screen_GRAY();

};

#endif // !defined(AFX_BLENDER_SCREEN_GRAY_H__483E49EF_23EC_4810_9231_7EE4BD72CC3B__INCLUDED_)
