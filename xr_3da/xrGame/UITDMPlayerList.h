#ifndef __XR_UITDMPLAYERLIST_H__
#define __XR_UITDMPLAYERLIST_H__
#pragma once

//#include "uiListBox.h"
#include "UIDMPlayerList.h"

class CUITDMPlayerList:public CUIDMPlayerList
{
	typedef CUIDMPlayerList inherited;
private:
	u8				m_CurTeam;
public:
	void			Init			(u8 Team);
	virtual void	UpdateItemsList	();
};

#endif //__XR_UITDMPLAYERLIST_H__