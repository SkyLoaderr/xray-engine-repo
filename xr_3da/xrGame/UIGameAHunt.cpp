#include "stdafx.h"
#include "UIGameAHunt.h"

#include "UIAHuntPlayerList.h"
#include "UIAHuntFragList.h"

#include "hudmanager.h"
#include "team_base_zone.h"
#include "level.h"
#include "game_cl_ArtefactHunt.h"

#define MSGS_OFFS 510
//  moved to game_cl_artefactHunt
//	#define	TEAM1_MENU		"artefacthunt_team1"
//	#define	TEAM2_MENU		"artefacthunt_team2"

#define BUY_MSG_COLOR		0xffffff00
#define SCORE_MSG_COLOR		0xffffffff
#define REINFORCEMENT_MSG_COLOR		0xff8080ff
#define ROUND_RESULT_COLOR	0xfff0fff0
#define TODO_MSG_COLOR		0xff00ff00
//--------------------------------------------------------------------
CUIGameAHunt::CUIGameAHunt()
{
//	m_bBuyEnabled = TRUE;
}
//--------------------------------------------------------------------
void CUIGameAHunt::SetClGame (game_cl_GameState* g)
{
	inherited::SetClGame(g);
	m_game = dynamic_cast<game_cl_ArtefactHunt*>(g);
	R_ASSERT(m_game);
}

void CUIGameAHunt::Init	()
{
	//-----------------------------------------------------------
	CUIAHuntFragList* pFragListT1	= xr_new<CUIAHuntFragList>	();pFragListT1->SetAutoDelete(true);
	CUIAHuntFragList* pFragListT2	= xr_new<CUIAHuntFragList>	();pFragListT2->SetAutoDelete(true);

	pFragListT1->Init(1);
	pFragListT2->Init(2);

	int ScreenW = Device.dwWidth;
	int ScreenH = Device.dwHeight;
	//-----------------------------------------------------------
	RECT FrameRect = pFragListT1->GetFrameRect ();
	int FrameW	= FrameRect.right - FrameRect.left;
	int FrameH	= FrameRect.bottom - FrameRect.top;

	pFragListT1->SetWndRect(ScreenW/4-FrameW/2, (ScreenH - FrameH)/2, FrameW, FrameH);
	//-----------------------------------------------------------
	FrameRect = pFragListT2->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pFragListT2->SetWndRect(ScreenW/4*3-FrameW/2, (ScreenH - FrameH)/2, FrameW, FrameH);
	//-----------------------------------------------------------
//	m_aFragsLists.push_back(pFragListT1);
	m_pFragLists->AttachChild(pFragListT1);
//	m_aFragsLists.push_back(pFragListT2);
	m_pFragLists->AttachChild(pFragListT2);
	//-----------------------------------------------------------

	CUIAHuntPlayerList* pPlayerListT1	= xr_new<CUIAHuntPlayerList>	();pPlayerListT1->SetAutoDelete(true);
	CUIAHuntPlayerList* pPlayerListT2	= xr_new<CUIAHuntPlayerList>	();pPlayerListT2->SetAutoDelete(true);

	pPlayerListT1->Init(1);
	pPlayerListT2->Init(2);
	//-----------------------------------------------------------
	FrameRect = pPlayerListT1->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pPlayerListT1->SetWndRect(ScreenW/4-FrameW/2, (ScreenH - FrameH)/2, FrameW, FrameH);
	//-----------------------------------------------------------
	FrameRect = pPlayerListT2->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pPlayerListT2->SetWndRect(ScreenW/4*3-FrameW/2, (ScreenH - FrameH)/2, FrameW, FrameH);
	//-----------------------------------------------------------
//	m_aPlayersLists.push_back(pPlayerListT1);
	m_pPlayerLists->AttachChild(pPlayerListT1);
//	m_aPlayersLists.push_back(pPlayerListT2);
	m_pPlayerLists->AttachChild(pPlayerListT2);
	//-----------------------------------------------------------
// moved to game_cl_artefactHunt
//	string64	Team1, Team2;
//	std::strcpy(Team1, TEAM1_MENU);
//	std::strcpy(Team2, TEAM2_MENU);
//	m_aTeamSections.push_back(Team1);
//	m_aTeamSections.push_back(Team2);
//	//----------------------------------------------------------------
//	pBuyMenuTeam1 = InitBuyMenu("artefacthunt_base_cost", 1);
//	pBuyMenuTeam2 = InitBuyMenu("artefacthunt_base_cost", 2);
//	//----------------------------------------------------------------
//	pSkinMenuTeam1 = InitSkinMenu(1);
//	pSkinMenuTeam2 = InitSkinMenu(2);
	//----------------------------------------------------------------

	m_reinforcement_caption			=	"ah_reinforcement";		
	m_gameCaptions.addCustomMessage(m_reinforcement_caption, 0.0f, -0.9f, 0.02f, HUD().pFontDI, CGameFont::alCenter, REINFORCEMENT_MSG_COLOR, "");

	m_score_caption					=	"ah_score";		
	m_gameCaptions.addCustomMessage(m_score_caption, 0.0f, -0.85f, 0.02f, HUD().pFontDI, CGameFont::alCenter, SCORE_MSG_COLOR, "");

	m_round_result_caption			=	"ah_round_result";
	m_gameCaptions.addCustomMessage(m_round_result_caption, 0.0f, 0.0f, 0.02f, HUD().pFontDI, CGameFont::alCenter, ROUND_RESULT_COLOR, "");

	m_todo_caption					=	"ah_todo";
	m_gameCaptions.addCustomMessage(m_todo_caption, 0.0f, -0.8f, 0.02f, HUD().pFontDI, CGameFont::alCenter, TODO_MSG_COLOR, "");
	m_gameCaptions.customizeMessage(m_todo_caption, CUITextBanner::tbsFlicker)->fPeriod = 0.5f;

	m_buy_msg_caption				=	"ah_buy";
	m_gameCaptions.addCustomMessage(m_buy_msg_caption, 0.0f, 0.9f, 0.02f, HUD().pFontDI, CGameFont::alCenter, BUY_MSG_COLOR, "");

};
//--------------------------------------------------------------------

