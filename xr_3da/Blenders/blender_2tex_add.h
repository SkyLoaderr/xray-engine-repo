// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_)
#define AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_
#pragma once

#include "Blender.h"

class ENGINE_API CBlender_2tex_add : public CBlender  
{
public:
	string64	oT1_Constant;	// constant for 1st texture
	xrP_TOKEN	oBlend;
	string64	oT2_Name;		// name of secondary texture
	string64	oT2_xform;		// xform for secondary texture
public:
	virtual		LPCSTR		getComment()	{ return "[level] tex0*c + tex1"; }

	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS);

	virtual		void		Compile			(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param=0, BOOL bEditor=FALSE);


	CBlender_complex2();
	virtual ~CBlender_complex2();
};

#endif // !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_)
