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
		R_ASSERT2(Load						(level_id),"Loading failed.");
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

		u32	m_base,c_base,m_lmaps,c_lmaps;
		Device.Shader._GetMemoryUsage		(m_base,c_base,m_lmaps,c_lmaps);

		Msg	("* t-report - base: %d, %d K",	c_base,		m_base/1024);
		Msg	("* t-report - lmap: %d, %d K",	c_lmaps,	m_lmaps/1024);
		if (m_base>64*1024*1024)
		{
			LPCSTR msg	= "Too much textures. Reduce number of textures (better) or their resolution (worse).";
			Msg			("***FATAL***: ",msg);
			Debug.fatal	(msg);
		}

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
