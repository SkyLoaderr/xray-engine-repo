// File:        UIComboBox.h
// Description: guess :)
// Created:     10.12.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
// 
// Copyright 2004 GSC Game World
//

#pragma once
#include "UIEditBox.h"
#include "UI3tButton.h"
#include "UIListWnd.h"
#include "UIInteractiveBackground.h"


class CUIComboBox : public CUIWindow{
	typedef enum{
		LIST_EXPANDED, 
		LIST_FONDED    
	} E_COMBO_STATE;

public:
	CUIComboBox();
	virtual ~CUIComboBox();

	// methods
	virtual CUIListWnd*	GetListWnd();
	virtual bool		SetListLength(int length);
	virtual void		SetVertScroll(bool bVScroll = true);
	virtual void		Init(int x, int y, int width);
	virtual void		Init(int x, int y, int width, int height);
	virtual void		AddItem(LPCSTR str, bool bSelected);
	virtual void		AddItem(LPCSTR str);
	virtual void		SendMessage(CUIWindow *pWnd, s16 msg, void* pData = 0);
	virtual void		Draw();
protected:
	virtual void		SetState(UIState state);	
	virtual void		OnMouse(int x, int y, EUIMessages mouse_action);
	virtual void		OnBtnClicked();
	virtual void		ShowList(bool bShow);
	virtual void		OnListItemSelect();
	virtual void		Update();

protected:
	bool            m_bInited;
	int             m_iListHeight;
	E_COMBO_STATE   m_eState;

	CUI_IB_FrameLineWnd	m_frameLine;
	CUIEditBox			m_editBox;
	CUI3tButton			m_btn;
	CUI_IB_FrameWindow	m_frameWnd;
public:
	CUIListWnd    m_list;

protected:	
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIComboBox)
#undef script_type_list
#define script_type_list save_type_list(CUIComboBox)