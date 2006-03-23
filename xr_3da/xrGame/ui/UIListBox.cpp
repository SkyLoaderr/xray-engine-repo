#include "stdafx.h"
#include "UIListBox.h"
#include "UIListBoxItem.h"
#include "UIScrollBar.h"

CUIListBox::CUIListBox(){
	m_flags.set(eItemsSelectabe, TRUE);
	m_cur_wnd_it = false;

	m_def_item_height = 20;
	m_last_selection = -1;
	m_text_color = 0xff000000;
	m_text_color_s = 0xff000000;
	m_text_al = CGameFont::alLeft;
	m_cur_wnd_it = NULL;
	m_last_wnd = NULL;
}

bool CUIListBox::OnMouse(float x, float y, EUIMessages mouse_action){
	if(CUIWindow::OnMouse(x,y,mouse_action)) return true;

	switch (mouse_action){
		case WINDOW_MOUSE_WHEEL_UP:
			m_VScrollBar->TryScrollDec();
			return true;
		break;
		case WINDOW_MOUSE_WHEEL_DOWN:
			m_VScrollBar->TryScrollInc();
			return true;
		break;
	};
	return false;
}

void CUIListBox::DeselectAll(){
	for(WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); m_pad->GetChildWndList().end()!=it; ++it)
	{
		(smart_cast<CUISelectable*>(*it))->SetSelected(false);
	}
}

CUIListBoxItem* CUIListBox::AddItem(LPCSTR text){
	CUIListBoxItem* pItem = xr_new<CUIListBoxItem>();
	pItem->Init(0,0,this->GetDesiredChildWidth() - 5, m_def_item_height);
	pItem->InitDefault();
	pItem->SetSelected(false);
	pItem->SetText(text);
	pItem->SetTextColor(m_text_color, m_text_color_s);
	pItem->SetMessageTarget(this);
	AddWindow(pItem, true);
	return pItem;
}

void CUIListBox::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	if (LIST_ITEM_SELECT == msg)
		GetMessageTarget()->SendMessage(this, LIST_ITEM_SELECT, pData);

	CUIWindow::SendMessage(pWnd, msg, pData);
}

LPCSTR CUIListBox::GetSelectedText(){
	for(WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); m_pad->GetChildWndList().end()!=it; ++it)
	{
		if (smart_cast<CUISelectable*>(*it)->GetSelected())
		{
			m_cur_wnd_it = it;
			return smart_cast<IUITextControl*>(*it)->GetText();
		}
	}
	return NULL;
}

CUIListBoxItem* CUIListBox::GetSelectedItem(){
	for(WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); m_pad->GetChildWndList().end()!=it; ++it)
	{
		if (smart_cast<CUIListBoxItem*>(*it)->GetSelected())
		{
			m_cur_wnd_it = it;
			return smart_cast<CUIListBoxItem*>(*it);
		}
	}
	return NULL;
}

LPCSTR CUIListBox::GetNextSelectedText(){
	m_cur_wnd_it++;
	for(WINDOW_LIST_it it = m_cur_wnd_it; m_pad->GetChildWndList().end()!=it; ++it)
	{
		if (smart_cast<CUISelectable*>(*it)->GetSelected())
		{
			m_cur_wnd_it = it;
			return smart_cast<IUITextControl*>(*it)->GetText();
		}
	}
	m_cur_wnd_it = m_pad->GetChildWndList().end();
	return NULL;
}

LPCSTR CUIListBox::GetFirstText(){
	m_cur_wnd_it = m_pad->GetChildWndList().begin();
	return smart_cast<IUITextControl*>(*m_cur_wnd_it)->GetText();
}

void CUIListBox::MoveSelectedUp(){
	R_ASSERT(!m_flags.test(CUIScrollView::eMultiSelect));
	WINDOW_LIST_it it = m_pad->GetChildWndList().begin();
	WINDOW_LIST_it it_prev = NULL;

	if (smart_cast<CUISelectable*>(*it)->GetSelected())
		return;

	for(; m_pad->GetChildWndList().end()!=it; it_prev = it,it++)
	{
		
		if (smart_cast<CUISelectable*>(*it)->GetSelected())
		{
			WINDOW_LIST_it temp = it_prev;
			*it_prev = *it;
            *it = *temp;			
		}
	}
}

