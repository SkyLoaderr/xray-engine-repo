#ifndef __XR_UITDMPLAYERLIST_H__
#define __XR_UITDMPLAYERLIST_H__
#pragma once

#include "uiListBox.h"

class CUITDMPlayerList:public CUIListBox
{
	typedef CUIListBox inherited;
private:
	u8				m_u8Team;
public:
	CUITDMPlayerList	();
	void			Init			(u8 Team, u32 X, u32 Y, u32 Width, u32 Height, EUIItemAlign Align);
	virtual void	OnFrame			();
};

#endif //__XR_UITDMPLAYERLIST_H__