// Blender_Screen_SET.h: interface for the Blender_Screen_SET class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_)
#define AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_
#pragma once

#include "blender.h"

class ENGINE_API CBlender_Screen_SET : public CBlender  
{
	BP_TOKEN	oBlend;
	BP_Integer	oAREF;
	BP_BOOL		oZTest;
	BP_BOOL		oZWrite;
	BP_BOOL		oLighting;
	BP_BOOL		oFog;
public:
	virtual		LPCSTR		getComment()	{ return "basic"; }
	
	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS);
	
	virtual		void		Compile			(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param=0, BOOL bEditor=FALSE);
	
	CBlender_Screen_SET();
	virtual ~CBlender_Screen_SET();
};

#endif // !defined(AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_)
