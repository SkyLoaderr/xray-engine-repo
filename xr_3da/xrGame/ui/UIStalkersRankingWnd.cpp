//=============================================================================
//  Filename:   UIStalkersRankingWnd.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Подокно дневника ПДА - рейтинг сталкеров всей игры
//=============================================================================

#include "StdAfx.h"
#include "UIStalkersRankingWnd.h"
#include "UIXmlInit.h"
#include "UIPdaAux.h"
#include "UIPdaListItem.h"
#include "../Level.h"
#include "../actor.h"
#include "../pda.h"
#include "../UI.h"

//////////////////////////////////////////////////////////////////////////

const char * const		STALKERS_RANKING_XML			= "stalkers_ranking.xml";
const char * const		STALKERS_RANKING_CHARACTER_XML	= "stalkers_ranking_character.xml";

//////////////////////////////////////////////////////////////////////////

void CUIStalkersRankingWnd::Init()
{
	CUIXml		uiXml;
	R_ASSERT3(uiXml.Init("$game_data$",STALKERS_RANKING_XML), "xml file not found", STALKERS_RANKING_XML);

	CUIXmlInit	xml_init;

	inherited::Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	// Декоративное оформление
	AttachChild(&UICharIconFrame);
	xml_init.InitFrameWindow(uiXml, "chicon_frame_window", 0, &UICharIconFrame);
	UICharIconFrame.AttachChild(&UICharIconHeader);
	xml_init.InitFrameLine(uiXml, "chicon_frame_line", 0, &UICharIconHeader);
	UICharIconHeader.AttachChild(&UIAnimatedIcon);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, &UIAnimatedIcon);

	AttachChild(&UIInfoFrame);
	xml_init.InitFrameWindow(uiXml, "info_frame_window", 0, &UIInfoFrame);
	UIInfoFrame.AttachChild(&UIInfoHeader);
	xml_init.InitFrameLine(uiXml, "info_frame_line", 0, &UIInfoHeader);
	UIInfoHeader.UITitleText.SetText(UICharacterInfo.UIName.GetText());

	UIInfoFrame.AttachChild(&UIStalkersList);
	xml_init.InitListWnd(uiXml, "list", 0, &UIStalkersList);
	UIStalkersList.SetMessageTarget(this);
	UIStalkersList.ActivateList(true);
	UIStalkersList.EnableScrollBar(true); 

	UICharIconFrame.AttachChild(&UICharacterWindow);
	UICharacterWindow.SetMessageTarget(this);
	xml_init.InitWindow(uiXml, "character_info", 0, &UICharacterWindow);

	UICharacterWindow.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0,UICharacterWindow.GetWidth(), 
		UICharacterWindow.GetHeight(), 
		STALKERS_RANKING_CHARACTER_XML);
	UICharacterInfo.m_bInfoAutoAdjust = false;
	UICharacterInfo.UIName.Show(false);

	xml_init.InitFrameWindow(uiXml, "mask_frame_window", 0, &UIMask);
	UICharacterInfo.UIIcon.SetMask(&UIMask);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "right_auto_static", &UICharIconFrame);
	xml_init.InitAutoStatic(uiXml, "left_auto_static",  &UIInfoFrame);

	UIInfoFrame.AttachChild(&UIArticleHeader);
	xml_init.InitStatic(uiXml, "article_header_static", 0, &UIArticleHeader);

	string256 buf;
	ZeroMemory(buf, sizeof(buf));
	strconcat(buf, ALL_PDA_HEADER_PREFIX, "/", UIArticleHeader.GetText());
	UIInfoHeader.UITitleText.SetText(buf);
}

//////////////////////////////////////////////////////////////////////////

void CUIStalkersRankingWnd::AddStalkerInfo()
{
	CInventoryOwner *m_pInvOwner = smart_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(!m_pInvOwner) return;

	CPda * m_pPda = m_pInvOwner->GetPDA();

	PDA_LIST_it it;	

	// Добавить список рейтингов
	for(it = m_pPda->m_PDAList.begin(); m_pPda->m_PDAList.end() != it; ++it)
	{	
		CUIPdaListItem* pItem = NULL;
		pItem = xr_new<CUIPdaListItem>();
		UIStalkersList.AddItem<CUIListItem>(pItem); 
		pItem->InitCharacter((*it)->GetOriginalOwner());
		// To ЮРА: самое важное, что надо оставить - это дате элемента задать указатель на
		// CCharacterInfo. Остальное можно перехерячить :)
		pItem->SetData((*it)->GetOriginalOwner());
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIStalkersRankingWnd::Show(bool status)
{
	inherited::Show(status);
	if (status)
	{
		AddStalkerInfo();
		UICharacterWindow.Show(false);
		UICharIconHeader.UITitleText.SetText("");
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIStalkersRankingWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (&UIStalkersList == pWnd && LIST_ITEM_CLICKED == msg)
	{
		UICharacterWindow.Show(true);
		CCharacterInfo &charInfo = reinterpret_cast<CInventoryOwner*>(reinterpret_cast<CUIListItem*>(pData)->GetData())->CharacterInfo();
		UICharacterInfo.InitCharacter(&charInfo);
		UICharIconHeader.UITitleText.SetText(UICharacterInfo.UIName.GetText());
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIStalkersRankingWnd::RemoveStalkers()
{
	UIStalkersList.RemoveAll();
	UICharacterWindow.Show(false);
	UICharIconHeader.UITitleText.SetText("");
}