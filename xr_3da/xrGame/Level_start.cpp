#include "stdafx.h"

BOOL CLevel::net_Start	( LPCSTR op_server, LPCSTR op_client )
{
	BOOL bResult				= FALSE;

	pApp->LoadBegin				();

	// Start client and server if need it
	if (op_server)
	{
		pApp->LoadTitle				("");						
		pApp->LoadTitle				("SERVER: Starting...");

		// Activate level
		string64					l_name;
		strcpy						(l_name,op_server);
		if (strchr(l_name,'/'))		*strchr(l_name,'/') = 0;
		int id						= pApp->Level_ID(l_name);
		if (id<0)	
		{
			pApp->LoadEnd			();
			return FALSE;
		}
		pApp->Level_Set				(id);

		// Connect
		Server						= new xrServer;
		Server->Connect				(op_server);

		// Analyze game and perform apropriate SERVER spawning
		Server->SLS_Default			();
	}

	// Start client
	bResult							= net_Start_client(op_client);

	pApp->LoadEnd					();

	return bResult;
}
