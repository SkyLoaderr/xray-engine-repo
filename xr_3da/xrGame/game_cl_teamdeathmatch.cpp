#include "stdafx.h"
#include "game_cl_teamdeathmatch.h"
#include "xrMessages.h"
#include "hudmanager.h"
#include "level.h"
#include "UIGameTDM.h"
#include "xr_level_controller.h"

#define	TEAM1_MENU		"teamdeathmatch_team1"
#define	TEAM2_MENU		"teamdeathmatch_team2"

game_cl_TeamDeathmatch::game_cl_TeamDeathmatch()
{
	pUITeamSelectWnd	= xr_new<CUISpawnWnd>	();
	pBuyMenuTeam1		= NULL;
	pBuyMenuTeam2		= NULL;

	PresetItemsTeam1.clear();
	PresetItemsTeam2.clear();

	pSkinMenuTeam1		= NULL;
	pSkinMenuTeam2		= NULL;

	m_bTeamSelected		= TRUE;
	m_game_ui			= NULL;


}
void game_cl_TeamDeathmatch::Init ()
{
	//-----------------------------------------------------------
	string64	Team1, Team2;
	std::strcpy(Team1, TEAM1_MENU);
	std::strcpy(Team2, TEAM2_MENU);
	m_aTeamSections.push_back(Team1);
	m_aTeamSections.push_back(Team2);

}

game_cl_TeamDeathmatch::~game_cl_TeamDeathmatch()
{
	xr_delete(pUITeamSelectWnd);

	xr_delete(pBuyMenuTeam1);
	xr_delete(pBuyMenuTeam2);

	xr_delete(pSkinMenuTeam1);
	xr_delete(pSkinMenuTeam2);
}

void game_cl_TeamDeathmatch::TranslateGameMessage	(u32 msg, NET_Packet& P)
{
	string512 Text;
	LPSTR	Color_Teams[3]	= {"%c255,255,255", "%c64,255,64", "%c64,64,255"};
	char	Color_Main[]	= "%c192,192,192";
	LPSTR	TeamsNames[3]	= {"Zero Team", "Team Green", "Team Blue"};

	switch(msg)	{
	case GAME_EVENT_PLAYER_JOIN_TEAM: //tdm
		{
			string64 PlayerName;
			P.r_stringZ	(PlayerName);
			u16 Team;
			P.r_u16		(Team);

			sprintf(Text, "%s%s %sjoined %s%s",
							Color_Teams[0],
							PlayerName,
							Color_Main,
							Color_Teams[Team],
							TeamsNames[Team]);
			CommonMessageOut(Text);
		}break;

	case GAME_EVENT_PLAYER_CHANGE_TEAM://tdm
		{
			u16 PlayerID, OldTeam, NewTeam;
			P.r_u16 (PlayerID);
			P.r_u16 (OldTeam);
			P.r_u16 (NewTeam);

			game_PlayerState* pPlayer = GetPlayerByGameID(PlayerID);
			if (!pPlayer) break;

			sprintf(Text, "%s%s %shas switched to %s%s", 
							Color_Teams[OldTeam], 
							pPlayer->name, 
							Color_Main, 
							Color_Teams[NewTeam], 
							TeamsNames[NewTeam]);
			CommonMessageOut(Text);
		}break;

	default:
		inherited::TranslateGameMessage(msg,P);
	};
}

CUIGameCustom* game_cl_TeamDeathmatch::createGameUI()
{
	CLASS_ID clsid			= CLSID_GAME_UI_TEAMDEATHMATCH;
	CUIGameTDM*			pUIGame	= smart_cast<CUIGameTDM*> ( NEW_INSTANCE ( clsid ) );
	R_ASSERT(pUIGame);
	pUIGame->SetClGame(this);
	pUIGame->Init();

	//-----------------------------------------------------------
	pBuyMenuTeam1 = InitBuyMenu("teamdeathmatch_base_cost", 1);
	pBuyMenuTeam2 = InitBuyMenu("teamdeathmatch_base_cost", 2);
	//-----------------------------------------------------------
	pSkinMenuTeam1 = InitSkinMenu(1);
	pSkinMenuTeam2 = InitSkinMenu(2);

	pInventoryMenu = xr_new<CUIInventoryWnd>();

	return pUIGame;
}


