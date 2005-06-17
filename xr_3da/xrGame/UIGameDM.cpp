#include "stdafx.h"
#include "UIGameDM.h"

#include "UIDMPlayerList.h"
#include "UIDMFragList.h"
#include "UIDMStatisticWnd.h"
#include "ui\UIBuyWeaponWnd.h"
#include "ui\UISkinSelector.h"
#include "HUDManager.h"
#include "level.h"
#include "game_cl_base.h"
#include "Spectator.h"
#include "Inventory.h"
#include "xrserver_objects_alife_items.h"
#include "xr_level_controller.h"

#include "game_cl_Deathmatch.h"

#define MSGS_OFFS 510

#define TIME_MSG_COLOR		0xffff0000
#define SPECTRMODE_MSG_COLOR		0xffff0000
#define NORMAL_MSG_COLOR	0xffffffff
#define ROUND_RESULT_COLOR	0xfff0fff0
#define VOTE0_MSG_COLOR	0xffff0000
#define VOTE1_MSG_COLOR	0xff00ff00
#define FRAGS_AND_PLACE_COLOR	0xff00ff00


#define DI2PX(x) float(iFloor((x+1)*float(UI_BASE_WIDTH)*0.5f))
#define DI2PY(y) float(iFloor((y+1)*float(UI_BASE_HEIGHT)*0.5f))
#define SZ(x) x*UI_BASE_WIDTH
//--------------------------------------------------------------------
CUIGameDM::CUIGameDM()
{
	m_game			= NULL; 
	m_pFragLists					= xr_new<CUIWindow>();
	m_pPlayerLists					= xr_new<CUIWindow>();
	m_pStatisticWnds					= xr_new<CUIWindow>();

	m_time_caption = "timelimit";
	GameCaptions()->addCustomMessage(m_time_caption, DI2PX(0.0f), DI2PY(-0.95f), SZ(0.03f), HUD().Font().pFontDI, CGameFont::alCenter, TIME_MSG_COLOR, "");
	m_spectrmode_caption = "spetatormode";
	GameCaptions()->addCustomMessage(m_spectrmode_caption, DI2PX(0.0f), DI2PY(-0.9f), SZ(0.03f), HUD().Font().pFontDI, CGameFont::alCenter, SPECTRMODE_MSG_COLOR, "");
	m_spectator_caption = "spectator";
	GameCaptions()->addCustomMessage(m_spectator_caption, DI2PX(0.0f), DI2PY(0.0f), SZ(0.03f), HUD().Font().pFontDI, CGameFont::alCenter, NORMAL_MSG_COLOR, "");
	m_pressjump_caption = "pressjump";
	GameCaptions()->addCustomMessage(m_pressjump_caption, DI2PX(0.0f), DI2PY(0.9f), SZ(0.02f), HUD().Font().pFontDI, CGameFont::alCenter, NORMAL_MSG_COLOR, "");
	m_pressbuy_caption = "pressbuy";
	GameCaptions()->addCustomMessage(m_pressbuy_caption, DI2PX(0.0f), DI2PY(0.95f), SZ(0.02f), HUD().Font().pFontDI, CGameFont::alCenter, NORMAL_MSG_COLOR, "");
	m_round_result_caption =	"round_result";
	GameCaptions()->addCustomMessage(m_round_result_caption, DI2PX(0.0f), DI2PY(-0.1f), SZ(0.03f), HUD().Font().pFontDI, CGameFont::alCenter, ROUND_RESULT_COLOR, "");
	m_force_respawn_time_caption =	"force_respawn_time";
	GameCaptions()->addCustomMessage(m_force_respawn_time_caption, DI2PX(0.0f), DI2PY(-0.9f), SZ(0.02f), HUD().Font().pFontDI, CGameFont::alCenter, NORMAL_MSG_COLOR, "");
	m_frags_and_place_caption =	"frags_and_place";
	GameCaptions()->addCustomMessage(m_frags_and_place_caption, DI2PX(0.0f), DI2PY(-0.75f), SZ(0.02f), HUD().Font().pFontDI, CGameFont::alCenter, FRAGS_AND_PLACE_COLOR, "");
	//-----------------------------------------------------------------------
	m_vote_caption0 = "votecaption0";
	m_vote_caption1 = "votecaption1";
	m_votetimeresult_caption= "votetimeresultcaption";
	GameCaptions()->addCustomMessage(m_vote_caption0, DI2PX(-1.0f), DI2PY(-0.45f), SZ(0.018f), HUD().Font().pFontDI, CGameFont::alLeft, VOTE0_MSG_COLOR, "");
	GameCaptions()->addCustomMessage(m_vote_caption1, DI2PX(-1.0f), DI2PY(-0.4f), SZ(0.018f), HUD().Font().pFontDI, CGameFont::alLeft, VOTE1_MSG_COLOR, "");
	GameCaptions()->addCustomMessage(m_votetimeresult_caption, DI2PX(-1.0f), DI2PY(-0.35f), SZ(0.018f), HUD().Font().pFontDI, CGameFont::alLeft, VOTE0_MSG_COLOR, "");
}
//--------------------------------------------------------------------
void CUIGameDM::SetClGame (game_cl_GameState* g)
{
	inherited::SetClGame(g);
	m_game = smart_cast<game_cl_Deathmatch*>(g);
	R_ASSERT(m_game);
}

