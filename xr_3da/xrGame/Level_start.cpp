#include "stdafx.h"

BOOL CLevel::net_Start	( LPCSTR name, BOOL server )
{
	BOOL bResult				= FALSE;

	// Force server creation for single-player
	switch (GAME)
	{
	case GAME_SINGLE:
		server		= TRUE;
		break;
	default:
		break;
	}

	// Start client and server if need it
	if (server)
	{
		pApp->LoadTitle				("SERVER: Starting...");

		// Activate level
		int id						= pApp->Level_ID(name);
		if (id<0)	
		{
			pApp->LoadEnd			();
			return FALSE;
		}
		pApp->Level_Set				(id);

		// Connect
		Server						= new xrServer;
		Server->Connect				(name);

		// Analyze game and perform apropriate SERVER spawning

		// Start client
		bResult						= net_Start_client("localhost");
	} else {
		bResult						= net_Start_client(name);
	}

	// Analyze game and perform apropriate CLIENT spawning
	switch (GAME)
	{
	case GAME_SINGLE:
		// Already spawned actor on server
		break;
	case GAME_DEATHMATCH:
		// Spawn at ANY r-point
		{
			g_cl_Spawn("actor",0xFF,0,0,0);
		}
		break;
	case GAME_CTF:
		// Spawn at TEAM r-point
		{
			g_cl_Spawn("actor",0xFD,0,0,0);
		}
		break;
	case GAME_ASSAULT:
		// Spawn at TEAM r-point
		{
			g_cl_Spawn("actor",0xFD,0,0,0);
		}
		break;
	}
	return bResult;
}
