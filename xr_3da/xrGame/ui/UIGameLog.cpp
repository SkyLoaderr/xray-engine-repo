//=============================================================================
//  Filename:   UIGameLog.h
//	Created by Vitaly 'Mad Max' Maximov, mad-max@gsc-game.kiev.ua
//	Copyright 2005. GSC Game World
//	---------------------------------------------------------------------------
//  Multiplayer game log window
//=============================================================================
#include "stdafx.h"
#include "UIGameLog.h"
#include "UIXmlInit.h"
#include "UIColorAnimatorWrapper.h"
#include "UIPdaMsgListItem.h"
#include "UIPdaKillMessage.h"
#include "UILines.h"

const char * const	CHAT_LOG_ITEMS_ANIMATION	= "ui_main_msgs_short";

CUIGameLog::CUIGameLog(){
	toDelList.reserve(30);
	kill_msg_height = 20;
	txt_color = 0xff000000;
}

CUIGameLog::~CUIGameLog(){

}


CUIStatic* CUIGameLog::AddLogMessage(LPCSTR msg)
{
	CUIStatic* pItem = NULL;
	ADD_TEXT_TO_VIEW3(msg, pItem, this);
	pItem->SetFont(GetFont());
	pItem->SetTextColor(txt_color);
	pItem->SetAnimDelay(5000.0f);
	pItem->SetLightAnim(CHAT_LOG_ITEMS_ANIMATION, false, true, true, true);
	ForceUpdate();
	return pItem;
}

// warning: initialization of item is incomplete!
// initialization of item's height, text static and icon still necessary
CUIPdaMsgListItem* CUIGameLog::AddPdaMessage(LPCSTR msg, float delay){
	CUIPdaMsgListItem* pItem = xr_new<CUIPdaMsgListItem>();
	pItem->Init(0,0, GetDesiredChildWidth(), 10);	//fake height
	pItem->UIMsgText.SetTextST(msg);
	pItem->SetAnimDelay(delay);
    pItem->SetLightAnim(CHAT_LOG_ITEMS_ANIMATION, false, true, true, true);
	AddWindow(pItem, true);

	return pItem;
}

u32 CUIGameLog::GetTextColor(){
	return txt_color;
}

CUIPdaKillMessage* CUIGameLog::AddLogMessage(KillMessageStruct& msg){
	CUIPdaKillMessage* pItem = pItem = xr_new<CUIPdaKillMessage>();	
	pItem->SetFont(GetFont());
	pItem->SetWidth(GetDesiredChildWidth());
	pItem->SetHeight(kill_msg_height);
	pItem->Init(msg);
	pItem->SetTextColor(GetTextColor());
	pItem->SetAnimDelay(5000.0f);
	pItem->SetLightAnim(CHAT_LOG_ITEMS_ANIMATION, false, true, true, true);
	AddWindow(pItem, true);
	return pItem;
}

void CUIGameLog::AddChatMessage(LPCSTR msg, LPCSTR author){
	string256 fullLine;
	sprintf(fullLine, "%s %s", author, msg);
    
	CUIStatic* pItem = NULL;

	pItem = xr_new<CUIStatic>();
	pItem->SetText(fullLine);
    pItem->m_pLines->SetCutWordsMode(true);
	pItem->SetFont(GetFont());
	pItem->SetTextColor(txt_color);
	pItem->SetAnimDelay(5000.0f);
	pItem->SetLightAnim(CHAT_LOG_ITEMS_ANIMATION, false, true, true, true);	
	pItem->SetWidth(this->GetDesiredChildWidth());
	pItem->AdjustHeightToText();
	AddWindow(pItem, true);	
}

void CUIGameLog::SetTextAtrib(CGameFont* pFont, u32 color){
	SetFont(pFont);
	txt_color = color;
}

void CUIGameLog::Update()
{
	CUIScrollView::Update();
	toDelList.clear();	


	// REMOVE ITEMS WITH COMPLETED ANIMATION
	WINDOW_LIST_it end_it = m_pad->GetChildWndList().end();
	WINDOW_LIST_it begin_it = m_pad->GetChildWndList().begin();

	for(WINDOW_LIST_it it = begin_it; it!=end_it; ++it)
	{
		CUIStatic* pItem = smart_cast<CUIStatic*>(*it);
		VERIFY(pItem);
		pItem->Update();

		if (pItem->IsAnimStoped())
			toDelList.push_back(pItem);
	}

	// Delete elements
	{
		xr_vector<CUIWindow*>::iterator it;
		for (it = toDelList.begin(); it != toDelList.end(); it++)
			RemoveWindow(*it);
	}

	// REMOVE INVISIBLE AND PART VISIBLE ITEMS
	if(m_flags.test	(eNeedRecalc) )
		RecalcSize			();

	toDelList.clear();
	Frect visible_rect = GetAbsoluteRect();
	for(	WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); 
			m_pad->GetChildWndList().end()!=it; 
			++it)
	{
		Frect	r		= (*it)->GetAbsoluteRect();
		if(! (visible_rect.in(r.x1, r.y1) && visible_rect.in(r.x2, r.y1) && visible_rect.in(r.x1, r.y2) && visible_rect.in(r.x2, r.y2)))
		{
			toDelList.push_back(*it);			
		}
			
	}

	// Delete elements
	{
		xr_vector<CUIWindow*>::iterator it;
		for (it = toDelList.begin(); it != toDelList.end(); it++)
			RemoveWindow(*it);
	}

	if(m_flags.test	(eNeedRecalc) )
		RecalcSize			();
}