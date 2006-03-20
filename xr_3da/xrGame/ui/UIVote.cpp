#include "StdAfx.h"
#include "UIVote.h"
#include "UIStatic.h"
#include "UI3tButton.h"
#include "UIListBox.h"
#include "UIFrameWindow.h"
//#include "UIListBoxItem.h"
#include "xrXmlParser.h"
#include "UIXmlInit.h"
#include "../level.h"
#include "../game_cl_base.h"
#include "../game_cl_teamdeathmatch.h"
#include "../../xr_ioconsole.h"

CUIVote::CUIVote(){
	m_prev_upd_time = 0;
	bkgrnd = xr_new<CUIStatic>();	bkgrnd->SetAutoDelete(true);	AttachChild(bkgrnd);
//	header = xr_new<CUIStatic>();	header->SetAutoDelete(true);	AttachChild(header);
	msg = xr_new<CUIStatic>();		msg->SetAutoDelete(true);		AttachChild(msg);
	for (int i = 0; i<3; i++){
		cap[i]		= xr_new<CUIStatic>();		cap[i]->SetAutoDelete(true);	AttachChild(cap[i]);
		frame[i]	= xr_new<CUIFrameWindow>();	frame[i]->SetAutoDelete(true);	AttachChild(frame[i]);
		list[i]		= xr_new<CUIListBox>();		list[i]->SetAutoDelete(true);	AttachChild(list[i]);
	}	

	btn_yes		= xr_new<CUI3tButton>();	btn_yes->SetAutoDelete(true);		AttachChild(btn_yes);
	btn_no		= xr_new<CUI3tButton>();	btn_no->SetAutoDelete(true);		AttachChild(btn_no);
	btn_cancel	= xr_new<CUI3tButton>();	btn_cancel->SetAutoDelete(true);	AttachChild(btn_cancel);
	
	Init();
}

void CUIVote::Init(){
	CUIXml xml_doc;
	xml_doc.Init(CONFIG_PATH, UI_PATH, "voting_category.xml");
	CUIXmlInit::InitWindow(xml_doc, "vote", 0, this);
	CUIXmlInit::InitStatic(xml_doc, "vote:background", 0, bkgrnd);
//	CUIXmlInit::InitStatic(xml_doc, "vote:header", 0, header);
	CUIXmlInit::InitStatic(xml_doc, "vote:msg", 0, msg);

	string256 path;

	for (int i = 0; i<3; i++)
	{
		sprintf(path, "vote:list_cap_%d", i+1);
		CUIXmlInit::InitStatic(xml_doc, path, 0, cap[i]);
		sprintf(path, "vote:list_back_%d", i+1);
		CUIXmlInit::InitFrameWindow(xml_doc, path, 0, frame[i]);
		sprintf(path, "vote:list_%d", i+1);
		CUIXmlInit::InitListBox(xml_doc, path, 0, list[i]);
	}	

	CUIXmlInit::Init3tButton(xml_doc, "vote:btn_yes", 0, btn_yes);
	CUIXmlInit::Init3tButton(xml_doc, "vote:btn_no", 0, btn_no);
	CUIXmlInit::Init3tButton(xml_doc, "vote:btn_cancel", 0, btn_cancel);
}

void CUIVote::SetVoting(LPCSTR txt){
	msg->SetText(txt);
}

void CUIVote::Update(){
	CUIDialogWnd::Update();

	static string512 teaminfo;
	if (m_prev_upd_time > Device.dwTimeContinual - 1000)
		return;
	m_prev_upd_time = Device.dwTimeContinual;
	game_cl_GameState::PLAYERS_MAP_IT I=Game().players.begin();
	game_cl_GameState::PLAYERS_MAP_IT E=Game().players.end();

	items.clear			();
	for (;I!=E;++I)		
	{
//		game_PlayerState* p = (game_PlayerState*) I->second;
		items.push_back(I->second);
	};

    std::sort(items.begin(), items.end(), DM_Compare_Players);

	list[0]->Clear();
	list[1]->Clear();
	list[2]->Clear();

	for (u32 i = 0; i<items.size(); i++){
		game_PlayerState* p = reinterpret_cast<game_PlayerState*>(items[i]);
		if (p->m_bCurrentVoteAgreed == 1)
			list[0]->AddItem(p->name);
		else if (p->m_bCurrentVoteAgreed == 0)
			list[1]->AddItem(p->name);
		else
			list[3]->AddItem(p->name);
	}

    //lst->SetSelected(selected_item);
}

void CUIVote::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
    if (BUTTON_CLICKED == msg)
	{
		if (btn_yes == pWnd)
			OnBtnYes();
		else if (btn_no == pWnd)
			OnBtnNo();
		else if (btn_cancel == pWnd)
			OnBtnCancel();
	}
}

void CUIVote::OnBtnYes(){
    Console->Execute("cl_voteyes");
	game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());
	game->StartStopMenu(this, true);
}

void CUIVote::OnBtnNo(){
    Console->Execute("cl_voteno");
	game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());
	game->StartStopMenu(this, true);
}

void CUIVote::OnBtnCancel(){
	game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());
	game->StartStopMenu(this, true);
}
