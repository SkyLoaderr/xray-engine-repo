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
#include "UIScrollView.h"
#include "UICharacterInfo.h"
#include "../actor_statistic_mgr.h"

const char * const		ACTOR_STATISTIC_XML		= "actor_statistic.xml";
const char * const		ACTOR_CHARACTER_XML		= "pda_dialog_character.xml";


CUIActorInfoWnd::CUIActorInfoWnd()
{}

void CUIActorInfoWnd::Init()
{
	CUIXml									uiXml;
	CUIXmlInit								xml_init;
	uiXml.Init								(CONFIG_PATH, UI_PATH,ACTOR_STATISTIC_XML);

	xml_init.InitWindow						(uiXml, "main_wnd", 0, this);

	// Декоративное оформление
	UICharIconFrame							= xr_new<CUIFrameWindow>();	UICharIconFrame->SetAutoDelete	(true);
	xml_init.InitFrameWindow				(uiXml, "chicon_frame_window", 0, UICharIconFrame);
	AttachChild								(UICharIconFrame);

	UICharIconHeader						= xr_new<CUIFrameLineWnd>();	UICharIconHeader->SetAutoDelete	(true);
	xml_init.InitFrameLine					(uiXml, "chicon_frame_line", 0, UICharIconHeader);
	UICharIconFrame->AttachChild			(UICharIconHeader);

	UIAnimatedIcon							= xr_new<CUIAnimatedStatic>();	UIAnimatedIcon->SetAutoDelete	(true);
	xml_init.InitAnimatedStatic				(uiXml, "a_static", 0, UIAnimatedIcon);
	UICharIconHeader->AttachChild			(UIAnimatedIcon);

	UIInfoFrame								= xr_new<CUIFrameWindow>(); UIInfoFrame->SetAutoDelete	(true);
	xml_init.InitFrameWindow				(uiXml, "info_frame_window", 0, UIInfoFrame);
	AttachChild								(UIInfoFrame);
	
	UIInfoHeader							= xr_new<CUIFrameLineWnd>();UIInfoHeader ->SetAutoDelete(true);
	xml_init.InitFrameLine					(uiXml, "info_frame_line", 0, UIInfoHeader);
	UIInfoFrame->AttachChild				(UIInfoHeader);

	UIDetailList							= xr_new<CUIScrollView>();UIDetailList->SetAutoDelete(true);
	UIInfoFrame->AttachChild				(UIDetailList);
	xml_init.InitScrollView					(uiXml, "detail_list", 0, UIDetailList);

	UIMasterList							= xr_new<CUIScrollView>();UIMasterList->SetAutoDelete(true);
	UICharIconFrame->AttachChild			(UIMasterList);
	xml_init.InitScrollView					(uiXml, "master_list", 0, UIMasterList);

	UICharacterWindow						= xr_new<CUIWindow>();UICharacterWindow->SetAutoDelete(true);
	UICharIconFrame->AttachChild			(UICharacterWindow);
	xml_init.InitWindow						(uiXml, "character_info", 0, UICharacterWindow);

	UICharacterInfo							= xr_new<CUICharacterInfo>(); UICharacterInfo->SetAutoDelete(true);
	UICharacterWindow->AttachChild			(UICharacterInfo);
	UICharacterInfo->Init					(0,0,UICharacterWindow->GetWidth(), UICharacterWindow->GetHeight(), ACTOR_CHARACTER_XML);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic					(uiXml, "right_auto_static", UICharIconFrame);
	xml_init.InitAutoStatic					(uiXml, "left_auto_static",  UIInfoFrame);

}


void CUIActorInfoWnd::Show(bool status)
{
	inherited::Show(status);
	if (!status) return;
	
	UICharacterInfo->InitCharacter			(Actor());
	UICharIconHeader->UITitleText.SetText	(Actor()->Name());
	FillInfo								();
}


void CUIActorInfoWnd::FillInfo			()
{
	CUIXml									uiXml;
	uiXml.Init								(CONFIG_PATH, UI_PATH,ACTOR_STATISTIC_XML);

	UIMasterList->Clear						();

	int items_num = uiXml.GetNodesNum		("actor_details_wnd", 0, "master_part");
	uiXml.SetLocalRoot						(uiXml.NavigateToNode("actor_details_wnd",0));
	string64 buff;
	
	for(int i=0; i<items_num; ++i){
		CUIActorStaticticHeader* itm		= xr_new<CUIActorStaticticHeader>(this);
		itm->Init							(&uiXml, "master_part", i);
		
		sprintf								(buff,"%d", Actor()->StatisticMgr().GetSectionPoints(itm->m_index));
		itm->SetText1						(buff);
		UIMasterList->AddWindow				(itm, true);
	}

}

