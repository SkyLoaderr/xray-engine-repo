#include "stdafx.h"
#include "..\xr_trims.h"
#include "..\xr_tokens.h"

#include "game_sv_single.h"
#include "game_sv_deathmatch.h"
#include "game_sv_cs.h"

xr_token					game_type_token						[ ]={
	{ "single",				GAME_SINGLE								},
	{ "deathmatch",			GAME_DEATHMATCH							},
	{ "ctf",				GAME_CTF								},
	{ "assault",			GAME_ASSAULT							},
	{ "cs",					GAME_CS									},
	{ 0,							0								}
};

BOOL xrServer::Connect(LPCSTR session_name)
{
	Msg						("! sv_Connect: %s",	session_name);

	BOOL bResult = IPureServer::Connect(session_name);
	if (bResult)
	{
		// Parse options and create game
		if (0==strchr(session_name,'/'))	return FALSE;
		string256				options;
		strcpy					(options,strchr(session_name,'/')+1);
		
		// Parse game type
		string64				type;
		strcpy					(type,options);
		if (strchr(type,'/'))	*strchr(type,'/') = 0;
		game					= NULL;
		u32 type_id				= _ParseItem(type,game_type_token);
		switch (type_id)
		{
		case GAME_SINGLE:	
			game				= new game_sv_Single();
			break;
		case GAME_DEATHMATCH:
			game				= new game_sv_Deathmatch();
			break;
		case GAME_CS:
			game				= new game_sv_CS();
			break;
		default:
			return				FALSE;
			break;
		}

		// Options
		if (0==game)				return FALSE;
		game->type					= type_id;
		game->Create				(options);
	}
	return	bResult;
}
