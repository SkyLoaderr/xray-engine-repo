//=============================================================================
//  Filename:   UIActorInfo.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Окно информации о актере
//=============================================================================

#include "StdAfx.h"
#include "UIActorInfo.h"
#include "UIXmlInit.h"
#include "UIPdaAux.h"
#include "../Level.h"
#include "../actor.h"

#include "UIInventoryUtilities.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIAnimatedStatic.h"
#include "UIListWnd.h"
#include "UICharacterInfo.h"

const char * const		ACTOR_STATISTIC_XML		= "actor_statistic.xml";
const char * const		ACTOR_CHARACTER_XML		= "pda_dialog_character.xml";

//////////////////////////////////////////////////////////////////////////
CUIActorInfoWnd::CUIActorInfoWnd()
{
	UIInfoFrame				= NULL;
	UICharIconFrame			= NULL;
	UIInfoHeader			= NULL;
	UICharIconHeader		= NULL;
	UIAnimatedIcon			= NULL;
	UIArticleHeader			= NULL;
	UICharacterWindow		= NULL;
	UICharacterInfo			= NULL;
	UIMask					= NULL;
	UIInfoList				= NULL;
}

void CUIActorInfoWnd::Init()
{
	CUIXml		uiXml;
	R_ASSERT3(uiXml.Init(CONFIG_PATH, UI_PATH,ACTOR_STATISTIC_XML), "xml file not found", ACTOR_STATISTIC_XML);

	CUIXmlInit	xml_init;

	xml_init.InitWindow(uiXml, "main_wnd", 0, this);

	// Декоративное оформление
	UICharIconFrame = xr_new<CUIFrameWindow>();	UICharIconFrame->SetAutoDelete	(true);
	AttachChild(UICharIconFrame);
	xml_init.InitFrameWindow(uiXml, "chicon_frame_window", 0, UICharIconFrame);

	UICharIconHeader = xr_new<CUIFrameLineWnd>();	UICharIconHeader->SetAutoDelete	(true);
	UICharIconFrame->AttachChild(UICharIconHeader);
	xml_init.InitFrameLine(uiXml, "chicon_frame_line", 0, UICharIconHeader);

	UIAnimatedIcon = xr_new<CUIAnimatedStatic>();	UIAnimatedIcon->SetAutoDelete	(true);
	UICharIconHeader->AttachChild(UIAnimatedIcon);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, UIAnimatedIcon);

	UIInfoFrame = xr_new<CUIFrameWindow>(); UIInfoFrame->SetAutoDelete	(true);
	AttachChild(UIInfoFrame);
	xml_init.InitFrameWindow(uiXml, "info_frame_window", 0, UIInfoFrame);
	
	
	UIInfoHeader = xr_new<CUIFrameLineWnd>();UIInfoHeader ->SetAutoDelete(true);
	UIInfoFrame->AttachChild(UIInfoHeader);
	xml_init.InitFrameLine(uiXml, "info_frame_line", 0, UIInfoHeader);

	UIInfoList = xr_new<CUIListWnd>();UIInfoList->SetAutoDelete(true);
	UIInfoFrame->AttachChild(UIInfoList);
	xml_init.InitListWnd(uiXml, "list", 0, UIInfoList);
	UIInfoList->ActivateList(false);
	UIInfoList->EnableScrollBar(true);

	UICharacterWindow = xr_new<CUIWindow>();UICharacterWindow->SetAutoDelete(true);
	UICharIconFrame->AttachChild(UICharacterWindow);
	UICharacterWindow->SetMessageTarget(this);
	xml_init.InitWindow(uiXml, "character_info", 0, UICharacterWindow);

	UICharacterInfo = xr_new<CUICharacterInfo>(); UICharacterInfo->SetAutoDelete(true);
	UICharacterWindow->AttachChild(UICharacterInfo);
	UICharacterInfo->Init(0,0,UICharacterWindow->GetWidth(), UICharacterWindow->GetHeight(), ACTOR_CHARACTER_XML);
	UICharacterInfo->m_bInfoAutoAdjust = false;
	UICharacterInfo->UIName().Show(false);

	UIMask = xr_new<CUIFrameWindow>();UIMask->SetAutoDelete(true);
	xml_init.InitFrameWindow(uiXml, "mask_frame_window", 0, UIMask);
	UICharacterInfo->UIIcon().SetMask(UIMask);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "right_auto_static", UICharIconFrame);
	xml_init.InitAutoStatic(uiXml, "left_auto_static",  UIInfoFrame);

	UIArticleHeader = xr_new<CUIStatic>(); UIArticleHeader->SetAutoDelete(true);
	UIInfoFrame->AttachChild(UIArticleHeader);
	xml_init.InitStatic(uiXml, "article_header_static", 0, UIArticleHeader);

	UIInfoHeader->UITitleText.SetText(UICharacterInfo->UIName().GetText());

}


void CUIActorInfoWnd::Show(bool status)
{
	inherited::Show(status);
	if (!status) return;
	
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if (pActor)
	{
		UICharacterInfo->InitCharacter(&pActor->CharacterInfo());
		UICharIconHeader->UITitleText.SetText(UICharacterInfo->UIName().GetText());
	}
}


void CUIActorInfoWnd::SetLineOfText(int idx, LPCSTR text)
{
	if (idx > UIInfoList->GetSize() - 1)
	{
		for (int i = UIInfoList->GetSize(); i <= idx; ++i)
		{
			UIInfoList->AddItem<CUIListItem>("");
		}
	}

	// Set item's text
	CUIListItem *pItem = UIInfoList->GetItem(idx);
	pItem->SetText(text);
}
