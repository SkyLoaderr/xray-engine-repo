// LevelItem.h: interface for the CLevelItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEVELITEM_H__29C593F5_8B06_4F80_8F89_371B376C64A9__INCLUDED_)
#define AFX_LEVELITEM_H__29C593F5_8B06_4F80_8F89_371B376C64A9__INCLUDED_
#pragma once

#include "..\\..\\customitem.h"

class CLevelItem : public CCustomItem  
{
public:
						CLevelItem		(void *data);
	virtual				~CLevelItem		();
};

#endif // !defined(AFX_LEVELITEM_H__29C593F5_8B06_4F80_8F89_371B376C64A9__INCLUDED_)