CUIGameAHunt::~CUIGameAHunt()
{
}


///-------------------------------------------------------------------
/* moved to game_cl_artefactHunt
void		CUIGameAHunt::OnObjectEnterTeamBase	(CObject *tpObject, CTeamBaseZone* pTeamBaseZone)
{
	CActor* pActor = dynamic_cast<CActor*> (tpObject);
	if (tpObject == Level().CurrentEntity() && pActor->g_Team() == pTeamBaseZone->GetZoneTeam())
	{
		m_bBuyEnabled = TRUE;
	};
};

void CUIGameAHunt::OnObjectLeaveTeamBase	(CObject *tpObject, CTeamBaseZone* pTeamBaseZone)
{
	CActor* pActor = dynamic_cast<CActor*> (tpObject);
	if (tpObject == Level().CurrentEntity() && pActor->g_Team() == pTeamBaseZone->GetZoneTeam())
	{
		m_bBuyEnabled = FALSE;
	};
};

BOOL		CUIGameAHunt::CanCallBuyMenu			()
{
	if (!m_bBuyEnabled) return FALSE;

	CActor* pCurActor = dynamic_cast<CActor*> (Level().CurrentEntity());
	if (!pCurActor || !pCurActor->g_Alive()) return FALSE;

	return TRUE;
};

bool		CUIGameAHunt::CanBeReady				()
{
	SetCurrentSkinMenu();
	SetCurrentBuyMenu();

	if (!m_bTeamSelected)
	{
		StartStopMenu(pUITeamSelectWnd);
		return false;
	};

	if (pCurBuyMenu && !pCurBuyMenu->IsShown())
		ClearBuyMenu();

	return true;
};*/

void CUIGameAHunt::SetReinforcementCaption(LPCSTR str)
{
	m_gameCaptions.setCaption(m_reinforcement_caption, str, REINFORCEMENT_MSG_COLOR, true);
}

void CUIGameAHunt::SetScoreCaption(LPCSTR str)
{
	m_gameCaptions.setCaption(m_score_caption, str, SCORE_MSG_COLOR, true);
}

void CUIGameAHunt::SetRoundResultCaption(LPCSTR str)
{
	m_gameCaptions.setCaption(m_round_result_caption, str, ROUND_RESULT_COLOR, true);
}

void CUIGameAHunt::SetTodoCaption(LPCSTR str)
{
	m_gameCaptions.setCaption(m_todo_caption, str, TODO_MSG_COLOR, true);
}

void CUIGameAHunt::SetBuyMsgCaption(LPCSTR str)
{
	m_gameCaptions.setCaption(m_buy_msg_caption, str, BUY_MSG_COLOR, true);
}
