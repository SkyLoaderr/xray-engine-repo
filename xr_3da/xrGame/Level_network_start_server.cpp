#include "stdafx.h"

BOOL CLevel::net_Server		( LPCSTR name_of_level	)
{
	pApp->LoadBegin	();

	// Startup server
	pApp->LoadTitle	("SERVER: Starting...");
	Server			= new xrServer;
	Server->Connect (name_of_level);

	// Startup client
	BOOL bResult	= net_Client("localhost");

	// End of process
	pApp->LoadEnd	(); 
	return bResult;
}