void CUIActorInfoWnd::FillDetail	(int idx)
{
	UIDetailList->Clear						();
	CUIXml									uiXml;
	uiXml.Init								(CONFIG_PATH, UI_PATH,ACTOR_STATISTIC_XML);
	uiXml.SetLocalRoot						(uiXml.NavigateToNode("actor_details_wnd",0));
	
	string512 path;
	sprintf									(path,"detail_part_%d",idx);
	
	XML_NODE* n								= uiXml.NavigateToNode(path,0);
	if(!n)
		sprintf								(path,"detail_part_def");

	SStatSectionData&	section				= Actor()->StatisticMgr().GetSection(idx);
	vStatDetailData::const_iterator it		= section.data.begin();
	vStatDetailData::const_iterator it_e	= section.data.end();

	int _cntr = 0;
	string64 buff;
	for(;it!=it_e;++it,++_cntr){
		CUIActorStaticticDetail* itm			= xr_new<CUIActorStaticticDetail>();
		itm->Init								(&uiXml, path, 0);

		sprintf									(buff,"%d. %s",_cntr, *((*it).key));
		itm->SetText1							(buff);

		sprintf									(buff,"x%d", (*it).count);
		itm->SetText2							(buff);

		sprintf									(buff,"%d", (*it).points);
		itm->SetText3							(buff);

		UIDetailList->AddWindow					(itm, true);
	}
}


CUIActorStaticticHeader::CUIActorStaticticHeader	(CUIActorInfoWnd* w)
:m_actorInfoWnd(w)
{}

void CUIActorStaticticHeader::SendMessage				(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

void CUIActorStaticticHeader::Init	(CUIXml* xml, LPCSTR path, int idx)
{
	XML_NODE* _stored_root				= xml->GetLocalRoot();

	CUIXmlInit							xml_init;
	xml_init.InitWindow					(*xml, path, idx, this);

	xml->SetLocalRoot					(xml->NavigateToNode(path,idx));

	m_text								= xr_new<CUIStatic>(); m_text->SetAutoDelete(true);
	AttachChild							(m_text);
	xml_init.InitStatic					(*xml, "caption_static", 0, m_text);

	m_num								= xr_new<CUIStatic>(); m_num->SetAutoDelete(true);
	AttachChild							(m_num);
	xml_init.InitStatic					(*xml, "num_static", 0, m_num);

	xml_init.InitAutoStaticGroup		(*xml, "auto", this);

	m_index								= _ParseItem(xml->ReadAttrib(xml->GetLocalRoot(),"id",0), actor_stats_token);

	m_stored_alpha						= color_get_A(m_text->GetTextColor());
	xml->SetLocalRoot					(_stored_root);

}

void CUIActorStaticticHeader::OnMouseDown	(bool left_button)
{
	if(left_button && m_index!=100)
		m_actorInfoWnd->MasterList().SetSelected	(this);
}

void CUIActorStaticticHeader::SetSelected(bool b)
{
	CUISelectable::SetSelected(b);
	m_text->SetTextColor( subst_alpha(m_text->GetTextColor(), b?255:m_stored_alpha ));
	if(b) 
		m_actorInfoWnd->FillDetail					(m_index);
}

void CUIActorStaticticHeader::SetText1				(LPCSTR str)
{
	m_num->SetText		(str);
}


void CUIActorStaticticDetail::Init		(CUIXml* xml, LPCSTR path, int idx)
{
	XML_NODE* _stored_root				= xml->GetLocalRoot();

	CUIXmlInit							xml_init;
	xml_init.InitWindow					(*xml, path, idx, this);

	xml->SetLocalRoot					(xml->NavigateToNode(path,idx));

	m_text								= xr_new<CUIStatic>(); m_text->SetAutoDelete(true);
	AttachChild							(m_text);
	xml_init.InitStatic					(*xml, "caption_static", 0, m_text);

	m_num1								= xr_new<CUIStatic>(); m_num1->SetAutoDelete(true);
	AttachChild							(m_num1);
	xml_init.InitStatic					(*xml, "num_static1", 0, m_num1);
	
	m_num2								= xr_new<CUIStatic>(); m_num2->SetAutoDelete(true);
	AttachChild							(m_num2);
	xml_init.InitStatic					(*xml, "num_static2", 0, m_num2);

	xml_init.InitAutoStaticGroup		(*xml, "auto", this);

	xml->SetLocalRoot					(_stored_root);
}

void CUIActorStaticticDetail::SetText1(LPCSTR str)
{
	m_text->SetText		(str);
}

void CUIActorStaticticDetail::SetText2(LPCSTR str)
{
	m_num1->SetText		(str);
}

void CUIActorStaticticDetail::SetText3(LPCSTR str)
{
	m_num2->SetText		(str);
}

