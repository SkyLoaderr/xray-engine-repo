#include "stdafx.h"

#include "xrserver.h"
#include "game_sv_single.h"
#include "game_sv_deathmatch.h"
#include "game_sv_teamdeathmatch.h"
#include "game_sv_artefacthunt.h"
#include "game_sv_cs.h"
#include "level.h"
/*
xr_token					game_type_token						[ ]={
	{ "single",				GAME_SINGLE								},
	{ "deathmatch",			GAME_DEATHMATCH							},
//	{ "ctf",				GAME_CTF								},
//	{ "assault",			GAME_ASSAULT							},
	{ "cs",					GAME_CS									},
	{ "teamdeathmatch",		GAME_TEAMDEATHMATCH						},
	{ "artefacthunt",		GAME_ARTEFACTHUNT						},
//	{ "new_dm",				GAME_NEW_DM								},
	{ 0,							0								}
};
*/
//moved to script game_registrator.get_game_clsid


BOOL xrServer::Connect(LPSTR &session_name)
{
	Msg						("! sv_Connect: %s",	session_name);

	// Parse options and create game
	if (0==strchr(session_name,'/'))
		return				FALSE;

	string64				options;
	strcpy					(options,strchr(session_name,'/')+1);
	
	// Parse game type
	string64				type;
	strcpy					(type,options);
	if (strchr(type,'/'))	*strchr(type,'/') = 0;
	game					= NULL;

	CLASS_ID clsid			= game_GameState::getCLASS_ID(type,true);
	game					= dynamic_cast<game_sv_GameState*> ( NEW_INSTANCE ( clsid ) );

/*
	u32 type_id				= _ParseItem(type,game_type_token);
	switch (type_id)
	{
	case GAME_SINGLE:	
		game				= xr_new<game_sv_Single> (Level().Server);
		break;
	case GAME_DEATHMATCH:
		game				= xr_new<game_sv_Deathmatch> (Level().Server);
		break;
	case GAME_TEAMDEATHMATCH:
		game				= xr_new<game_sv_TeamDeathmatch> (Level().Server);
		break;
	case GAME_ARTEFACTHUNT:
		game				= xr_new<game_sv_ArtefactHunt> (Level().Server);
		break;

	default:
		R_ASSERT2(0, "Unknown game type!!!");
		//		return				FALSE;
		break;
	}

*/



	// Options
	if (0==game)			return FALSE;
	csPlayers.Enter			();
//	game->type				= type_id;
	Msg("Created server_game %s",game->type_name());
	game->Create			(session_name);
	csPlayers.Leave			();
	
	return					IPureServer::Connect(session_name);
}
