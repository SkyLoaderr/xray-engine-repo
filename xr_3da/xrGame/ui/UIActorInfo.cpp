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

//////////////////////////////////////////////////////////////////////////

const char * const		ACTOR_STATISTIC_XML		= "actor_statistic.xml";
const char * const		ACTOR_CHARACTER_XML		= "pda_dialog_character.xml";

//////////////////////////////////////////////////////////////////////////

void CUIActorInfoWnd::Init()
{
	CUIXml		uiXml;
	R_ASSERT3(uiXml.Init("$game_data$",ACTOR_STATISTIC_XML), "xml file not found", ACTOR_STATISTIC_XML);

	CUIXmlInit	xml_init;

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

	UIInfoFrame.AttachChild(&UIInfoList);
	xml_init.InitListWnd(uiXml, "list", 0, &UIInfoList);
	UIInfoList.ActivateList(false);
	UIInfoList.EnableScrollBar(true);

	UICharIconFrame.AttachChild(&UICharacterWindow);
	UICharacterWindow.SetMessageTarget(this);
	xml_init.InitWindow(uiXml, "character_info", 0, &UICharacterWindow);

	UICharacterWindow.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0,UICharacterWindow.GetWidth(), 
		UICharacterWindow.GetHeight(), 
		ACTOR_CHARACTER_XML);
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

void CUIActorInfoWnd::Show(bool status)
{
	inherited::Show(status);
	if (!status) return;
	
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if (pActor)
	{
		UICharacterInfo.InitCharacter(&pActor->CharacterInfo());
		UICharIconHeader.UITitleText.SetText(UICharacterInfo.UIName.GetText());
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIActorInfoWnd::SetLineOfText(int idx, LPCSTR text)
{
	if (idx > UIInfoList.GetSize() - 1)
	{
		for (int i = UIInfoList.GetSize(); i <= idx; ++i)
		{
			UIInfoList.AddItem<CUIListItem>("");
		}
	}

	// Set item's text
	CUIListItem *pItem = UIInfoList.GetItem(idx);
	pItem->SetText(text);
}
