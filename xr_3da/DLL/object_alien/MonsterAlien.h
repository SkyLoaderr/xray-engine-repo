// MonsterAlien.h: interface for the CMonsterAlien class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONSTERALIEN_H__9A937F4A_C114_4B87_A241_BFE4C93667FE__INCLUDED_)
#define AFX_MONSTERALIEN_H__9A937F4A_C114_4B87_A241_BFE4C93667FE__INCLUDED_
#pragma once

#include "..\..\xr_object.h"
#include "..\..\ai_frustum.h"
#include "..\..\ai_pathquantizer.h"

class CMonsterAlien : public CObject  
{
public:
	virtual void		Load			(CInifile* ini, const char* section);
	virtual void		OnMove			();

						CMonsterAlien	(void *p);
	virtual				~CMonsterAlien	();
};

#endif // !defined(AFX_MONSTERALIEN_H__9A937F4A_C114_4B87_A241_BFE4C93667FE__INCLUDED_)
