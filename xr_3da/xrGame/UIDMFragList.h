#ifndef __XR_UIDMFRAGLIST_H__
#define __XR_UIDMFRAGLIST_H__
#pragma once

#include "uilistbox.h"

class CUIDMFragList:public CUIListBox
{
	typedef CUIListBox inherited;
public:
					CUIDMFragList	();
	void			Init			();
	virtual void	OnFrame			();
};

#endif //__XR_UIFRAGLIST_H__
