#include "stdafx.h"

BOOL CLevel::net_Start	( LPCSTR op_server, LPCSTR op_client )
{
	BOOL bResult				= FALSE;

	pApp->LoadBegin				();

	// Start client and server if need it
	if (op_server)
	{
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
		Server						= xr_new<xrServer>();
		Server->Connect				(op_server);

		// Analyze game and perform apropriate SERVER spawning
		Server->SLS_Default			();
	}

	// Start client
	bResult							= net_Start_client(op_client);

	pApp->LoadEnd					();

	return bResult;
}
#include "stdafx.h"
#include "HUDmanager.h"
#include "PHdynamicdata.h"
#include "Physics.h"

BOOL CLevel::net_Start_client	( LPCSTR options )
{
	pApp->LoadBegin	();

	// name_of_server
	string64					name_of_server;
	strcpy						(name_of_server,options);
	if (strchr(name_of_server,'/'))	*strchr(name_of_server,'/') = 0;

	// Startup client
	string128					temp;
	sprintf						(temp,"CLIENT: Connecting to '%s'...",name_of_server);
	pApp->LoadTitle				(temp);

	// HUD
	Device.Shader.DeferredLoad	(TRUE);

	if (Connect(options)) 
	{
		// Determine internal level-ID
		LPCSTR	level_name	= net_SessionName	();
		int		level_id	= pApp->Level_ID(level_name);
		if	(level_id<0)	{
			Disconnect		();
			pApp->LoadEnd	();
			return FALSE;
		}
		pApp->Level_Set		(level_id);

		// Load level
		R_ASSERT(Load						(level_id));
		ph_world							= xr_new<CPHWorld>();
		ph_world->Create					();

		// Waiting for connection/configuration completition
		pApp->LoadTitle						("CLIENT: Spawning...");
		while (!net_isCompleted_Connect())	Sleep(5);
		while (!net_isCompleted_Sync())		{ ClientReceive(); Sleep(5); }
		while (!game_configured)			{ ClientReceive(); Sleep(5); }

		// HUD
		pHUD->Load							();

		// Textures
		pApp->LoadTitle						("Loading textures...");
		Device.Shader.DeferredLoad			(FALSE);
		Device.Shader.DeferredUpload		();
		Msg	("* %d K used for textures",	Device.Shader._GetMemoryUsage()/1024);

		// Sync
		pApp->LoadTitle						("CLIENT: Syncronising...");
		Device.PreCache						(50);

		if (strstr(Path.Current,"escape"))	Engine.Event.Signal	("level.weather.rain.start");

		// signal OK
		pApp->LoadEnd	();
		return TRUE;
	}
	Device.Shader.DeferredLoad	(FALSE);
	Device.Shader.DeferredUpload();

	pApp->LoadEnd				(); 
	return FALSE;
}
