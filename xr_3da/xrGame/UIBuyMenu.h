#ifndef __XR_UIBUYMENU_H__
#define __XR_UIBUYMENU_H__
#pragma once

#include "UICustomMenu.h"
// refs
class CCustomMenuItem;
class CUIGameCustom;

class CUIBuyMenu{
	CCustomMenuItem*	menu_root;	
	CCustomMenuItem*	menu_active;
	
	int					menu_offs;
	int					menu_offs_col[2];
public:
						CUIBuyMenu			();
	virtual				~CUIBuyMenu			();

	void				Load				(LPCSTR ini, CUIGameCustom* parent, OnExecuteEvent exec);

	void				Render				();
	void				OnFrame				();

	bool				OnKeyboardPress		(int dik);
	bool				OnKeyboardRelease	(int dik);
	bool				OnMouseMove			(int dx, int dy);

	void				OnActivate			(){menu_active=menu_root;}
	void				OnDeactivate		(){;}
};

#endif // __XR_UIBUYMENU_H__
