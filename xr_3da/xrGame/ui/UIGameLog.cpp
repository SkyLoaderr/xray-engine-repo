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

//////////////////////////////////////////////////////////////////////////

extern const char * const	CHAT_MP_WND_XML;
extern const int			fadeDelay;
const char * const	CHAT_LOG_ITEMS_ANIMATION	= "ui_mp_chat";

//////////////////////////////////////////////////////////////////////////

void CUIGameLog::AddLogMessage(const shared_str &msg)
{
	CUIColorAnimatorWrapper	*animation	= xr_new<CUIColorAnimatorWrapper>(CHAT_LOG_ITEMS_ANIMATION);
//	UILogList.AddItem<CUIListItem>(msg.c_str(), 0, animation);
	CUIPdaMsgListItem* pItem = NULL;
	
	pItem = xr_new<CUIPdaMsgListItem>(-1);
	

	UILogList.AddItem(pItem, -1); 
	CUIListItem	*item = UILogList.GetItem(UILogList.GetSize() - 1);
	VERIFY(item);
	
	pItem->UIMsgText.SetText(msg.c_str());	
	pItem->UIMsgText.SetWndPos(0, 0);	
	pItem->SetData(animation);
//	item->SetData(animation);
//	item->SetText(msg.c_str());

	item->Show(true);
	animation->SetColorToModify(&item->GetTextColorRef());
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

}

//////////////////////////////////////////////////////////////////////////

void CUIGameLog::Update()
{
	CUIListItem				*pItem		= NULL;
	CUIColorAnimatorWrapper *anm		= NULL;
	toDelIndexes.clear();
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
			pPItem->UIMsgText.SetTextColor(subst_alpha(pPItem->UIMsgText.GetTextColor(), color_get_A(anm->GetColor())));
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
	}

	// Delete elements
	for (ToDelIndexes_it it = toDelIndexes.begin(); it != toDelIndexes.end(); ++it)
	{
		UILogList.RemoveItem(*it);
	}
}