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
#include "UIMainIngameWnd.h"
#include "UIPdaMsgListItem.h"
#include "UIPdaKillMessage.h"

extern const char * const	CHAT_MP_WND_XML;
extern const int			fadeDelay;
const char * const	CHAT_LOG_ITEMS_ANIMATION	= "ui_mp_chat";

CUIGameLog::CUIGameLog(){

}

CUIGameLog::~CUIGameLog(){

}


void CUIGameLog::AddLogMessage(const shared_str &msg)
{
	CUIColorAnimatorWrapper	*animation	= xr_new<CUIColorAnimatorWrapper>(CHAT_LOG_ITEMS_ANIMATION);
	CUIPdaMsgListItem* pItem = NULL;
	
	pItem = xr_new<CUIPdaMsgListItem>(-1);	

	UILogList.AddItem(pItem, 0); 
	CUIListItem	*item = UILogList.GetItem(UILogList.GetSize() - 1);
	VERIFY(item);

	pItem->UIMsgText.SetText(msg.c_str());	
	pItem->SetFont(GetFont());
	int h = pItem->GetHeight();
	int h2 = (int)pItem->UIMsgText.GetFont()->CurrentHeight();
	int y = (h - h2)/2;	
	pItem->UIMsgText.SetTextPos(0, y);
	pItem->UIMsgText.SetWndPos(0, 0);	
	pItem->SetData(animation);

	item->Show(true);
	animation->SetColorToModify(&item->GetTextColorRef());
	animation->Cyclic(false);
	
}

void CUIGameLog::AddLogMessage(KillMessageStruct& msg){
	CUIColorAnimatorWrapper	*animation	= xr_new<CUIColorAnimatorWrapper>(CHAT_LOG_ITEMS_ANIMATION);
	CUIPdaKillMessage* pItem = NULL;
	
	pItem = xr_new<CUIPdaKillMessage>(-1);

	pItem->SetFont(GetFont());
	UILogList.AddItem(pItem, 0); 
	
	pItem->Init(msg);	
	pItem->SetData(animation);

	pItem->Show(true);

	animation->SetColorToModify(&pItem->GetTextColorRef());
	animation->Cyclic(false);	
}

//////////////////////////////////////////////////////////////////////////

void CUIGameLog::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, CHAT_MP_WND_XML);
	R_ASSERT2(xml_result, "xml file not found");

	//	uiXml.Init("$game_data$", CHAT_MP_WND_XML);

	CUIXmlInit xml_init;

	// Chat log
	AttachChild(&UILogList);
	xml_init.InitListWnd(uiXml, "game_log_list", 0, &UILogList);
	UILogList.SetVertFlip(true);
	UILogList.EnableScrollBar(false);
}

void CUIGameLog::Draw(){
	CUIDialogWnd::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIGameLog::Update()
{
	CUIListItem				*pItem		= NULL;
	CUIColorAnimatorWrapper *anm		= NULL;
	toDelIndexes.clear();

	int invisible_items = 0;

	for (int i = 0; i < UILogList.GetSize(); ++i)
	{
		pItem = UILogList.GetItem(i);
		CUIPdaMsgListItem	*pPItem = smart_cast<CUIPdaMsgListItem*>(pItem);

		VERIFY(pItem);
		anm		= reinterpret_cast<CUIColorAnimatorWrapper*>(pItem->GetData());
		VERIFY(anm);
		anm->Update();

		if (pPItem)
		{
			pPItem->SetTextColor(subst_alpha(pPItem->UIMsgText.GetTextColor(), color_get_A(anm->GetColor())));
		}
		else
		{
			pItem->SetTextColor(subst_alpha(pItem->GetTextColor(), color_get_A(anm->GetColor())));
		}

		// Remove at animation end
		if (anm->Done())
		{
			xr_delete(anm);
			toDelIndexes.insert(i);
		}

		if (!pItem->IsShown())
			invisible_items++;		
	}

	for (int i = 0; i < invisible_items; i++)
	{
		pItem = UILogList.GetItem(i);
		anm		= reinterpret_cast<CUIColorAnimatorWrapper*>(pItem->GetData());
		
		xr_delete(anm);
		toDelIndexes.insert(i);
	}
	

	// Delete elements
	for (ToDelIndexes_it it = toDelIndexes.begin(); it != toDelIndexes.end(); ++it)
	{
		UILogList.RemoveItem(*it);
	}
}