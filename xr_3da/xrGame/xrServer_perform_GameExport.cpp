#include "stdafx.h"

void xrServer::Perform_game_export	()
{
	// Broadcase game state to every body
	// But it is slightly different view for each "player"

	NET_Packet		P;
	DWORD			mode			= net_flags(TRUE,TRUE);

	// Game config (all, info includes new player)
	P.w_begin				(M_SV_CONFIG_GAME);
	game->net_Export_State	(P);
	SendBroadcast			(0xffffffff,P,mode);
}
