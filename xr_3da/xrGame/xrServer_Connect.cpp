#include "stdafx.h"
#include "game_sv_single.h"
#include "..\xr_trims.h"
#include "..\xr_tokens.h"

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
	BOOL bResult = IPureServer::Connect(session_name);
	if (bResult)
	{
		// Parse options and create game
		if (0==strchr(session_name,'|'))	return FALSE;
		string256				options;
		strcpy					(options,strchr(session_name,'|')+1);
		
		// Parse game type
		if (0==strchr(options,'/'))			return FALSE;
		string64				type;
		strcpy					(type,strchr(options,'/')+1);
		game								= NULL;
		switch (_ParseItem(type,game_type_token))
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
	}
}
