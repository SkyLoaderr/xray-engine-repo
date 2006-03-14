#include "stdafx.h"
#include "UIVotingCategory.h"
#include "UIXmlInit.h"
#include "xrXmlParser.h"
#include "UI3tButton.h"
#include "../game_cl_teamdeathmatch.h"
#include "UIKickPlayer.h"

CUIVotingCategory::CUIVotingCategory(){
	xml_doc = NULL;
	kick = NULL;
	bkgrnd = xr_new<CUIStatic>(); 
	bkgrnd->SetAutoDelete(true);
	AttachChild(bkgrnd);

	btn_cancel = xr_new<CUI3tButton>();
	btn_cancel->SetAutoDelete(true);
	AttachChild(btn_cancel);

	for (int i = 0; i<7; i++)
	{
		btn[i] = xr_new<CUI3tButton>();
		btn[i]->SetAutoDelete(true);
		AttachChild(btn[i]);

		txt[i] = xr_new<CUIStatic>();
		txt[i]->SetAutoDelete(true);
		AttachChild(txt[i]);
	}

	Init();
}

CUIVotingCategory::~CUIVotingCategory(){
	xr_delete(kick);
	xr_delete(xml_doc);
}

void CUIVotingCategory::Init(){
	if (!xml_doc)
		xml_doc = xr_new<CUIXml>();

	xml_doc->Init(CONFIG_PATH, UI_PATH, "voting_category.xml");

	CUIXmlInit::InitStatic(*xml_doc, "category:background", 0, bkgrnd);

	string256 _path;
	for (int i = 0; i<7; i++){
		sprintf(_path, "category:btn_%d", i + 1);
		CUIXmlInit::Init3tButton(*xml_doc, _path, 0, btn[i]);
		sprintf(_path, "category:txt_%d", i + 1);
		CUIXmlInit::InitStatic(*xml_doc, _path, 0, txt[i]);
	}

	CUIXmlInit::Init3tButton(*xml_doc, "category:btn_cancel", 0, btn_cancel);
}

void CUIVotingCategory::SendMessage(CUIWindow* pWnd, s16 msg, int pData){
	if (BUTTON_CLICKED == msg)
	{
		if (btn_cancel == pWnd)
			OnBtnCancel();
		for (int i=0; i<6; i++){
			if (btn[i] == pWnd){
				OnBtn(i);
				return;
			}
		}
	}

}

#include <dinput.h>

bool CUIVotingCategory::OnKeyboard(int dik, EUIMessages keyboard_action){
	if (WINDOW_KEY_PRESSED == keyboard_action){
		if (DIK_ESCAPE == dik){
			OnBtnCancel();
			return true;
		}
		if (dik >= DIK_1 && dik <= DIK_7)
			OnBtn(dik - DIK_1);
	}

	return false;
}

#include "../../xr_ioconsole.h"

void CUIVotingCategory::OnBtn(int i){
	game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());
	switch (i){
		case 0:
			Console->Execute("cl_votestart g_restart");
			game->StartStopMenu(this, true);
			break;
		case 1:
			Console->Execute("cl_votestart g_restart_fast");
			game->StartStopMenu(this, true);
			break;
		case 2:
			if (!kick)
				kick = xr_new<CUIKickPlayer>();
			kick->InitKick(*xml_doc);
			game->StartStopMenu(kick, false);
			break;
		case 3:
			if (!kick)
				kick = xr_new<CUIKickPlayer>();
			kick->InitBan(*xml_doc);
			game->StartStopMenu(kick, false);
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
	}

//	game->StartStopMenu(this, true);
}

void CUIVotingCategory::OnBtnCancel(){
	game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());
	game->StartStopMenu(this, true);
}
