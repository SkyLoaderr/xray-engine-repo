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

	// Start client and server if need it
	if (op_server)
	{
		pApp->LoadTitle			("SERVER: Starting...");

		strcpy					(m_caServerOptions,op_server);
		strcpy					(m_caClientOptions,op_client);
		
		// Connect
		Server					= xr_new<xrServer>();

		if (!strstr(m_caServerOptions,"/alife")) {
			string64			l_name;
			strcpy				(l_name,m_caServerOptions);
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
		
		strcpy					(m_caServerOptions,op_server);
	}

	//make Client Name if options doesn't have it
	if (!strstr(m_caClientOptions,"/name="))
	{
		char Text[MAX_COMPUTERNAME_LENGTH + 1]; 
		u32 size = MAX_COMPUTERNAME_LENGTH + 1;
		GetComputerName(Text, (LPDWORD)&size);
		if ((xr_strlen(m_caClientOptions)+xr_strlen(Text)+xr_strlen("/name= ")) < 256)
		{
			strcat(m_caClientOptions, "/name=");
			strcat(m_caClientOptions, Text);
		};
	};
	// Start client
	bResult						= net_Start_client(m_caClientOptions);
	// Send Ready message to server
	if (bResult && Game().type != GAME_SINGLE) 
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
