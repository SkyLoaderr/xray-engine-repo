// Blender.h: interface for the CBlender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
#define AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_
#pragma once

#include "Blender_Recorder.h"

class ENGINE_API CBlender  
{
	DWORD		BC			(BOOL v)		{ return v?0xff:0; }
public:
	virtual		LPCSTR		getName()		= 0;
	virtual		LPCSTR		getComment()	= 0;

	virtual		void		Compile			(CBlender_Recorder& RS)		= 0;

	CBlender();
	virtual ~CBlender();
};

#endif // !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
