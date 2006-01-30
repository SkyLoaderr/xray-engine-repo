// File:		UIMessagesWindow.h
// Description:	Window with MP chat and Game Log ( with PDA messages in single and Kill Messages in MP)
// Created:		22.04.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#include "StdAfx.h"

#include "UIMessagesWindow.h"
#include "../level.h"
#include "UIGameLog.h"
#include "xrXmlParser.h"
#include "UIXmlInit.h"
#include "UIChatLog.h"
#include "UIChatWnd.h"
#include "UIPdaMsgListItem.h"
#include "UIColorAnimatorWrapper.h"
#include "../InfoPortion.h"
#include "../string_table.h"
#include "../game_cl_artefacthunt.h"
#include "UIProgressShape.h"

CUIMessagesWindow::CUIMessagesWindow(){
	m_pChatLog = NULL;
	m_pChatWnd = NULL;
	m_pGameLog = NULL;
	m_pReinforcementInidcator = NULL;
	Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);
}

CUIMessagesWindow::~CUIMessagesWindow(){
	
}

void CUIMessagesWindow::AddLogMessage(KillMessageStruct& msg){
	m_pGameLog->AddLogMessage(msg);
}

void CUIMessagesWindow::AddLogMessage(const shared_str& msg){
	m_pGameLog->AddLogMessage(msg);
}

void CUIMessagesWindow::Init(float x, float y, float width, float height){

	CUIXml xml;

	xml.Init(CONFIG_PATH, UI_PATH, "messages_window.xml");

	m_pGameLog = xr_new<CUIGameLog>();m_pGameLog->SetAutoDelete(true);
	m_pGameLog->Show(true);
	AttachChild(m_pGameLog);
	if ( IsGameTypeSingle() )
	{
		CUIXmlInit::InitListWnd(xml, "sp_log_list", 0, m_pGameLog);
		m_ListPos2 = CUIXmlInit::GetFRect(xml, "sp_log_list2", 0);
	}
	else
	{
		m_pChatLog = xr_new<CUIChatLog>(); m_pChatLog->SetAutoDelete(true);
		m_pChatLog->Show(true);
		AttachChild(m_pChatLog);
		m_pChatWnd = xr_new<CUIChatWnd>(m_pChatLog); m_pChatWnd->SetAutoDelete(true);
		AttachChild(m_pChatWnd);

		CUIXmlInit::InitListWnd(xml, "mp_log_list", 0, m_pGameLog);
		CUIXmlInit::InitListWnd(xml, "chat_log_list", 0, m_pChatLog);
		
		m_pChatWnd->Init(xml);
	}	

	if (GAME_ARTEFACTHUNT == GameID())
	{			
        m_pReinforcementInidcator = xr_new<CUIProgressShape>();
		m_pReinforcementInidcator->SetAutoDelete(true);
		m_pReinforcementInidcator->SetVisible(false);
		AttachChild(m_pReinforcementInidcator);

		CUIXmlInit::InitProgressShape(xml, "reinforcement", 0, m_pReinforcementInidcator);			

		game_cl_ArtefactHunt* game = smart_cast<game_cl_ArtefactHunt*>(&Game());
		game->m_reinforcement_progress = m_pReinforcementInidcator;
	}

}
/*
bool CUIMessagesWindow::SetDelayForPdaMessage(int iValue, int iDelay){
	int index = m_pGameLog->FindItemWithValue(iValue);

	if (index >= 0)
	{
        CUIPdaMsgListItem* item = smart_cast<CUIPdaMsgListItem*>(m_pGameLog->GetItem(index));
        item->SetDelay(iDelay);
		return true;
	}

	return false;
}
*/
/*
void CUIMessagesWindow::AddPdaMessage(CInventoryOwner* pSender, EPdaMsg msg, INFO_ID info_id){
		R_ASSERT(pSender);

	CUIPdaMsgListItem* pItem = NULL;
	pItem = xr_new<CUIPdaMsgListItem>();
	m_pGameLog->AddItem<CUIListItem>(pItem, 0); 
	pItem->InitCharacter(smart_cast<CInventoryOwner*>(pSender));
	CUIColorAnimatorWrapper *p = xr_new<CUIColorAnimatorWrapper>("ui_main_msgs");
	R_ASSERT(p);
	p->Cyclic(false);
	pItem->SetData(p);
	if(msg == ePdaMsgInfo)
	{
		CInfoPortion info_portion;
		info_portion.Load(info_id);
		pItem->UIMsgText.SetText(*CStringTable().translate(info_portion.GetText()));
	}
	else
	{
	//	pItem->UIMsgText.SetText(CPda::m_PdaMsgStr[msg]);
	}
}
*/
void CUIMessagesWindow::AddPdaMessage(LPCSTR message, int iDelay){
	AddMessageToList(message, m_pGameLog, iDelay);
}

CUIPdaMsgListItem* CUIMessagesWindow::AddMessageToList(LPCSTR message, CUIListWnd* pListWnd, int iDelay){
	CUIPdaMsgListItem* pItem			= xr_new<CUIPdaMsgListItem>(iDelay);
	pListWnd->AddItem<CUIListItem>		(pItem, 0); 
	pListWnd->ScrollToBegin				();
	// create animation
	CUIColorAnimatorWrapper *p			= xr_new<CUIColorAnimatorWrapper>("ui_main_msgs_short");
	// no loop animation
	p->Cyclic							(false);
	// set animation wrapper as Item Data
	pItem->SetData						(p);
	// set id to item. With this we can find item in list and for some manipulation
//.	pItem->SetValue						(iId);
	pItem->UIMsgText.SetText			(message);	

	return pItem;
}

void CUIMessagesWindow::AddIconedPdaMessage(LPCSTR textureName, Frect originalRect, LPCSTR message, int iDelay){
	
	CUIPdaMsgListItem *pItem = AddMessageToList(message, m_pGameLog, iDelay);
	if (pItem)
	{
		pItem->UIIcon.InitTexture			(textureName);
		pItem->UIIcon.SetOriginalRect		(originalRect.left, originalRect.top, originalRect.right, originalRect.bottom);
//		pItem->UIMsgText.SetWndPos			(originalRect.x2, pItem->UIMsgText.GetWndPos().y);
		pItem->UIMsgText.SetVTextAlignment	(valTop);
	}
}

void CUIMessagesWindow::AddChatMessage(shared_str msg, shared_str author){
	 m_pChatLog->AddLogMessage(msg, author);
}

void CUIMessagesWindow::SetChatOwner(game_cl_GameState* owner){
	if (m_pChatWnd)
		m_pChatWnd->SetOwner(owner);
	else
		R_ASSERT2(false, "Can't set owner to NULL object");
}

void CUIMessagesWindow::DrawPdaMessages(){
	Frect tmp = m_pGameLog->GetWndRect		();
	m_pGameLog->SetWndPos					(m_ListPos2.x1, m_ListPos2.y1);m_pGameLog->SetWidth(m_ListPos2.width());
	m_pGameLog->SetWidth					(m_ListPos2.height());
	m_pGameLog->Draw						();

	m_pGameLog->SetWndPos					(tmp.x1, tmp.y1);
	m_pGameLog->SetWidth					(tmp.width());
	m_pGameLog->SetWidth					(tmp.height());
}
void CUIMessagesWindow::Update()
{
	CUIWindow::Update();
}