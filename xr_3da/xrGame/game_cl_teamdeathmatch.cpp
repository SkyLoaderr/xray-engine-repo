#include "stdafx.h"
#include "game_cl_teamdeathmatch.h"
#include "xrMessages.h"
#include "hudmanager.h"
#include "level.h"

void game_cl_TeamDeathmatch::TranslateGameMessage	(u32 msg, NET_Packet& P)
{
	string512 Text;
	LPSTR	Color_Teams[3]	= {"%c255,255,255", "%c64,255,64", "%c64,64,255"};
	char	Color_Main[]	= "%c192,192,192";
	LPSTR	TeamsNames[3]	= {"Zero Team", "Team Green", "Team Blue"};

	switch(msg)	{
	case GMSG_PLAYER_JOIN_TEAM: //tdm
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
			HUD().GetUI()->UIMainIngameWnd.AddGameMessage(NULL, Text);
		}break;

	case GMSG_PLAYER_SWITCH_TEAM://tdm
		{
			u16 PlayerID, OldTeam, NewTeam;
			P.r_u16 (PlayerID);
			P.r_u16 (OldTeam);
			P.r_u16 (NewTeam);

			game_cl_GameState::Player* pPlayer = GetPlayerByGameID(PlayerID);
			if (!pPlayer) break;

			sprintf(Text, "%s%s %shas switched to %s%s", 
							Color_Teams[OldTeam], 
							pPlayer->name, 
							Color_Main, 
							Color_Teams[NewTeam], 
							TeamsNames[NewTeam]);
			HUD().GetUI()->UIMainIngameWnd.AddGameMessage(NULL, Text);
		}break;

	default:
		inherited::TranslateGameMessage(msg,P);
	};
}
