#ifndef __XR_UITDMFRAGLIST_H__
#define __XR_UITDMFRAGLIST_H__
#pragma once

#include "uilistbox.h"

class CUITDMFragList:public CUIListBox
{
	typedef CUIListBox inherited;
public:
	CUITDMFragList	();
	void			Init			();
	virtual void	OnFrame			();
};

#endif //__XR_UITDMFRAGLIST_H__
