// Blender_default_aref.h: interface for the CBlender_default_aref class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_DEFAULT_AREF_H__E17F011F_C371_4464_B75A_01D68F55FC4E__INCLUDED_)
#define AFX_BLENDER_DEFAULT_AREF_H__E17F011F_C371_4464_B75A_01D68F55FC4E__INCLUDED_
#pragma once

#include "Blender.h"

class CBlender_default_aref : public CBlender  
{
public:
	xrP_Integer	oAREF;
public:
	virtual		LPCSTR		getComment()	{ return "base blender with tranparency";	}

	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS, WORD version);

	virtual		void		Compile			(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param=0, BOOL bEditor=FALSE);

	CBlender_default_aref();
	virtual ~CBlender_default_aref();
};

#endif // !defined(AFX_BLENDER_DEFAULT_AREF_H__E17F011F_C371_4464_B75A_01D68F55FC4E__INCLUDED_)
