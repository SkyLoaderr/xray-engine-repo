#include "stdafx.h"

void xrServer::Perform_game_export	()
{
	// Broadcase game state to every body
	// But it is slightly different view for each "player"

	NET_Packet		P;
	u32			mode			= net_flags(TRUE,TRUE);

	// Game config (all, info includes new player)
	csPlayers.Enter		();
	for (u32 client=0; client<net_Players.size(); client++)
	{
		u32 ID						= net_Players[client]->ID;
		P.w_begin						(M_SV_CONFIG_GAME);
		game->net_Export_State			(P,ID);
		SendTo							(ID,P,mode);
	}
	csPlayers.Leave		();
}
