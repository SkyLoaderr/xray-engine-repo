#include "stdafx.h"
#include "UIGameAHunt.h"

#include "UIAHuntPlayerList.h"
#include "UIAHuntFragList.h"

#include "hudmanager.h"
#include "team_base_zone.h"

#define MSGS_OFFS 510

#define	TEAM1_MENU		"artefacthunt_team1"
#define	TEAM2_MENU		"artefacthunt_team2"

//--------------------------------------------------------------------
CUIGameAHunt::CUIGameAHunt(CUI* parent):CUIGameTDM(parent)
{
	m_bBuyEnabled = TRUE;
}
//--------------------------------------------------------------------
void		CUIGameAHunt::Init				()
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
	//--------------------------------------------------------------------
	pBuyMenuTeam1 = InitBuyMenu(1);
	pBuyMenuTeam2 = InitBuyMenu(2);
	//-----------------------------------------------------------
	pSkinMenuTeam1 = InitSkinMenu(1);
	pSkinMenuTeam2 = InitSkinMenu(2);
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

	CActor* pCurActor = dynamic_cast<CActor*> (Level().CurrentEntity());
	switch (Game().phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			HUD().GetUI()->ShowIndicators();

			if (m_bBuyEnabled)
			{
				if (pCurActor && pCurActor->g_Alive() && !pCurBuyMenu->IsShown())
				{
					HUD().pFontDI->SetColor		(0xffffff00);
					HUD().pFontDI->Out			(0.f,0.9f,"Press B to access Buy Menu");
				};
			};

			if (pCurActor)
			{
				game_TeamState team0 = Game().teams[0];
				game_TeamState team1 = Game().teams[1];

				HUD().pFontDI->SetColor		(0xffffffff);
				HUD().pFontDI->Out			(0.f,-0.9f,"Your Team has : %3d Artefacts from %3d", 
					Game().teams[pCurActor->g_Team()-1].score, Game().m_ArtefactsNum);
				
				if (Game().m_ArtefactBearerID == 0)
				{
					HUD().pFontDI->SetColor		(0xffffffff);
					HUD().pFontDI->Out			(0.f,-0.85f,"Grab the Artefact");
				}
				else
				{
					if (Game().m_TeamInPosession != pCurActor->g_Team())
					{
						HUD().pFontDI->SetColor		(0xffff0000);
						HUD().pFontDI->Out			(0.f,-0.85f,"Stop ArtefactBearer.");
					}
					else
					{
						HUD().pFontDI->SetColor		(0xff00ff00);
						if (pCurActor->ID() == Game().m_ArtefactBearerID)
						{
							HUD().pFontDI->Out			(0.f,-0.85f,"You got the Artefact. Bring it to your base.");
						}
						else
						{
							HUD().pFontDI->Out			(0.f,-0.85f,"Protect your ArtefactBearer.");
						};
					};
				};
			};
		}break;
	case GAME_PHASE_TEAM1_SCORES:
		{
			HUD().GetUI()->HideIndicators();

			HUD().pFontDI->SetColor		(0xfff0fff0);
			HUD().pFontDI->Out			(0.f,0.0f,"Team Red WINS!");
		}break;
	case GAME_PHASE_TEAM2_SCORES:
		{
			HUD().GetUI()->HideIndicators();

			HUD().pFontDI->SetColor		(0xfff0fff0);
			HUD().pFontDI->Out			(0.f,0.0f,"Team Blue WINS!");
		}break;
	default:
		{
			
		}break;
	};
};
//--------------------------------------------------------------------
void CUIGameAHunt::OnBuyMenu_Ok	()
{
	if (!m_bBuyEnabled) return;
	CObject *l_pObj = Level().CurrentEntity();

	CGameObject *l_pPlayer = dynamic_cast<CGameObject*>(l_pObj);
	if(!l_pPlayer) return;

	NET_Packet		P;
	l_pPlayer->u_EventGen		(P,GEG_PLAYER_BUY_FINISHED,l_pPlayer->ID()	);
//-------------------------------------------------------------------------------
	u8 NumItems = pCurBuyMenu->GetBeltSize();
	for (u8 Slot=KNIFE_SLOT; Slot<APPARATUS_SLOT; Slot++)
	{
		if (pCurBuyMenu->GetWeaponIndex(Slot) != 0xff) NumItems++;
	}

	//-------------------------------------------------------------------------------
	P.w_u8		(NumItems);
	//-------------------------------------------------------------------------------
	for (Slot=KNIFE_SLOT; Slot<APPARATUS_SLOT; Slot++)
	{
		u8 ItemID = pCurBuyMenu->GetWeaponIndex(Slot);
		if (ItemID == 0xff) continue;
		P.w_u8	(Slot);
		P.w_u8	(ItemID);
	};
	for (u8 i=0; i<pCurBuyMenu->GetBeltSize(); i++)
	{
		u8 SectID, ItemID;
		pCurBuyMenu->GetWeaponIndexInBelt(i, SectID, ItemID);
		P.w_u8	(SectID);
		P.w_u8	(ItemID);
	};	
	//-------------------------------------------------------------------------------
	/*
	P.w_u8		(pCurBuyMenu->GetWeaponIndex(KNIFE_SLOT));
	P.w_u8		(pCurBuyMenu->GetWeaponIndex(PISTOL_SLOT));
	P.w_u8		(pCurBuyMenu->GetWeaponIndex(RIFLE_SLOT));
	P.w_u8		(pCurBuyMenu->GetWeaponIndex(GRENADE_SLOT));

	P.w_u8		(pCurBuyMenu->GetBeltSize());

	for (u8 i=0; i<pCurBuyMenu->GetBeltSize(); i++)
	{
		u8 SectID, ItemID;
		pCurBuyMenu->GetWeaponIndexInBelt(i, SectID, ItemID);
		P.w_u8	(SectID);
		P.w_u8	(ItemID);
	};	
	*/
	l_pPlayer->u_EventSend		(P);
};

BOOL		CUIGameAHunt::CanCallBuyMenu			()
{
	BOOL res = inherited::CanCallBuyMenu();
	if (!res) return FALSE;

	CActor* pCurActor = dynamic_cast<CActor*> (Level().CurrentEntity());
	if (!pCurActor || !pCurActor->g_Alive()) return FALSE;

	return TRUE;
};