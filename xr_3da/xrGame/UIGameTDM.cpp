#include "stdafx.h"
#include "UIGameTDM.h"

#include "UITDMPlayerList.h"
#include "UITDMFragList.h"
#include "UIDMStatisticWnd.h"

#include "hudmanager.h"
#include "game_cl_base.h"

#include "game_cl_TeamDeathmatch.h"

#define MSGS_OFFS 510

//--------------------------------------------------------------------
CUIGameTDM::CUIGameTDM()
{
	m_game				= NULL;
}
//--------------------------------------------------------------------
void CUIGameTDM::SetClGame (game_cl_GameState* g)
{
	inherited::SetClGame(g);
	m_game = smart_cast<game_cl_TeamDeathmatch*>(g);
	R_ASSERT(m_game);
}

void CUIGameTDM::Init ()
{
	//-----------------------------------------------------------
	CUITDMFragList* pFragListT1	= xr_new<CUITDMFragList>	();pFragListT1->SetAutoDelete(true);
	CUITDMFragList* pFragListT2	= xr_new<CUITDMFragList>	();pFragListT2->SetAutoDelete(true);
	CUIDMStatisticWnd* pStatisticWnd = xr_new<CUIDMStatisticWnd>(); pStatisticWnd->SetAutoDelete(true);

	pFragListT1->Init(1);
	pFragListT2->Init(2);

	float ScreenW = UI_BASE_WIDTH;
	float ScreenH = UI_BASE_HEIGHT;
	//-----------------------------------------------------------
	Frect FrameRect = pFragListT1->GetFrameRect ();
	float FrameW	= FrameRect.right - FrameRect.left;
	float FrameH	= FrameRect.bottom - FrameRect.top;

	pFragListT1->SetWndRect(ScreenW/4.0f-FrameW/2.0f, (ScreenH - FrameH)/2.0f, FrameW, FrameH);
	//-----------------------------------------------------------
	FrameRect = pFragListT2->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pFragListT2->SetWndRect(ScreenW/4.0f*3.0f-FrameW/2.0f, (ScreenH - FrameH)/2.0f, FrameW, FrameH);
	//-----------------------------------------------------------
	m_pFragLists->AttachChild(pFragListT1);
	m_pFragLists->AttachChild(pFragListT2);
	//-----------------------------------------------------------
	CUITDMPlayerList* pPlayerListT1	= xr_new<CUITDMPlayerList>	();pPlayerListT1->SetAutoDelete(true);
	CUITDMPlayerList* pPlayerListT2	= xr_new<CUITDMPlayerList>	();pPlayerListT2->SetAutoDelete(true);

	pPlayerListT1->Init(1);
	pPlayerListT2->Init(2);
	//-----------------------------------------------------------
	FrameRect = pPlayerListT1->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pPlayerListT1->SetWndRect(ScreenW/4.0f-FrameW/2.0f, (ScreenH - FrameH)/2.0f, FrameW, FrameH);
	//-----------------------------------------------------------
	FrameRect = pPlayerListT2->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pPlayerListT2->SetWndRect(ScreenW/4.0f*3.0f-FrameW/2.0f, (ScreenH - FrameH)/2.0f, FrameW, FrameH);
	//-----------------------------------------------------------
	m_pPlayerLists->AttachChild(pPlayerListT1);
	m_pPlayerLists->AttachChild(pPlayerListT2);
	//-----------------------------------------------------------
	FrameRect = pStatisticWnd->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;
	pStatisticWnd->SetWndRect((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f, FrameW, FrameH);

	m_pStatisticWnds->AttachChild(pStatisticWnd);
}
//--------------------------------------------------------------------
CUIGameTDM::~CUIGameTDM()
{
}
//--------------------------------------------------------------------
bool CUIGameTDM::IR_OnKeyboardPress(int dik)
{
	switch (dik) {
		case DIK_CAPSLOCK :
		{
			if (m_game)
			{
				m_game->Set_ShowPlayerNames(true);
				return true;
			};
		}break;
	}
	if(inherited::IR_OnKeyboardPress(dik)) return true;
	return false;
}

bool CUIGameTDM::IR_OnKeyboardRelease(int dik)
{
	switch (dik) {
		case DIK_CAPSLOCK :
			{
				if (m_game)
				{
					m_game->Set_ShowPlayerNames(false);
					return true;
				};
			}break;
	}
	if(inherited::IR_OnKeyboardRelease(dik)) return true;
	return false;
}