void game_cl_TeamDeathmatch::GetMapEntities(xr_vector<SZoneMapEntityData>& dst)
{
	SZoneMapEntityData D;
	u32 color_self_team		=		0xff00ff00;
	D.color					=		color_self_team;

	s16 local_team			=		local_player->team;

	PLAYERS_MAP_IT it = players.begin();
	for(;it!=players.end();++it){
		if(local_team == it->second->team){
			u16 id = it->second->GameID;
			CObject* pObject = Level().Objects.net_Find(id);
			if (!pObject) continue;

			VERIFY(pObject);
			D.pos = pObject->Position();
			dst.push_back(D);
		}

	}
}

void game_cl_TeamDeathmatch::OnTeamSelect(int Team)
{
	if (Team+1 != local_player->team) 
	{
		CObject *l_pObj = Level().CurrentEntity();

		CGameObject *l_pPlayer = smart_cast<CGameObject*>(l_pObj);
		if(!l_pPlayer) return;

		NET_Packet		P;
		l_pPlayer->u_EventGen		(P,GE_GAME_EVENT,l_pPlayer->ID()	);
		P.w_u16(GAME_EVENT_PLAYER_CHANGE_TEAM);
		
		P.w_s16			(s16(Team+1));
		//P.w_u32			(0);
		l_pPlayer->u_EventSend		(P);
		//-----------------------------------------------------------------
		m_bSkinSelected = FALSE;
	};
	//-----------------------------------------------------------------
	m_bTeamSelected = TRUE;	
};
//-----------------------------------------------------------------
void game_cl_TeamDeathmatch::SetCurrentBuyMenu	()
{
	if (!pCurBuyMenu || !pCurBuyMenu->IsShown())
	{
		if (local_player->team == 1) 
		{
			pCurBuyMenu = pBuyMenuTeam1;
			pCurPresetItems = &PresetItemsTeam1;
		}
		else 
		{
			pCurBuyMenu = pBuyMenuTeam2;
			pCurPresetItems = &PresetItemsTeam2;
		};
	};

//	if(!pCurSkinMenu)SetCurrentSkinMenu	();
	if (pCurBuyMenu) pCurBuyMenu->SetSkin(local_player->skin);
	if (!pCurBuyMenu) return;

	if (!local_player) return;

};

void game_cl_TeamDeathmatch::SetCurrentSkinMenu	()
{
	CUISkinSelectorWnd* pNewSkinMenu;
	if (local_player->team == 1) pNewSkinMenu = pSkinMenuTeam1;
	else pNewSkinMenu = pSkinMenuTeam2;

	if (pNewSkinMenu != pCurSkinMenu)
		if (pCurSkinMenu && pCurSkinMenu->IsShown()) StartStopMenu(pCurSkinMenu,true);

	pCurSkinMenu = pNewSkinMenu;
	pCurSkinMenu->SwitchSkin(local_player->skin);
};

bool game_cl_TeamDeathmatch::CanBeReady				()
{
	if (!m_bTeamSelected)
	{
		StartStopMenu(pUITeamSelectWnd,true);
		return false;
	}

	return inherited::CanBeReady();
};

char*	game_cl_TeamDeathmatch::getTeamSection(int Team)
{
	switch (Team)
	{
	case 1:
		{
			return "teamdeathmatch_team1";
		}break;
	case 2:
		{
			return "teamdeathmatch_team2";
		}break;
	default:
		return NULL;
	};
};

void game_cl_TeamDeathmatch::shedule_Update			(u32 dt)
{
	if(!m_game_ui && HUD().GetUI() ) m_game_ui = smart_cast<CUIGameTDM*>( HUD().GetUI()->UIGame() );
	inherited::shedule_Update(dt);
}

bool	game_cl_TeamDeathmatch::OnKeyboardPress			(int key)
{
	if (kTEAM == key )
	{
		StartStopMenu(pUITeamSelectWnd,true);

		return true;
	};

	
	return inherited::OnKeyboardPress(key);
}



