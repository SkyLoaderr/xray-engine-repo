#include "stdafx.h"

BOOL CLevel::net_Server			( LPCSTR name, BOOL SLS_Load )
{
	pApp->LoadBegin				();

	// Startup server
	BOOL bResult				= FALSE;
	if (SLS_Load)
	{
		pApp->LoadTitle				("SERVER: Starting [LOAD]...");
		string256					name_of_level;

		destructor<CStream>			FS(Engine.FS.Open(name));
		R_ASSERT					(FS().FindChunk(fsSLS_Description));
		FS().RstringZ				(name_of_level);

		// Connect
		Server						= new xrServer;
		Server->Connect				(name_of_level);

		// Load SRV data
		CStream* F					= FS().OpenChunk(fsSLS_ServerState);
		Server->SLS_Load			(*F);
		F->Close					();

		// Startup client
		bResult						= net_Client("localhost");

		// Load client-specific data
		F							= FS().OpenChunk(fsSLS_UnitsSpecific);
		Objects.SLS_Load			(*F);
		F->Close					();
	} else {
		pApp->LoadTitle				("SERVER: Starting...");

		// Connect
		Server						= new xrServer;
		Server->Connect				(name);

		// Default 
		Server->SLS_Default			();

		// Startup client
		bResult						= net_Client("localhost");
	}

	// End of process
	pApp->LoadEnd				(); 

	return bResult;
}
