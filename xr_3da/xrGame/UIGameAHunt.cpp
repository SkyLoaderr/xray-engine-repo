#include "stdafx.h"
#include "UIGameAHunt.h"

#include "UIAHuntPlayerList.h"
#include "UIAHuntFragList.h"

#include "hudmanager.h"
#include "team_base_zone.h"
#include "level.h"
#include "game_cl_ArtefactHunt.h"

#define MSGS_OFFS 510

#define	TEAM1_MENU		"artefacthunt_team1"
#define	TEAM2_MENU		"artefacthunt_team2"

#define BUY_MSG_COLOR		0xffffff00
#define SCORE_MSG_COLOR		0xffffffff
#define ROUND_RESULT_COLOR	0xfff0fff0
#define TODO_MSG_COLOR		0xff00ff00
//--------------------------------------------------------------------
CUIGameAHunt::CUIGameAHunt()
{
	m_bBuyEnabled = TRUE;
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
	CUIAHuntFragList* pFragListT1	= xr_new<CUIAHuntFragList>	();
	CUIAHuntFragList* pFragListT2	= xr_new<CUIAHuntFragList>	();

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
	m_aFragsLists.push_back(pFragListT1);
	m_aFragsLists.push_back(pFragListT2);
	//-----------------------------------------------------------

	CUIAHuntPlayerList* pPlayerListT1	= xr_new<CUIAHuntPlayerList>	();
	CUIAHuntPlayerList* pPlayerListT2	= xr_new<CUIAHuntPlayerList>	();

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
	m_aPlayersLists.push_back(pPlayerListT1);
	m_aPlayersLists.push_back(pPlayerListT2);
	//-----------------------------------------------------------
	string64	Team1, Team2;
	std::strcpy(Team1, TEAM1_MENU);
	std::strcpy(Team2, TEAM2_MENU);
	m_aTeamSections.push_back(Team1);
	m_aTeamSections.push_back(Team2);
	//----------------------------------------------------------------
	pBuyMenuTeam1 = InitBuyMenu("artefacthunt_base_cost", 1);
	pBuyMenuTeam2 = InitBuyMenu("artefacthunt_base_cost", 2);
	//----------------------------------------------------------------
	pSkinMenuTeam1 = InitSkinMenu(1);
	pSkinMenuTeam2 = InitSkinMenu(2);
	//----------------------------------------------------------------


	m_score_caption					=	"ah_score";		
	m_gameCaptions.addCustomMessage(m_score_caption, 0.0f, -0.9f, 0.02f, HUD().pFontDI, SCORE_MSG_COLOR, "");

	m_round_result_caption			=	"ah_round_result";
	m_gameCaptions.addCustomMessage(m_round_result_caption, 0.0f, 0.0f, 0.02f, HUD().pFontDI, ROUND_RESULT_COLOR, "");

	m_todo_caption					=	"ah_todo";
	m_gameCaptions.addCustomMessage(m_todo_caption, 0.0f, -0.85f, 0.02f, HUD().pFontDI, TODO_MSG_COLOR, "");
	m_gameCaptions.customizeMessage(m_todo_caption, CUITextBanner::tbsFlicker)->fPeriod = 0.5f;
	

	m_buy_msg_caption				=	"ah_buy";
	m_gameCaptions.addCustomMessage(m_buy_msg_caption, 0.0f, 0.9f, 0.02f, HUD().pFontDI, BUY_MSG_COLOR, "");

};
//--------------------------------------------------------------------

CUIGameAHunt::~CUIGameAHunt()
{
}
///-------------------------------------------------------------------
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
//--------------------------------------------------------------------
void CUIGameAHunt::OnFrame()
{
	inherited::OnFrame();
/*
	HUD().pFontDI->SetSize	(0.02f);
	CActor* pCurActor = dynamic_cast<CActor*> (Level().CurrentEntity());

	SetBuyMsgCaption("");
	SetScoreCaption("");
	SetTodoCaption("");
	SetRoundResultCaption("");

	switch (m_game->phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			///HUD().GetUI()->ShowIndicators();

			if (m_bBuyEnabled)
			{
				if (pCurActor && pCurActor->g_Alive() && !pCurBuyMenu->IsShown())
				{
					SetBuyMsgCaption("Press B to access Buy Menu");
				};
			};

			if (pCurActor && !pCurBuyMenu->IsShown())
			{
				game_TeamState team0 = m_game->teams[0];
				game_TeamState team1 = m_game->teams[1];

				string256 S;
				sprintf(S,		"Your Team : %3d - Enemy Team %3d - from %3d Artefacts",
								m_game->teams[pCurActor->g_Team()-1].score, 
								m_game->teams[1 - (pCurActor->g_Team()-1)].score, 
								m_game->artefactsNum);
				SetScoreCaption(S);

	
			if ( (m_game->artefactBearerID==0) && (m_game->artefactID!=0) )
				{
					SetTodoCaption("Grab the Artefact");
				}
				else
				{
					if (m_game->teamInPossession != pCurActor->g_Team())
					{
						SetTodoCaption("Stop ArtefactBearer");
					}
					else
					{
						if (pCurActor->ID() == m_game->artefactBearerID)
						{
							SetTodoCaption("You got the Artefact. Bring it to your base.");
						}
						else
						{
							SetTodoCaption("Protect your ArtefactBearer");
						};
					};
				};
			};
		}break;
	case GAME_PHASE_TEAM1_SCORES:
		{
			HUD().GetUI()->HideIndicators();
			SetRoundResultCaption("Team Green WINS!");
		}break;
	case GAME_PHASE_TEAM2_SCORES:
		{
			HUD().GetUI()->HideIndicators();
			SetRoundResultCaption("Team Blue WINS!");
		}break;
	default:
		{
			
		}break;
	};
*/
};
//--------------------------------------------------------------------
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
};

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
