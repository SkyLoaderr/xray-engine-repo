#include "stdafx.h"
#include "HUDmanager.h"
#include "PHdynamicdata.h"
#include "Physics.h"

BOOL CLevel::net_Client		( LPCSTR name_of_server )
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
		R_ASSERT(Load				(level_id));
		pHUD->Load					();

		// Waiting for connection completition
		pApp->LoadTitle				("CLIENT: Spawning...");
		while (!net_isCompleted_Connect()) Sleep(5);

		// Spawn actor if non-local server
		if (0!=stricmp(name_of_server,"localhost"))
			g_cl_Spawn("actor",0,0,0,0);

		// And receiving spawn information (game-state)
		BOOL bFinished		= FALSE;
		while (!bFinished) 
		{
			for (NET_Packet* P = net_msg_Retreive(); P; P=net_msg_Retreive())
			{
				u16			m_type;	
				P->r_begin	(m_type);
				switch (m_type)
				{
				case M_SV_CONFIG_FINISHED:	bFinished = TRUE;	break;
				case M_SPAWN:				g_sv_Spawn(P);		break;
				}
				net_msg_Release	();
			}
			Sleep	(1);
		}

		// Waiting for actor spawn
		/*
		while (0==CurrentEntity()) 
		{
			for (NET_Packet* P = net_Retreive(); P; P=net_Retreive())
			{
				u16			m_type;	
				P->r_begin	(m_type);
				if			(M_SPAWN==m_type) g_sv_Spawn(P);
			}
			Sleep	(1);
		}
		*/

		// Textures
		pApp->LoadTitle						("Loading textures...");
		Device.Shader.DeferredLoad			(FALSE);
		Device.Shader.DeferredUpload		();
		Msg	("* %d K used for textures",	Device.Shader._GetMemoryUsage()/1024);

		// Sync
		pApp->LoadTitle						("CLIENT: Syncronising...");
		while (!net_isCompleted_Sync())		Sleep(5);

		if (strstr(Path.Current,"escape"))	Engine.Event.Signal	("level.weather.rain.start");

		// signal OK
		pApp->LoadEnd	();
		return TRUE;
	}
	Device.Shader.DeferredLoad	(FALSE);
	Device.Shader.DeferredUpload();

	pApp->LoadEnd	(); 
	return FALSE;
}
