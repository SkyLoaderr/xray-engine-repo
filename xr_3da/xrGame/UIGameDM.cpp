#include "stdafx.h"
#include "UIGameDM.h"

#include "UIDMPlayerList.h"
#include "UIDMFragList.h"
#include "ui\UIBuyWeaponWnd.h"
#include "ui\UISkinSelector.h"
#include "HUDManager.h"
#include "level.h"
#include "game_cl_base.h"
#include "Spectator.h"
#include "Inventory.h"
#include "xrserver_objects_alife_items.h"

#include "game_cl_Deathmatch.h"

#define MSGS_OFFS 510

#define TIME_MSG_COLOR		0xffff0000
#define SPECTRMODE_MSG_COLOR		0xffff0000
#define NORMAL_MSG_COLOR	0xffffffff
//--------------------------------------------------------------------
CUIGameDM::CUIGameDM()
{
	m_game			= NULL; 
	m_pFragLists					= xr_new<CUIWindow>();
	m_pPlayerLists					= xr_new<CUIWindow>();

	m_time_caption = "timelimit";
	m_gameCaptions.addCustomMessage(m_time_caption, 0.0f, -0.95f, 0.03f, HUD().pFontDI, CGameFont::alCenter, TIME_MSG_COLOR, "");
	m_spectrmode_caption = "spetatormode";
	m_gameCaptions.addCustomMessage(m_spectrmode_caption, 0.0f, -0.9f, 0.03f, HUD().pFontDI, CGameFont::alCenter, SPECTRMODE_MSG_COLOR, "");
	m_spectator_caption = "spectator";
	m_gameCaptions.addCustomMessage(m_spectator_caption, 0.0f, 0.0f, 0.03f, HUD().pFontDI, CGameFont::alCenter, NORMAL_MSG_COLOR, "");
	m_pressjump_caption = "pressjump";
	m_gameCaptions.addCustomMessage(m_pressjump_caption, 0.0f, 0.9f, 0.02f, HUD().pFontDI, CGameFont::alCenter, NORMAL_MSG_COLOR, "");
	m_pressbuy_caption = "pressbuy";
	m_gameCaptions.addCustomMessage(m_pressbuy_caption, 0.0f, 0.95f, 0.02f, HUD().pFontDI, CGameFont::alCenter, NORMAL_MSG_COLOR, "");
	//-----------------------------------------------------------------------
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
	pFragList->SetAutoDelete(true);
	pPlayerList->SetAutoDelete(true);


	int ScreenW = UI_BASE_WIDTH;
	int ScreenH = UI_BASE_HEIGHT;
	//-----------------------------------------------------------
	RECT FrameRect = pFragList->GetFrameRect ();
	int FrameW	= FrameRect.right - FrameRect.left;
	int FrameH	= FrameRect.bottom - FrameRect.top;

	pFragList->SetWndRect((ScreenW-FrameW)/2, (ScreenH - FrameH)/2, FrameW, FrameH);


	m_pFragLists->AttachChild(pFragList);
	//-----------------------------------------------------------
	FrameRect = pPlayerList->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;
	pPlayerList->SetWndRect((ScreenW-FrameW)/2, (ScreenH - FrameH)/2, FrameW, FrameH);

	m_pPlayerLists->AttachChild(pPlayerList);
};
//--------------------------------------------------------------------

void	CUIGameDM::ClearLists ()
{
	m_pFragLists->DetachAll();
	m_pPlayerLists->DetachAll();
}
//--------------------------------------------------------------------
CUIGameDM::~CUIGameDM()
{
	ClearLists();
	xr_delete(m_pFragLists);
	xr_delete(m_pPlayerLists);
}
//--------------------------------------------------------------------

void CUIGameDM::SetTimeMsgCaption		(LPCSTR str)
{
		m_gameCaptions.setCaption(m_time_caption, str, TIME_MSG_COLOR, true);
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

void CUIGameDM::SetSpectrModeMsgCaption		(LPCSTR str)
{
		m_gameCaptions.setCaption(m_spectrmode_caption, str, SPECTRMODE_MSG_COLOR, true);
}

void CUIGameDM::SetSpectatorMsgCaption		(LPCSTR str)
{
		m_gameCaptions.setCaption(m_spectator_caption, str, NORMAL_MSG_COLOR, true);
}

void CUIGameDM::SetPressJumpMsgCaption		(LPCSTR str)
{
		m_gameCaptions.setCaption(m_pressjump_caption, str, NORMAL_MSG_COLOR, true);
}

void CUIGameDM::SetPressBuyMsgCaption		(LPCSTR str)
{
		m_gameCaptions.setCaption(m_pressbuy_caption, str, NORMAL_MSG_COLOR, true);
}

