#include "stdafx.h"
#include "../resourcemanager.h"

#include "HUDmanager.h"
#include "PHdynamicdata.h"
#include "Physics.h"
#include "level.h"

extern	pureFrame*				g_pNetProcessor;

BOOL CLevel::net_Start_client	( LPCSTR options )
{
	pApp->LoadBegin	();

	// name_of_server
	string64					name_of_server;
	strcpy						(name_of_server,options);
	if (strchr(name_of_server,'/'))	*strchr(name_of_server,'/') = 0;

	// Startup client
	string256					temp;
	sprintf						(temp,"CLIENT: Connecting to '%s'...",name_of_server);
	pApp->LoadTitle				(temp);

///	if (Connect(options)) 
	if (Connect2Server(options))
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

		// Send physics to single or multithreaded mode
		ph_world							= xr_new<CPHWorld>();
		ph_world->Create					();

		// Send network to single or multithreaded mode
		// *note: release version always has "mt_*" enabled
		Device.seqFrameMT.Remove			(g_pNetProcessor);
		Device.seqFrame.Remove				(g_pNetProcessor);
		if (psDeviceFlags.test(mtNetwork))	Device.seqFrameMT.Add	(g_pNetProcessor,REG_PRIORITY_HIGH	+ 2);
		else								Device.seqFrame.Add		(g_pNetProcessor,REG_PRIORITY_LOW	- 2);

		// Waiting for connection/configuration completition
		pApp->LoadTitle						("CLIENT: Spawning...");
		while (!net_isCompleted_Connect())	Sleep(5);
		while (!net_isCompleted_Sync())		{ ClientReceive(); Sleep(5); }
		while (!game_configured)			{ ClientReceive(); Sleep(5); }
		// HUD
		pHUD->Load							();
		
		// Textures
		if	(!g_pGamePersistent->bDedicatedServer)
		{
			pApp->LoadTitle						("Loading textures...");
			Device.Resources->DeferredLoad		(FALSE);
			Device.Resources->DeferredUpload	();
			LL_CheckTextures					();
		}

		// Sync
		pApp->LoadTitle						("CLIENT: Synchronising...");
		Device.PreCache						(30);

		// if (strstr(Path.Current,"escape"))	Engine.Event.Signal	("level.weather.rain.start");

		//-----------------------------------------------------------------------
		if (!pStatGraph && OnServer())
		{
			/*
			pStatGraph = xr_new<CStatGraph>();
			pStatGraph->SetRect(0, 0, 1024, 100, 0xff000000, 0xff000000);

			pStatGraph->SetMinMax(-0.0f, 100.0f, 300);
			pStatGraph->SetGrid(0, 0.0f, 10, 10.0f, 0xff808080, 0xffffffff);

			pStatGraph->SetStyle(CStatGraph::stBar);
			pStatGraph->AppendSubGraph(CStatGraph::stBar);
			pStatGraph->AppendSubGraph(CStatGraph::stBar);
			*/
		};
		//-----------------------------------------------------------------------

		// signal OK
		pApp->LoadEnd	();
		return TRUE;
	}

	pApp->LoadEnd				(); 
	return FALSE;
}
