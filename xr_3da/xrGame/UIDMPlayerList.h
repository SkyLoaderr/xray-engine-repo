#ifndef __XR_UIDMPLAYERLIST_H__
#define __XR_UIDMPLAYERLIST_H__
#pragma once

#include "uiListBox.h"

class CUIDMPlayerList:public CUIListBox
{
	typedef CUIListBox inherited;
public:
					CUIDMPlayerList	();
	void			Init			();
	virtual void	OnFrame			();
};

#endif //__XR_UIDMPLAYERLIST_H__
