#include "stdafx.h"

#include "LevelFogOfWar.h"

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
		Server->Connect			(m_caServerOptions);

		string64				l_name;
		strcpy					(l_name,m_caServerOptions);
		// Activate level
		if (strchr(l_name,'/'))
			*strchr(l_name,'/')	= 0;

		int						id = pApp->Level_ID(l_name);
		
		if (id<0) {
			pApp->LoadEnd		();
			Log					("Can't find level: ",l_name);
			return				FALSE;
		}
		pApp->Level_Set			(id);
		
		Server->SLS_Default		();
		
		strcpy					(m_caServerOptions,op_server);

	}

	// Start client
	bResult						= net_Start_client(op_client);

	//init the fog of war for the current level
	m_pFogOfWar->Init();

	pApp->LoadEnd				();

	return						bResult;
}
