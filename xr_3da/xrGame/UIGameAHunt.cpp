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

/*
	BuyMsg.SetStyleParams			(CUITextBanner::tbsNone);
	BuyMsg.SetFont					(HUD().pFontDI);
	BuyMsg.SetTextColor				(0xffffff00);
	
	StaticMsg.SetFont				(HUD().pFontDI);

	WarningMsg.SetStyleParams		(CUITextBanner::tbsFade)->fPeriod = 1;
	WarningMsg.SetFont				(HUD().pFontDI);
	WarningMsg.SetTextColor			(0xffff0000);

	WarningMsg2.SetStyleParams		(CUITextBanner::tbsFlicker)->fPeriod = 0.5f;
	WarningMsg2.SetFont				(HUD().pFontDI);

*/

	m_score_caption					=	"ah_score";		
	m_gameCaptions.addCustomMessage(m_score_caption, 0.0f, -0.9f, HUD().pFontDI, 0xffffffff, "");

	m_round_result_caption			=	"ah_round_result";
	m_gameCaptions.addCustomMessage(m_round_result_caption, 0.0f, 0.0f, HUD().pFontDI, 0xfff0fff0, "");

	m_todo_caption					=	"ah_todo";
	m_gameCaptions.addCustomMessage(m_todo_caption, 0.0f, -0.85f, HUD().pFontDI, 0xffffffff, "");
	m_gameCaptions.customizeMessage(m_todo_caption, CUITextBanner::tbsFlicker)->fPeriod = 0.5f;
	

	m_buy_msg_caption				=	"ah_buy";
	m_gameCaptions.addCustomMessage(m_buy_msg_caption, 0.0f, 0.9f, HUD().pFontDI, 0xffffff00, "");

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

void		CUIGameAHunt::OnObjectLeaveTeamBase	(CObject *tpObject, CTeamBaseZone* pTeamBaseZone)
{
	CActor* pActor = dynamic_cast<CActor*> (tpObject);
	if (tpObject == Level().CurrentEntity() && pActor->g_Team() == pTeamBaseZone->GetZoneTeam())
	{
		m_bBuyEnabled = FALSE;
	};
};
//--------------------------------------------------------------------
void			CUIGameAHunt::OnFrame()
{
	inherited::OnFrame();

	HUD().pFontDI->SetSize	(0.02f);
	CActor* pCurActor = dynamic_cast<CActor*> (Level().CurrentEntity());

	m_gameCaptions.setCaption(m_buy_msg_caption, "");
	m_gameCaptions.setCaption(m_score_caption, "");
	m_gameCaptions.setCaption(m_todo_caption, "");
	m_gameCaptions.setCaption(m_round_result_caption, "");
	switch (m_game->phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			///HUD().GetUI()->ShowIndicators();

			if (m_bBuyEnabled)
			{
				if (pCurActor && pCurActor->g_Alive() && !pCurBuyMenu->IsShown())
				{
//					BuyMsg.Out					(0.0f,0.9f,"Press B to access Buy Menu");
//					BuyMsg.Update				();
					m_gameCaptions.setCaption(m_buy_msg_caption, "Press B to access Buy Menu");
				};
			};

			if (pCurActor && !pCurBuyMenu->IsShown())
			{
				game_TeamState team0 = m_game->teams[0];
				game_TeamState team1 = m_game->teams[1];

//				StaticMsg.Out	(0.0f, -0.9f, "Your Team : %3d - Enemy Team %3d - from %3d Artefacts",
//					m_game->teams[pCurActor->g_Team()-1].score, 
//					m_game->teams[1 - (pCurActor->g_Team()-1)].score, 
//					m_game->artefactsNum);
//
//				StaticMsg.Update				();

				string256 S;
				sprintf(S,		"Your Team : %3d - Enemy Team %3d - from %3d Artefacts",
								m_game->teams[pCurActor->g_Team()-1].score, 
								m_game->teams[1 - (pCurActor->g_Team()-1)].score, 
								m_game->artefactsNum);
				m_gameCaptions.setCaption(m_score_caption, S);

	
			if ( (m_game->artefactBearerID==0) && (m_game->artefactID!=0) )
				{
//					StaticMsg.Out				(0.0f, -0.85f, "Grab the Artefact");
//					StaticMsg.Update			();
					m_gameCaptions.setCaption(m_todo_caption, "Grab the Artefact");
				}
				else
				{
					if (m_game->teamInPossession != pCurActor->g_Team())
					{
//						WarningMsg.Out				(0.f,-0.85f,"Stop ArtefactBearer.");
//						WarningMsg.Update			();
					m_gameCaptions.setCaption(m_todo_caption, "Stop ArtefactBearer");
					}
					else
					{
//						WarningMsg2.SetTextColor		(0xff00ff00);
						if (pCurActor->ID() == m_game->artefactBearerID)
						{
//							WarningMsg2.Out			(0.f,-0.85f,"You got the Artefact. Bring it to your base.");
//							WarningMsg2.Update		();
							m_gameCaptions.setCaption(m_todo_caption, "You got the Artefact. Bring it to your base.",0xff00ff00,true);
						}
						else
						{
//							WarningMsg2.Out			(0.f,-0.85f,"Protect your ArtefactBearer.");
//							WarningMsg2.Update		();
							m_gameCaptions.setCaption(m_todo_caption, "Protect your ArtefactBearer",0xff00ff00,true);
						};
					};
				};
			};
		}break;
	case GAME_PHASE_TEAM1_SCORES:
		{
			HUD().GetUI()->HideIndicators();

//			StaticMsg.SetTextColor		(0xfff0fff0);
//			StaticMsg.Out				(0.f,0.0f,"Team Green WINS!");
			m_gameCaptions.setCaption(m_round_result_caption, "Team Green WINS!");
		}break;
	case GAME_PHASE_TEAM2_SCORES:
		{
			HUD().GetUI()->HideIndicators();

//			StaticMsg.SetTextColor		(0xfff0fff0);
//			StaticMsg.Out				(0.f,0.0f,"Team Blue WINS!");
			m_gameCaptions.setCaption(m_round_result_caption, "Team Blue WINS!");
		}break;
	default:
		{
			
		}break;
	};
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
