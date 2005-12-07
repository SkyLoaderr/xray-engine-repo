#include "stdafx.h"
#include "UIGameTDM.h"

#include "UITDMPlayerList.h"
#include "UITDMFragList.h"
#include "UIDMStatisticWnd.h"

#include "hudmanager.h"
#include "game_cl_base.h"

#include "game_cl_TeamDeathmatch.h"

#include "ui/UIFrags2.h"

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
	CUIXml xml_doc;
	bool xml_result = xml_doc.Init(CONFIG_PATH, UI_PATH, "stats.xml");
	R_ASSERT2(xml_result, "xml file not found");

	CUIFrags2* pFragList		= xr_new<CUIFrags2>();			pFragList->SetAutoDelete(true);
	//-----------------------------------------------------------
	CUIDMStatisticWnd* pStatisticWnd = xr_new<CUIDMStatisticWnd>(); pStatisticWnd->SetAutoDelete(true);

	pFragList->Init(xml_doc,"stats_wnd","frag_wnd_tdm");

	float ScreenW = UI_BASE_WIDTH;
	float ScreenH = UI_BASE_HEIGHT;
	//-----------------------------------------------------------
	Frect FrameRect = pFragList->GetWndRect ();
	float FrameW	= FrameRect.right - FrameRect.left;
	float FrameH	= FrameRect.bottom - FrameRect.top;

	pFragList->SetWndPos((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f);
	//-----------------------------------------------------------
	m_pFragLists->AttachChild(pFragList);
	//-----------------------------------------------------------
	CUIFrags2* pPlayerListT1	= xr_new<CUIFrags2>	();pPlayerListT1->SetAutoDelete(true);
//	CUIFrags2* pPlayerListT2	= xr_new<CUIFrags2>	();pPlayerListT2->SetAutoDelete(true);

	pPlayerListT1->Init(xml_doc, "players_wnd", "frag_wnd_tdm");
//	pPlayerListT2->Init(xml_doc, "players_wnd", "frag_wnd_tdm");
	//-----------------------------------------------------------
	FrameRect = pPlayerListT1->GetWndRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pPlayerListT1->SetWndPos((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f);
	//-----------------------------------------------------------
//	FrameRect = pPlayerListT2->GetWndRect ();
//	FrameW	= FrameRect.right - FrameRect.left;
//	FrameH	= FrameRect.bottom - FrameRect.top;

//	pPlayerListT2->SetWndRect(ScreenW/4.0f*3.0f-FrameW/2.0f, (ScreenH - FrameH)/2.0f, FrameW, FrameH);
	//-----------------------------------------------------------
	m_pPlayerLists->AttachChild(pPlayerListT1);
//	m_pPlayerLists->AttachChild(pPlayerListT2);
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
//by adny
//				m_game->Set_ShowPlayerNames(true);
				m_game->Set_ShowPlayerNames( !m_game->Get_ShowPlayerNames() );
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
//					m_game->Set_ShowPlayerNames(false);
					return true;
				};
			}break;
	}
	if(inherited::IR_OnKeyboardRelease(dik)) return true;
	return false;
}
