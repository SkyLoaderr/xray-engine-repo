// File:		UITabButtonMP.h
// Description:	Tab Button designed special for TabControl in CUIBuyWeaponWnd
// Created:		17.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#include "UITabButton.h"

enum Orientation{
	O_HORIZONTAL,
	O_VERTICAL
};

class CUITabButtonMP : public CUITabButton {
public:
	CUITabButtonMP();
	virtual ~CUITabButtonMP();

	virtual void Init(int x, int y, int width, int height);
	virtual void Update();
	virtual void UpdateTextAlign();	
	virtual void DrawHighlightedText() {}
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData /* = 0 */);

	virtual bool IsEnabled(){ return true; }
			void SetNumber(LPCSTR num);
			void SetOrientation(int orientation);

private:

    CUIStatic	m_staticNumber;
    int			m_orientation;
};