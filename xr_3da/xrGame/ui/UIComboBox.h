// File:        UIComboBox.h
// Description: guess :)
// Created:     10.12.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
// 
// Copyright 2004 GSC Game World
//

#pragma once
#include "UIFrameLineWnd.h"
#include "UIFrameWindow.h"
#include "UIEditBox.h"
#include "UI3tButton.h"
#include "UIListWndEx.h"

#define cs const static

struct data {
	struct btn {
		cs int width = 60;
	};
};

class CUIComboBox : public CUIWindow{
public:
	CUIComboBox();
	~CUIComboBox();

	// methods
	virtual CUIListWndEx* GetListWnd();
	virtual void          Init(int x, int y, int width);	
	virtual void          AddItem(LPCSTR str, bool bSelected = false);
	virtual void          SendMessage(CUIWindow *pWnd, s16 msg, void* pData = 0);
protected:
	virtual void          OnBtnClicked();
	virtual void          OnListItemSelect();
	virtual void          Draw();
	virtual void          Update();

protected:
	int             m_iListHeight;

	CUIFrameLineWnd m_frameLine;
	CUIEditBox      m_editBox;
	CUI3tButton     m_btn;
	CUIFrameWindow  m_frameWnd;
public:
	CUIListWndEx    m_list;

protected:	
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIComboBox)
#undef script_type_list
#define script_type_list save_type_list(CUIComboBox)