#include "stdafx.h"
#include "HUDmanager.h"
#include "PHdynamicdata.h"
#include "Physics.h"

BOOL CLevel::net_Start_client	( LPCSTR name_of_server )
{
	pApp->LoadBegin	();

	// Startup client
	char			temp[256];
	sprintf			(temp,"CLIENT: Connecting to '%s'...",name_of_server);
	pApp->LoadTitle	(temp);

	// HUD
	Device.Shader.DeferredLoad	(TRUE);

	if (Connect(name_of_server)) 
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
		pHUD->Load							();
		ph_world							= new CPHWorld;
		ph_world->Create					();

		// Waiting for connection completition
		pApp->LoadTitle						("CLIENT: Spawning...");
		while (!net_isCompleted_Connect())	Sleep(5);
		while (!net_isCompleted_Sync())		{ ClientReceive(); Sleep(5); }
		ClientReceive						();

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
