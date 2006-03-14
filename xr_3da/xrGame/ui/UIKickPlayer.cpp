#include "StdAfx.h"

#include "UIKickPlayer.h"
#include "UIVotingCategory.h"
#include "UIXmlInit.h"
#include "xrXmlParser.h"
#include "UI3tButton.h"
#include "UIListBox.h"
#include "../level.h"
#include "../game_cl_base.h"
//#include "../game_cl_teamdeathmatch.h"

CUIKickPlayer::CUIKickPlayer(){
	m_prev_upd_time = 0;
	bkgrnd = xr_new<CUIStatic>(); bkgrnd->SetAutoDelete(true);
	AttachChild(bkgrnd);

	lst_back = xr_new<CUIFrameWindow>(); lst_back->SetAutoDelete(true);
	AttachChild(lst_back);

	lst = xr_new<CUIListBox>(); lst->SetAutoDelete(true);
	AttachChild(lst);

	btn_ok = xr_new<CUI3tButton>(); btn_ok->SetAutoDelete(true);
	AttachChild(btn_ok);

	btn_cancel = xr_new<CUI3tButton>(); btn_cancel->SetAutoDelete(true);
	AttachChild(btn_cancel);
}

void CUIKickPlayer::Init(CUIXml& xml_doc){
	CUIXmlInit::InitStatic(xml_doc, "kick_ban:background", 0, bkgrnd);
	CUIXmlInit::InitFrameWindow(xml_doc, "kick_ban:list_back", 0, lst_back);
	CUIXmlInit::InitListBox(xml_doc, "kick_ban:list", 0, lst);
	CUIXmlInit::Init3tButton(xml_doc, "kick_ban:btn_ok", 0, btn_ok);
	CUIXmlInit::Init3tButton(xml_doc, "kick_ban:btn_cancel", 0, btn_cancel);
}

void CUIKickPlayer::InitBan(CUIXml& xml_doc){
    Init(xml_doc);
}

void CUIKickPlayer::InitKick(CUIXml& xml_doc){
	Init(xml_doc);
}

bool CUIKickPlayer::OnKeyboard(int dik, EUIMessages keyboard_action){
	return CUIDialogWnd::OnKeyboard(dik, keyboard_action);
}

void CUIKickPlayer::SendMessage(CUIWindow* pWnd, s16 msg, int pData){

}

void CUIKickPlayer::OnBtnOk(){

}

void CUIKickPlayer::OnBtnCancel(){

}

IC bool	DM_Compare_Players		(LPVOID v1, LPVOID v2);

void CUIKickPlayer::Update(){
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

	lst->Clear();

	for (u32 i = 0; i<items.size(); i++){
		game_PlayerState* p = reinterpret_cast<game_PlayerState*>(items[i]);
		lst->AddItem(p->name);
	}

}