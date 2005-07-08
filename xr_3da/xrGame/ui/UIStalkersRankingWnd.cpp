//=============================================================================
//  Filename:   UIStalkersRankingWnd.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ������� �������� ��� - ������� ��������� ���� ����
//=============================================================================

#include "StdAfx.h"
#include "UIStalkersRankingWnd.h"
#include "UIXmlInit.h"
#include "UIPdaAux.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIPdaListItem.h"
#include "UIAnimatedStatic.h"
#include "UIListWnd.h"
#include "UICharacterInfo.h"
#include "../InventoryOwner.h"
#include "../Level.h"
#include "../pda.h"
#include "../MainUI.h"

//////////////////////////////////////////////////////////////////////////

const char * const		STALKERS_RANKING_XML			= "stalkers_ranking.xml";
const char * const		STALKERS_RANKING_CHARACTER_XML	= "stalkers_ranking_character.xml";

//////////////////////////////////////////////////////////////////////////

void CUIStalkersRankingWnd::Init()
{
	CUIXml		uiXml;
	R_ASSERT3(uiXml.Init(CONFIG_PATH, UI_PATH,STALKERS_RANKING_XML), "xml file not found", STALKERS_RANKING_XML);

	CUIXmlInit	xml_init;

	inherited::Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	// ������������ ����������
	UICharIconFrame = xr_new<CUIFrameWindow>(); UICharIconFrame->SetAutoDelete(true);
	AttachChild(UICharIconFrame);
	xml_init.InitFrameWindow(uiXml, "chicon_frame_window", 0, UICharIconFrame);

	UICharIconHeader = xr_new<CUIFrameLineWnd>(); UICharIconHeader->SetAutoDelete(true);
	UICharIconFrame->AttachChild(UICharIconHeader);
	xml_init.InitFrameLine(uiXml, "chicon_frame_line", 0, UICharIconHeader);

	UIAnimatedIcon = xr_new<CUIAnimatedStatic>(); UIAnimatedIcon->SetAutoDelete(true);
	UICharIconHeader->AttachChild(UIAnimatedIcon);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, UIAnimatedIcon);

	UIInfoFrame = xr_new<CUIFrameWindow>(); UIInfoFrame->SetAutoDelete(true);
	AttachChild(UIInfoFrame);
	xml_init.InitFrameWindow(uiXml, "info_frame_window", 0, UIInfoFrame);
	
	UIInfoHeader = xr_new<CUIFrameLineWnd>(); UIInfoHeader->SetAutoDelete(true);
	UIInfoFrame->AttachChild(UIInfoHeader);
	xml_init.InitFrameLine(uiXml, "info_frame_line", 0, UIInfoHeader);

	UIStalkersList = xr_new<CUIListWnd>(); UIStalkersList->SetAutoDelete(true);
	UIInfoFrame->AttachChild(UIStalkersList);
	xml_init.InitListWnd(uiXml, "list", 0, UIStalkersList);
	UIStalkersList->SetMessageTarget(this);
	UIStalkersList->ActivateList(true);
	UIStalkersList->EnableScrollBar(true); 

	UICharacterWindow = xr_new<CUIWindow>(); UICharacterWindow->SetAutoDelete(true);
	UICharIconFrame->AttachChild(UICharacterWindow);
	UICharacterWindow->SetMessageTarget(this);
	xml_init.InitWindow(uiXml, "character_info", 0, UICharacterWindow);

	UICharacterInfo = xr_new<CUICharacterInfo>(); UICharacterInfo->SetAutoDelete(true);
	UICharacterWindow->AttachChild(UICharacterInfo);
	UICharacterInfo->Init(0,0,UICharacterWindow->GetWidth(), 
									UICharacterWindow->GetHeight(), 
									STALKERS_RANKING_CHARACTER_XML);
	UICharacterInfo->m_bInfoAutoAdjust = false;


	UIMask = xr_new<CUIFrameWindow>(); UIMask->SetAutoDelete(true);
	xml_init.InitFrameWindow(uiXml, "mask_frame_window", 0, UIMask);
	UICharacterInfo->UIIcon().SetMask(UIMask);

	//�������� ��������������� ����������
	xml_init.InitAutoStatic(uiXml, "right_auto_static", UICharIconFrame);
	xml_init.InitAutoStatic(uiXml, "left_auto_static",  UIInfoFrame);

	UIArticleHeader = xr_new<CUIStatic>(); UIArticleHeader->SetAutoDelete(true);
	UIInfoFrame->AttachChild(UIArticleHeader);
	xml_init.InitStatic(uiXml, "article_header_static", 0, UIArticleHeader);
}

//////////////////////////////////////////////////////////////////////////

void CUIStalkersRankingWnd::AddStalkerInfo()
{
	UIStalkersList->RemoveAll();
	CInventoryOwner *m_pInvOwner = smart_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(!m_pInvOwner) return;

	CPda * m_pPda = m_pInvOwner->GetPDA();
	if(!m_pPda)return;
//	PDA_LIST_it it;	

	// �������� ������ ���������

	xr_vector<CPda*> pda_list;
	m_pPda->ActiveContacts(pda_list);
	xr_vector<CPda*>::iterator it = pda_list.begin();

	for(	; it!=pda_list.end(); ++it){
		CPda* pda = (*it);
		CUIPdaListItem* pItem = NULL;
		pItem = xr_new<CUIPdaListItem>();
		UIStalkersList->AddItem<CUIListItem>(pItem); 
		pItem->InitCharacter(pda->GetOriginalOwner());
		// To ���: ����� ������, ��� ���� �������� - ��� ���� �������� ������ ��������� ��
		// CCharacterInfo. ��������� ����� ������������ :)
		pItem->SetData(pda->GetOriginalOwner());
	}
/*
	for(it = m_pPda->m_PDAList.begin(); m_pPda->m_PDAList.end() != it; ++it)
	{	
		CUIPdaListItem* pItem = NULL;
		pItem = xr_new<CUIPdaListItem>();
		UIStalkersList.AddItem<CUIListItem>(pItem); 
		pItem->InitCharacter((*it)->GetOriginalOwner());
		// To ���: ����� ������, ��� ���� �������� - ��� ���� �������� ������ ��������� ��
		// CCharacterInfo. ��������� ����� ������������ :)
		pItem->SetData((*it)->GetOriginalOwner());
	}
*/
}

//////////////////////////////////////////////////////////////////////////

void CUIStalkersRankingWnd::Show(bool status)
{
	inherited::Show(status);
	if (status)
	{
		AddStalkerInfo();
		UICharacterWindow->Show(false);
		UICharIconHeader->UITitleText.SetText("");
	}
}

//////////////////////////////////////////////////////////////////////////
#include "../character_info.h"

void CUIStalkersRankingWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (UIStalkersList == pWnd && LIST_ITEM_CLICKED == msg)
	{
		UICharacterWindow->Show(true);
		CCharacterInfo &charInfo = reinterpret_cast<CInventoryOwner*>(reinterpret_cast<CUIListItem*>(pData)->GetData())->CharacterInfo();
		UICharacterInfo->InitCharacter(&charInfo);
		UICharIconHeader->UITitleText.SetText(charInfo.Name());
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIStalkersRankingWnd::RemoveStalkers()
{
	UIStalkersList->RemoveAll();
	UICharacterWindow->Show(false);
	UICharIconHeader->UITitleText.SetText("");
}