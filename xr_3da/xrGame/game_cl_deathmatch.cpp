#include "stdafx.h"
#include "game_cl_deathmatch.h"
#include "xrMessages.h"
#include "hudmanager.h"
#include "level.h"
#include "UIGameDM.h"

void game_cl_Deathmatch::TranslateGameMessage	(u32 msg, NET_Packet& P)
{
	string512 Text;
	char	Color_Weapon[]	= "%c255,0,0";
	LPSTR	Color_Teams[3]	= {"%c255,255,255", "%c64,255,64", "%c64,64,255"};
	char	Color_Main[]	= "%c192,192,192";
	switch(msg)	{
	
	case GMSG_PLAYER_KILLED: //dm
		{
			u16 PlayerID, KillerID, WeaponID;
			P.r_u16 (PlayerID);
			P.r_u16 (KillerID);
			P.r_u16 (WeaponID);

			game_cl_GameState::Player* pPlayer = GetPlayerByGameID(PlayerID);
			game_cl_GameState::Player* pKiller = GetPlayerByGameID(KillerID);
			CObject* pWeapon = Level().Objects.net_Find(WeaponID);

			if (!pPlayer || !pKiller) break;

			if (pWeapon && WeaponID != 0)
				sprintf(Text, "%s%s %skilled from %s%s %sby %s%s", 
								Color_Teams[pPlayer->team], 
								pPlayer->name, 
								Color_Main,
								Color_Weapon,
								*(pWeapon->cName()), 
								Color_Main,
								Color_Teams[pKiller->team], 
								pKiller->name);
			else
				sprintf(Text, "%s%s %skilled by %s%s",
								Color_Teams[pPlayer->team],
								pPlayer->name, 
								Color_Main,
								Color_Teams[pKiller->team],
								pKiller->name);

			CommonMessageOut(Text);
		}break;
	default:
		inherited::TranslateGameMessage(msg,P);
	}
}

CUIGameCustom* game_cl_Deathmatch::createGameUI()
{
	CLASS_ID clsid			= CLSID_GAME_UI_DEATHMATCH;
	CUIGameDM*			pUIGame	= dynamic_cast<CUIGameDM*> ( NEW_INSTANCE ( clsid ) );
	R_ASSERT(pUIGame);
	pUIGame->SetClGame(this);
	pUIGame->Init();
	return pUIGame;
}

void game_cl_Deathmatch::net_import_state	(NET_Packet& P)
{
	inherited::net_import_state	(P);

	P.r_s32			(fraglimit);
	P.r_s32			(timelimit);
	// Teams
	// Teams
	u16				t_count;
	P.r_u16			(t_count);
	teams.clear	();
	for (u16 t_it=0; t_it<t_count; ++t_it)
	{
		game_TeamState	ts;
		P.r				(&ts,sizeof(game_TeamState));
		teams.push_back	(ts);
	}
	
}
