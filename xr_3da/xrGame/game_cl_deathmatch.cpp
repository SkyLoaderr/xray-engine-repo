#include "stdafx.h"
#include "game_cl_deathmatch.h"
#include "xrMessages.h"
#include "hudmanager.h"
#include "level.h"

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

			HUD().GetUI()->UIMainIngameWnd.AddGameMessage(NULL, Text);
		}break;
	default:
		inherited::TranslateGameMessage(msg,P);
	}
}