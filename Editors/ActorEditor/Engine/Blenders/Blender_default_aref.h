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
	xrP_BOOL	oBlend;
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: lmap*base.aref";	}

	virtual		void		Save			(IWriter&  FS);
	virtual		void		Load			(IReader&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_default_aref();
	virtual ~CBlender_default_aref();
};

#endif // !defined(AFX_BLENDER_DEFAULT_AREF_H__E17F011F_C371_4464_B75A_01D68F55FC4E__INCLUDED_)
