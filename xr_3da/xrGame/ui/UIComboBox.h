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
//#include "UIListWnd.h"
#include "UIListBox.h"
#include "UIInteractiveBackground.h"
#include "UIOptionsItem.h"

class CUIComboBox : public CUIWindow, public CUIOptionsItem {
	friend class CUIXmlInit;
	typedef enum{
		LIST_EXPANDED, 
		LIST_FONDED    
	} E_COMBO_STATE;

public:
	CUIComboBox();
	virtual ~CUIComboBox();
	// CUIOptionsItem
	virtual void	SetCurrentValue();
	virtual void	SaveValue();
	virtual bool	IsChanged();

			LPCSTR	GetText();

	// methods
	virtual CUIListBox*	GetListWnd();
	virtual void		SetListLength(int length);
	virtual void		SetVertScroll(bool bVScroll = true);
	virtual void		AddItem(LPCSTR str, bool bSelected);
	virtual void		AddItem(LPCSTR str);
	virtual void		Init(float x, float y, float width);
			void		SetItem(int i);

	virtual void		Init(float x, float y, float width, float height);	
	virtual void		SendMessage(CUIWindow *pWnd, s16 msg, void* pData = 0);
	virtual void		Draw();
	virtual void		OnFocusLost();
	virtual void		OnFocusReceive();
protected:
	virtual void		SetState(UIState state);	
	virtual bool		OnMouse(float x, float y, EUIMessages mouse_action);
	virtual void		OnBtnClicked();
	virtual void		ShowList(bool bShow);
	virtual void		OnListItemSelect();
	virtual void		Update();

protected:
	bool            m_bInited;
	int             m_iListHeight;
	E_COMBO_STATE   m_eState;

	CUI_IB_FrameLineWnd	m_frameLine;
	CUILabel			m_text;
	CUI3tButton			m_btn;
	CUIFrameWindow		m_frameWnd;
public:
	CUIListBox	    m_list;

protected:	
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIComboBox)
#undef script_type_list
#define script_type_list save_type_list(CUIComboBox)