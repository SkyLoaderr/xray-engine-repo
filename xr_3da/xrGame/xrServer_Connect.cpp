#include "stdafx.h"

#include "xrserver.h"
#include "game_sv_single.h"
#include "game_sv_deathmatch.h"
#include "game_sv_teamdeathmatch.h"
#include "game_sv_artefacthunt.h"
#include "game_sv_cs.h"

xr_token					game_type_token						[ ]={
	{ "single",				GAME_SINGLE								},
	{ "deathmatch",			GAME_DEATHMATCH							},
	{ "ctf",				GAME_CTF								},
	{ "assault",			GAME_ASSAULT							},
	{ "cs",					GAME_CS									},
	{ "teamdeathmatch",		GAME_TEAMDEATHMATCH						},
	{ "artefacthunt",		GAME_ARTEFACTHUNT						},
	{ "new_dm",				GAME_NEW_DM						},
	{ 0,							0								}
};

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
	u32 type_id				= _ParseItem(type,game_type_token);
	switch (type_id)
	{
	case GAME_SINGLE:	

//		game				= dynamic_cast<game_sv_Single*> ( NEW_INSTANCE (CLSID_GAME_SINGLE) );
		game				= xr_new<game_sv_Single> (this);
		break;
	case GAME_DEATHMATCH:
		game				= dynamic_cast<game_sv_Deathmatch*> ( NEW_INSTANCE (CLSID_GAME_DEATHMATCH) );
//		game				= xr_new<game_sv_Deathmatch> ();
		break;
	case GAME_TEAMDEATHMATCH:
		game				= dynamic_cast<game_sv_TeamDeathmatch*> ( NEW_INSTANCE (CLSID_GAME_TEAMDEATHMATCH) );
//		game				= xr_new<game_sv_TeamDeathmatch> ();
		break;
	case GAME_ARTEFACTHUNT:
		game				= dynamic_cast<game_sv_ArtefactHunt*> ( NEW_INSTANCE (CLSID_GAME_ARTEFACTHUNT) );
//		game				= xr_new<game_sv_ArtefactHunt> ();
		break;
	case GAME_CS:
		game				= dynamic_cast<game_sv_CS*> ( NEW_INSTANCE (CLSID_GAME_CS) );
//		game				= xr_new<game_sv_CS> ();
		break;

	case GAME_NEW_DM:
		game				= dynamic_cast<game_sv_GameState*> ( NEW_INSTANCE (MK_CLSID('G','M','_','N','E','W','D','M') ) );
		//		game				= xr_new<game_sv_CS> ();
		break;

	default:
		R_ASSERT2(0, "Unknown game type!!!");
//		return				FALSE;
		break;
	}




	// Options
	if (0==game)			return FALSE;
	csPlayers.Enter			();
	game->type				= type_id;
	game->Create			(session_name);
	csPlayers.Leave			();
	
	return					IPureServer::Connect(session_name);
}
