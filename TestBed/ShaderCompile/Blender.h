// Blender.h: interface for the CBlender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
#define AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_
#pragma once

#include "Blender_Recorder.h"
#include "Blender_Params.h"

class ENGINE_API CBlender  
{
	DWORD		BC			(BOOL v)		{ return v?0xff:0; }

	void		BP_write	(CFS_Base& FS,  DWORD ID, LPCSTR name, LPCVOID data, DWORD size );
	DWORD		BP_read     (CStream&  FS);
public:
	virtual		LPCSTR		getName()		= 0;
	virtual		LPCSTR		getComment()	= 0;

	virtual		void		Save			(CFS_Base&  FS)				= 0;
	virtual		void		Load			(CStream&	FS)				= 0;

	virtual		void		Compile			(CBlender_Recorder& RS)		= 0;

	CBlender();
	virtual ~CBlender();
};

#endif // !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
