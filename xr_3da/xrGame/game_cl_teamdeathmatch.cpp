#include "stdafx.h"
#include "game_cl_teamdeathmatch.h"
#include "xrMessages.h"
#include "hudmanager.h"
#include "level.h"
#include "UIGameTDM.h"

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
			CommonMessageOut(Text);
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
			CommonMessageOut(Text);
		}break;

	default:
		inherited::TranslateGameMessage(msg,P);
	};
}

CUIGameCustom* game_cl_TeamDeathmatch::createGameUI()
{
	CLASS_ID clsid			= CLSID_GAME_UI_TEAMDEATHMATCH;
	CUIGameTDM*			pUIGame	= dynamic_cast<CUIGameTDM*> ( NEW_INSTANCE ( clsid ) );
	R_ASSERT(pUIGame);
	pUIGame->SetClGame(this);
	pUIGame->Init();
	return pUIGame;
}


void game_cl_TeamDeathmatch::GetMapEntities(xr_vector<SZoneMapEntityData>& dst)
{
	SZoneMapEntityData D;
	u32 color_self_team		=		0xff00ff00;
	D.color					=		color_self_team;

	s16 local_team			=		local_player->team;

	xr_map<u32,Player>::iterator it = players.begin();
	for(;it!=players.end();++it){
		if(local_team == it->second.team){
			u16 id = it->second.GameID;
			CObject* pObject = Level().Objects.net_Find(id);
			if (!pObject) continue;

			VERIFY(pObject);
			D.pos = pObject->Position();
			dst.push_back(D);
		}

	}
}
