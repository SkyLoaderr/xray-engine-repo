#include "stdafx.h"
#include "game_sv_single.h"
#include "ai_alife.h"

void xrServer::SLS_Default	()
{
	// Spawn all other objects
	if (game->type == GAME_SINGLE) {
		game_sv_Single *tpGame	= dynamic_cast<game_sv_Single*>(game);
		if (tpGame && tpGame->m_tpALife)
			return;
	}
	
	string256				fn_spawn;
	if (FS.exist(fn_spawn, "$level$", "level.spawn")) {
		IReader*			SP		= FS.r_open(fn_spawn);
		IReader*			S		= 0;
		int					S_id	= 0;
		NET_Packet			P;
		while (0 != (S = SP->open_chunk(S_id))) {
			P.B.count		= S->length();
			S->r			(P.B.data,P.B.count);
			S->close		();

			u16				ID;
			P.r_begin		(ID);
			R_ASSERT		(M_SPAWN==ID);
			Process_spawn	(P,0);

			S_id++;
		}
		FS.r_close			(SP);
	}
}