void	CUIGameDM::Init				()
{

	CUIDMFragList* pFragList		= xr_new<CUIDMFragList>		();
	CUIDMPlayerList* pPlayerList	= xr_new<CUIDMPlayerList>	();
	CUIDMStatisticWnd* pStatisticWnd = xr_new<CUIDMStatisticWnd>();
	pFragList->SetAutoDelete(true);
	pPlayerList->SetAutoDelete(true);
	pStatisticWnd->SetAutoDelete(true);


	float ScreenW = UI_BASE_WIDTH;
	float ScreenH = UI_BASE_HEIGHT;
	//-----------------------------------------------------------
	Frect FrameRect = pFragList->GetFrameRect ();
	float FrameW	= FrameRect.right - FrameRect.left;
	float FrameH	= FrameRect.bottom - FrameRect.top;

	pFragList->SetWndRect((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f, FrameW, FrameH);


	m_pFragLists->AttachChild(pFragList);
	//-----------------------------------------------------------
	FrameRect = pPlayerList->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;
	pPlayerList->SetWndRect((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f, FrameW, FrameH);

	m_pPlayerLists->AttachChild(pPlayerList);
	//-----------------------------------------------------------
	FrameRect = pStatisticWnd->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;
	pStatisticWnd->SetWndRect((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f, FrameW, FrameH);

	m_pStatisticWnds->AttachChild(pStatisticWnd);
};
//--------------------------------------------------------------------

void	CUIGameDM::ClearLists ()
{
	m_pFragLists->DetachAll();
	m_pPlayerLists->DetachAll();
	m_pStatisticWnds->DetachAll();
}
//--------------------------------------------------------------------
CUIGameDM::~CUIGameDM()
{
	ClearLists();
	xr_delete(m_pFragLists);
	xr_delete(m_pPlayerLists);
	xr_delete(m_pStatisticWnds);
}

void	CUIGameDM::ReInitInventoryWnd		() 
{
	if (m_game && m_game->GetInventoryWnd() && m_game->GetInventoryWnd()->IsShown())
	{	
		m_game->GetInventoryWnd()->InitInventory();
	};
};
//--------------------------------------------------------------------

void CUIGameDM::SetTimeMsgCaption		(LPCSTR str)
{
		GameCaptions()->setCaption(m_time_caption, str, TIME_MSG_COLOR, true);
}

void CUIGameDM::ShowFragList			(bool bShow)
{
	if(bShow)
		AddDialogToRender(m_pFragLists);
	else
		RemoveDialogToRender(m_pFragLists);

}

void CUIGameDM::ShowPlayersList			(bool bShow)
{
	if(bShow)
		AddDialogToRender(m_pPlayerLists);
	else
		RemoveDialogToRender(m_pPlayerLists);
}
void CUIGameDM::ShowStatistic			(bool bShow)
{
	if(bShow)
		AddDialogToRender(m_pStatisticWnds);
	else
		RemoveDialogToRender(m_pStatisticWnds);
}

void CUIGameDM::SetSpectrModeMsgCaption		(LPCSTR str)
{
		GameCaptions()->setCaption(m_spectrmode_caption, str, SPECTRMODE_MSG_COLOR, true);
}

void CUIGameDM::SetSpectatorMsgCaption		(LPCSTR str)
{
		GameCaptions()->setCaption(m_spectator_caption, str, NORMAL_MSG_COLOR, true);
}

void CUIGameDM::SetPressJumpMsgCaption		(LPCSTR str)
{
		GameCaptions()->setCaption(m_pressjump_caption, str, NORMAL_MSG_COLOR, true);
}

void CUIGameDM::SetPressBuyMsgCaption		(LPCSTR str)
{
		GameCaptions()->setCaption(m_pressbuy_caption, str, NORMAL_MSG_COLOR, true);
}


void CUIGameDM::SetRoundResultCaption(LPCSTR str)
{
	GameCaptions()->setCaption(m_round_result_caption, str, ROUND_RESULT_COLOR, true);
}

void CUIGameDM::SetForceRespawnTimeCaption(LPCSTR str)
{
	GameCaptions()->setCaption(m_force_respawn_time_caption, str, NORMAL_MSG_COLOR, true);
}

void CUIGameDM::SetFragsAndPlaceCaption(LPCSTR str)
{
	GameCaptions()->setCaption(m_frags_and_place_caption, str, FRAGS_AND_PLACE_COLOR, true);
}

void CUIGameDM::SetVoteMessage					(LPCSTR str)
{
	if (str[0])
	{
		GameCaptions()->setCaption(m_vote_caption0, str, VOTE0_MSG_COLOR, true);
		GameCaptions()->setCaption(m_vote_caption1, "Press [ for Yes or ] for No", VOTE1_MSG_COLOR, true);
	}
	else
	{
		GameCaptions()->setCaption(m_vote_caption0, str, VOTE0_MSG_COLOR, true);
		GameCaptions()->setCaption(m_vote_caption1, "", VOTE1_MSG_COLOR, true);
	}
};

void CUIGameDM::SetVoteTimeResultMsg			(LPCSTR str)
{
	GameCaptions()->setCaption(m_votetimeresult_caption, str, VOTE0_MSG_COLOR, true);
}

bool		CUIGameDM::IR_OnKeyboardPress		(int dik)
{
	if(inherited::IR_OnKeyboardPress(dik)) return true;

	switch (key_binding[dik])
	{
	case kINVENTORY: 
	case kBUY:
	case kSKIN:
	case kTEAM:
		case kMAP:
		{
			return Game().OnKeyboardPress(key_binding[dik]);
		}break;
	}
	return false;
};

bool		CUIGameDM::IR_OnKeyboardRelease	(int dik)
{
	return false;
};
