#ifndef __XR_UITDMFRAGLIST_H__
#define __XR_UITDMFRAGLIST_H__
#pragma once

#include "uilistbox.h"

class CUITDMFragList:public CUIListBox
{
	typedef CUIListBox inherited;
private:
	u8				m_u8Team;
public:
	CUITDMFragList	();
	void			Init			(u8	Team);
	virtual void	OnFrame			();
};

#endif //__XR_UITDMFRAGLIST_H__
