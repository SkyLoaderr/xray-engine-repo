#include "stdafx.h"
#include "level.h"
#include "LevelFogOfWar.h"
#include "Level_Bullet_Manager.h"
#include "xrserver.h"
#include "game_cl_base.h"
#include "xrmessages.h"

BOOL CLevel::net_Start	( LPCSTR op_server, LPCSTR op_client )
{
	BOOL bResult				= FALSE;

	pApp->LoadBegin				();

	//make Client Name if options doesn't have it
	if (!strstr(op_client,"/name="))
	{
		string512 tmp;
		strcpy(tmp, op_client);
		strcat(tmp, "/name=");
		strcat(tmp, Core.CompName);
		m_caClientOptions			= tmp;
	} else {
		m_caClientOptions			= op_client;
	};
	m_caServerOptions			    = op_server;

	// Start client and server if need it
	if (op_server)
	{
		pApp->LoadTitle			("SERVER: Starting...");

		// Connect
		Server					= xr_new<xrServer>();

		if (!strstr(*m_caServerOptions,"/alife")) {
			string64			l_name;
			strcpy				(l_name,*m_caServerOptions);
			// Activate level
			if (strchr(l_name,'/'))
				*strchr(l_name,'/')	= 0;

			int					id = pApp->Level_ID(l_name);

			if (id<0) {
				pApp->LoadEnd	();
				Log				("Can't find level: ",l_name);
				return			FALSE;
			}
			pApp->Level_Set		(id);
		}
		
		Server->Connect			(m_caServerOptions);	
		Server->SLS_Default		();
		
		m_caServerOptions		= op_server;
	}

	// Start client
	bResult						= net_Start_client(*m_caClientOptions);
	// Send Ready message to server
	if (bResult && GameID() != GAME_SINGLE) 
	{
		NET_Packet		NP;
		NP.w_begin		(M_CLIENTREADY);
		Send(NP,net_flags(TRUE,TRUE));
	};

	//init the fog of war for the current level
	FogOfWar().Init();
	
	//init bullet manager
	BulletManager().Clear		();
	BulletManager().Load		();

	pApp->LoadEnd				();

	return						bResult;
}

void CLevel::InitializeClientGame	(NET_Packet& P)
{
	string256 game_type_name;
	P.r_stringZ(game_type_name);
	if(game && !xr_strcmp(game_type_name, game->type_name()) )
		return;
	
	xr_delete(game);
	CLASS_ID clsid			= game_GameState::getCLASS_ID(game_type_name,false);
	game					= dynamic_cast<game_cl_GameState*> ( NEW_INSTANCE ( clsid ) );
	game->set_type_name(game_type_name);
}

