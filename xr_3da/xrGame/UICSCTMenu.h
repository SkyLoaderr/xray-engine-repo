#ifndef __XR_CSCAHNGETEAMMENU_H__
#define __XR_CSCAHNGETEAMMENU_H__
#pragma once

#include "UIBuyMenu.h"
#include "UICustomMenu.h"

// refs
class CCustomMenuItem;
class CUIGameCustom;

class CCSChangeTeamMenu{
	CCustomMenuItem*	menu_root;	
	CCustomMenuItem*	menu_active;
	
	int					menu_offs;
public:
						CCSChangeTeamMenu	();
	virtual				~CCSChangeTeamMenu	();

	void				Load				(LPCSTR ini, CUIGameCustom* parent, OnExecuteEvent exec);

	void				Render				();
	void				OnFrame				();

	bool				OnKeyboardPress		(int dik);
	bool				OnKeyboardRelease	(int dik);
	bool				OnMouseMove			(int dx, int dy);

	void				Activate			(){menu_active=menu_root;}
	void				Deactivate			(){;}
};

#endif // __XR_CSCAHNGETEAMMENU_H__
