#include "stdafx.h"
#include "game_sv_single.h"
#include "alife_simulator.h"

void xrServer::SLS_Default	()
{
	// Spawn all other objects
	if ((game->type == GAME_SINGLE) && ai().get_alife())
		return;
	
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

			++S_id;
		}
		FS.r_close			(SP);
	}
}
