#ifndef __XR_UITDMPLAYERLIST_H__
#define __XR_UITDMPLAYERLIST_H__
#pragma once

#include "uiListBox.h"

class CUITDMPlayerList:public CUIListBox
{
	typedef CUIListBox inherited;
public:
	CUITDMPlayerList	();
	void			Init			();
	virtual void	OnFrame			();
};

#endif //__XR_UITDMPLAYERLIST_H__