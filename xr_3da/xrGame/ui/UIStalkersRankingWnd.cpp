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

	xml_init.InitAutoStaticGroup		(uiXml, "left_auto",				UIInfoFrame);
	xml_init.InitAutoStaticGroup		(uiXml, "right_auto",				UICharIconFrame);
}


void CUIStalkersRankingWnd::Show(bool status)
{
	inherited::Show(status);
	if (status)
		FillList								();
}
void CUIStalkersRankingWnd::FillList()
{
	CUIXml									uiXml;
	uiXml.Init								(CONFIG_PATH, UI_PATH,STALKERS_RANKING_XML);

	UIList->Clear							();

	uiXml.SetLocalRoot						(uiXml.NavigateToNode("stalkers_list",0));

	for(int i=1; i<21; ++i){
		if(i==20)
			AddActorItem					(&uiXml, i, true);
		else
			AddStalkerItem					(&uiXml, i, NULL);
	}

	UIList->SetSelected						(UIList->GetItem(0) );
}

void CUIStalkersRankingWnd::ShowHumanInfo(u16 id)
{
	UICharacterInfo->InitCharacter(id);
}

void CUIStalkersRankingWnd::AddStalkerItem(CUIXml* xml, int num, CSE_ALifeTraderAbstract* t)
{
	string64								buff;
	CUIStalkerRankingInfoItem* itm		= xr_new<CUIStalkerRankingInfoItem>(this);
	itm->Init							(xml, "item_human", 0);

	sprintf								(buff,"%d.",num);
	itm->m_text1->SetText				(buff);		

	sprintf								(buff,"human %d",num);
	itm->m_text2->SetText				(buff);		

	sprintf								(buff,"%d",num*100);
	itm->m_text3->SetText				(buff);		

	UIList->AddWindow					(itm, true);

}

void CUIStalkersRankingWnd::AddActorItem(CUIXml* xml, int num, bool bAddEllipsis)
{
	string64								buff;
	CUIStalkerRankingInfoItem*				itm;
	if(bAddEllipsis){
		itm									= xr_new<CUIStalkerRankingInfoItem>(this);
		itm->Init							(xml, "item_ellipsis", 0);
		UIList->AddWindow					(itm, true);
	}

	itm										= xr_new<CUIStalkerRankingInfoItem>(this);
	itm->Init								(xml, "item_actor", 0);

	sprintf									(buff,"%d.",num);
	itm->m_text1->SetText					(buff);		


	sprintf									(buff,"Actor");
	itm->m_text2->SetText					(buff);		

	sprintf									(buff,"%d",num*100);
	itm->m_text3->SetText					(buff);		

	UIList->AddWindow						(itm, true);
}


CUIStalkerRankingInfoItem::CUIStalkerRankingInfoItem(CUIStalkersRankingWnd* w)
:m_StalkersRankingWnd(w),m_humanID(u16(-1))
{
}

void CUIStalkerRankingInfoItem::Init	(CUIXml* xml, LPCSTR path, int idx)
{
	XML_NODE* _stored_root					= xml->GetLocalRoot();

	CUIXmlInit								xml_init;
	xml_init.InitWindow						(*xml, path, idx, this);

	xml->SetLocalRoot						(xml->NavigateToNode(path,idx));

	m_text1									= xr_new<CUIStatic>(); m_text1->SetAutoDelete(true);
	AttachChild								(m_text1);
	xml_init.InitStatic						(*xml, "text_1", 0, m_text1);

	m_text2									= xr_new<CUIStatic>(); m_text2->SetAutoDelete(true);
	AttachChild								(m_text2);
	xml_init.InitStatic						(*xml, "text_2", 0, m_text2);

	m_text3									= xr_new<CUIStatic>(); m_text3->SetAutoDelete(true);
	AttachChild								(m_text3);
	xml_init.InitStatic						(*xml, "text_3", 0, m_text3);

	xml_init.InitAutoStaticGroup			(*xml, "auto", this);

	m_stored_alpha							= color_get_A(m_text2->GetTextColor());
	xml->SetLocalRoot						(_stored_root);
}

void CUIStalkerRankingInfoItem::SetSelected	(bool b)
{
	CUISelectable::SetSelected				(b);
	m_text1->SetTextColor( subst_alpha(m_text1->GetTextColor(), b?255:m_stored_alpha ));
	m_text2->SetTextColor( subst_alpha(m_text2->GetTextColor(), b?255:m_stored_alpha ));
	m_text3->SetTextColor( subst_alpha(m_text3->GetTextColor(), b?255:m_stored_alpha ));
	if(b){ 
		m_StalkersRankingWnd->ShowHumanInfo			(0);
	}

}

void CUIStalkerRankingInfoItem::OnMouseDown		(bool left_button)
{
	if(left_button)
		m_StalkersRankingWnd->GetTopList().SetSelected	(this);
}
