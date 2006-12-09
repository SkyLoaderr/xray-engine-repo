// File:		UITabButtonMP.h
// Description:	Tab Button designed special for TabControl in CUIBuyWeaponWnd
// Created:		17.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
// Copyright 2005 GSC Game World

#pragma once
#include "UITabButton.h"

class CUITabButtonMP : public CUITabButton 
{
	typedef CUITabButton inherited;
public:
					CUITabButtonMP			();

	virtual void 	Draw					();
	virtual void 	Update					();
	virtual void 	UpdateTextAlign			();	
	virtual void 	DrawHighlightedText		()	{}
	virtual void 	SendMessage				(CUIWindow* pWnd, s16 msg, void* pData);

	virtual bool 	IsEnabled				()					{ return true; }
			void 	SetOrientation			(bool bVert)		{m_orientationVertical = bVert;};
	u32				m_temp_index;
private:
    bool			m_orientationVertical;

};