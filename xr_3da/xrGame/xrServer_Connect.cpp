#include "stdafx.h"

#include "xrserver.h"
#include "game_sv_single.h"
#include "game_sv_deathmatch.h"
#include "game_sv_teamdeathmatch.h"
#include "game_sv_artefacthunt.h"
#include "level.h"
#include "xrMessages.h"


BOOL xrServer::Connect(ref_str &session_name)
{
	Msg						("! sv_Connect: %s",	*session_name);

	// Parse options and create game
	if (0==strchr(*session_name,'/'))
		return				FALSE;

	string64				options;
	strcpy					(options,strchr(*session_name,'/')+1);
	
	// Parse game type
	string64				type;
	strcpy					(type,options);
	if (strchr(type,'/'))	*strchr(type,'/') = 0;
	game					= NULL;

	CLASS_ID clsid			= game_GameState::getCLASS_ID(type,true);
	game					= smart_cast<game_sv_GameState*> ( NEW_INSTANCE ( clsid ) );


	// Options
	if (0==game)			return FALSE;
	csPlayers.Enter			();
//	game->type				= type_id;
	Msg("Created server_game %s",game->type_name());
	game->Create			(session_name);
	csPlayers.Leave			();
	
	return					IPureServer::Connect(*session_name);
}


void xrServer::new_client			(ClientID clientID, LPCSTR name, bool bLocal)
{
	NET_Packet P;
	P.B.count = 0;
	P.w_stringZ(name);
	P.w_u8(bLocal);
	P.r_pos = 0;

	game->AddDelayedEvent(P,GAME_EVENT_CREATE_CLIENT,0,clientID);
	if(client_Count()==0)
		Update();
}

void xrServer::AttachNewClient			(IClient* CL)
{
	csPlayers.Enter			();
	net_Players.push_back	(CL);
	csPlayers.Leave			();
	
	// config client
	SendTo_LL				(CL->ID,&msgConfig,sizeof(msgConfig));

	// gen message
	OnCL_Connected			(CL);
}
