// Blender.h: interface for the CBlender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
#define AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_
#pragma once

class CBlender  
{
public:
	virtual		LPCSTR		getName()		= 0;
	virtual		LPCSTR		getComment()	= 0;

	virtual		void		Compile()		= 0;

	CBlender();
	virtual ~CBlender();
};

#endif // !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
