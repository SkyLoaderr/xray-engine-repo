#include "stdafx.h"

BOOL CLevel::net_Start	( LPCSTR op_server, LPCSTR op_client )
{
	BOOL bResult				= FALSE;

	pApp->LoadBegin				();

	// Start client and server if need it
	if (op_server)
	{
		pApp->LoadTitle			("SERVER: Starting...");

		LPSTR					l_name = (LPSTR)xr_malloc(1024*sizeof(char));
		strcpy					(l_name,op_server);
		
		// Connect
		Server					= xr_new<xrServer>();
		Server->Connect			(l_name);

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
		
		xr_free					(l_name);
	}

	// Start client
	bResult						= net_Start_client(op_client);

	pApp->LoadEnd				();

	return						bResult;
}