void CUIListBox::MoveSelectedDown(){
	R_ASSERT(!m_flags.test(CUIScrollView::eMultiSelect));
	WINDOW_LIST_it it = m_pad->GetChildWndList().begin();
	WINDOW_LIST_it it_prev = NULL;

	for(; m_pad->GetChildWndList().end()!=it; it_prev = it,it++)
	{		
		if (smart_cast<CUISelectable*>(*it_prev)->GetSelected())
		{
			WINDOW_LIST_it temp = it_prev;
			*it_prev = *it;
            *it = *temp;
		}
	}
}

#include "../../xr_input.h"

void CUIListBox::SetSelected(CUIWindow* pWnd){
	if (NULL == pWnd)
		return;
	if(!m_flags.test(eItemsSelectabe)) return;

	bool shift = !!pInput->iGetAsyncKeyState(DIK_LSHIFT);
	bool ctrl = !!pInput->iGetAsyncKeyState(DIK_LCONTROL);

	if (shift && m_flags.test(CUIScrollView::eMultiSelect))
	{	
		CUIWindow* selected = m_last_wnd;
//		CUIWindow* selected_last = GetSelectedLast();

		if (NULL == selected)
		{
			CUIScrollView::SetSelected(pWnd);
			m_last_wnd = GetSelected();
			return;
		}

		WINDOW_LIST_it first;	first = m_pad->GetChildWndList().end();
		WINDOW_LIST_it second;	second = m_pad->GetChildWndList().end();
		for(WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); m_pad->GetChildWndList().end()!=it; ++it){
			if (selected == *it){
				if (first==m_pad->GetChildWndList().end())
					first = it;
				else{
					second = it;
					break;
				}
			}
			if (pWnd == *it){
				if (first==m_pad->GetChildWndList().end())
					first = it;
				else{
					second = it;
					break;
				}
			}
		}

		bool flag = false;

		for(WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); m_pad->GetChildWndList().end()!=it; ++it)
		{			
            if (it == first && flag == false)
				flag = true;

			smart_cast<CUISelectable*>(*it)->SetSelected(flag);

			if (it == second)
				flag = false;
		}
        return;
	}

	if (ctrl && m_flags.test(CUIScrollView::eMultiSelect))
	{		
		for(WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); m_pad->GetChildWndList().end()!=it; ++it)
		{
			if (*it==pWnd)
			{
				CUIListBoxItem* s = smart_cast<CUIListBoxItem*>(*it);
				s->SetSelected(!s->GetSelected());
				m_last_wnd = s;
				GetMessageTarget()->SendMessage(this, LIST_ITEM_CLICKED);
				break;
			}
		}

		return;
	}

	CUIScrollView::SetSelected(pWnd);
	GetMessageTarget()->SendMessage(this, LIST_ITEM_CLICKED);
	m_last_wnd = GetSelected();
}

void CUIListBox::SetSelected(u32 uid){
	SetSelected(GetItemByID(uid));
}

CUIListBoxItem* CUIListBox::GetItemByID(u32 uid){
	for(WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); m_pad->GetChildWndList().end()!=it; ++it)
	{
		CUIListBoxItem* item = smart_cast<CUIListBoxItem*>(*it);
		if (item)
		{
			if (item->GetID() == uid)
				return item;
		}
		
	}
	return NULL;
}

LPCSTR CUIListBox::GetNextText(){
	m_cur_wnd_it++;
	if (m_pad->GetChildWndList().end() == m_cur_wnd_it)
		return NULL;
	else
		return smart_cast<IUITextControl*>(*m_cur_wnd_it)->GetText();
}

void CUIListBox::SetTextColor(u32 color){
	m_text_color = color;
}

void CUIListBox::SetTextColorS(u32 color){
	m_text_color_s = color;
}

u32 CUIListBox::GetTextColor(){
	return m_text_color;
}

void CUIListBox::SetFont(CGameFont* pFont){
	CUIWindow::SetFont(pFont);
}

CGameFont* CUIListBox::GetFont(){
	return CUIWindow::GetFont();
}

void CUIListBox::SetTextAlignment(ETextAlignment alignment){
	m_text_al = alignment;
}

ETextAlignment CUIListBox::GetTextAlignment(){
	return m_text_al;
}