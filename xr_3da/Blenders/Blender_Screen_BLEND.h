// Blender_Screen_SET.h: interface for the Blender_Screen_SET class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_2)
#define AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_2
#pragma once

#include "blender.h"

class ENGINE_API CBlender_Screen_BLEND : public CBlender  
{
public:
	BP_Integer	oAREF;
public:
	virtual		LPCSTR		getComment()	{ return "screen-space blend "; }
	
	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS);
	
	virtual		void		Compile			(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices);
	
	CBlender_Screen_BLEND();
	virtual ~CBlender_Screen_BLEND();

};

#endif // !defined(AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_)
