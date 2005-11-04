#include "StdAfx.h"
#include "UIStalkersRankingWnd.h"
#include "UIXmlInit.h"
#include "UIPdaAux.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIPdaListItem.h"
#include "UIAnimatedStatic.h"
#include "UIScrollView.h"
#include "UICharacterInfo.h"
#include "../InventoryOwner.h"
#include "../Level.h"
#include "../pda.h"
#include "../MainUI.h"

const char * const		STALKERS_RANKING_XML			= "stalkers_ranking.xml";
const char * const		STALKERS_RANKING_CHARACTER_XML	= "stalkers_ranking_character.xml";

void CUIStalkersRankingWnd::Init()
{
	CUIXml								uiXml;
	uiXml.Init							(CONFIG_PATH, UI_PATH,STALKERS_RANKING_XML);

	CUIXmlInit							xml_init;

	xml_init.InitWindow					(uiXml, "main_wnd", 0, this);

	UICharIconFrame						= xr_new<CUIFrameWindow>(); UICharIconFrame->SetAutoDelete(true);
	AttachChild							(UICharIconFrame);
	xml_init.InitFrameWindow			(uiXml, "chicon_frame_window", 0, UICharIconFrame);

	UICharIconHeader					= xr_new<CUIFrameLineWnd>(); UICharIconHeader->SetAutoDelete(true);
	UICharIconFrame->AttachChild		(UICharIconHeader);
	xml_init.InitFrameLine				(uiXml, "chicon_frame_line", 0, UICharIconHeader);


	UIInfoFrame							= xr_new<CUIFrameWindow>(); UIInfoFrame->SetAutoDelete(true);
	AttachChild							(UIInfoFrame);
	xml_init.InitFrameWindow			(uiXml, "info_frame_window", 0, UIInfoFrame);
	
	UIInfoHeader						= xr_new<CUIFrameLineWnd>(); UIInfoHeader->SetAutoDelete(true);
	UIInfoFrame->AttachChild			(UIInfoHeader);
	xml_init.InitFrameLine				(uiXml, "info_frame_line", 0, UIInfoHeader);

	UIAnimatedIcon						= xr_new<CUIAnimatedStatic>(); UIAnimatedIcon->SetAutoDelete(true);
	UIInfoHeader->AttachChild			(UIAnimatedIcon);
	xml_init.InitAnimatedStatic			(uiXml, "a_static", 0, UIAnimatedIcon);

	UIList								= xr_new<CUIScrollView>(); UIList->SetAutoDelete(true);
	UIInfoFrame->AttachChild			(UIList);
	xml_init.InitScrollView				(uiXml, "list", 0, UIList);

	UICharacterWindow					= xr_new<CUIWindow>(); UICharacterWindow->SetAutoDelete(true);
	UICharIconFrame->AttachChild		(UICharacterWindow);
	xml_init.InitWindow					(uiXml, "character_info", 0, UICharacterWindow);

	UICharacterInfo						= xr_new<CUICharacterInfo>(); UICharacterInfo->SetAutoDelete(true);
	UICharacterWindow->AttachChild		(UICharacterInfo);
	UICharacterInfo->Init				(0,0,UICharacterWindow->GetWidth(), 
											UICharacterWindow->GetHeight(), 
											STALKERS_RANKING_CHARACTER_XML);

	xml_init.InitAutoStatic				(uiXml, "right_auto_static", UICharIconFrame);
	xml_init.InitAutoStatic				(uiXml, "left_auto_static",  UIInfoFrame);
}


void CUIStalkersRankingWnd::Show(bool status)
{
	inherited::Show(status);
	if (status)
	{
//		AddStalkerInfo							();
		UICharacterWindow->Show					(false);
		UICharIconHeader->UITitleText.SetText	("");
	}
}

#include "../character_info.h"

void CUIStalkersRankingWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
/*
	if (UIList == pWnd && LIST_ITEM_CLICKED == msg)
	{
		UICharacterWindow->Show(true);
		CInventoryOwner* IO = reinterpret_cast<CInventoryOwner*>(reinterpret_cast<CUIListItem*>(pData)->GetData());
		UICharacterInfo->InitCharacter(IO);
		UICharIconHeader->UITitleText.SetText(IO->Name());
	}*/
}
