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

		// Load level
		R_ASSERT(Load				(level_id));
		pHUD->Load					();

		// Waiting for connection completition
		pApp->LoadTitle				("CLIENT: Spawning...");
		while (!net_isCompleted_Connect()) Sleep(5);

		// Signal main actor spawn
		LPCSTR		s_cmd			= Engine.Params;
		string64	s_name			= "actor";
		if (strstr(s_cmd,"-actor "))	{
			sscanf(strstr(s_cmd,"-actor ")+strlen("-actor "),"%s",s_name);
			ph_world			= new CPHWorld;
			ph_world->Create	();
		}
		g_cl_Spawn	(s_name, -1, 0, 0, 0);

		// Spawn all other objects
		FILE_NAME	fn_spawn;
		if (Engine.FS.Exist(fn_spawn, Path.Current, "level.spawn"))
		{
			CStream*		SP	= Engine.FS.Open(fn_spawn);
			NET_Packet		P;
			CStream*		S		= 0;
			int				S_id	= 0;
			while (0!=(S = SP->OpenChunk(S_id)))
			{
				P.B.count	=	S->Length();
				S->Read		(P.B.data,P.B.count);
				S->Close	();
				Send		(P,net_flags(TRUE));
				S_id		+=	1;
			}
			Engine.FS.Close	(SP);
		}

		// And receiving spawn information (game-state)
		BOOL bFinished		= FALSE;
		while (!bFinished) 
		{
			for (NET_Packet* P = net_Retreive(); P; P=net_Retreive())
			{
				u16			m_type;	
				P->r_begin	(m_type);
				switch (m_type)
				{
				case M_SV_CONFIG_FINISHED:	bFinished = TRUE;	break;
				case M_SPAWN:				g_sv_Spawn(P);		break;
				}
			}
			Sleep	(1);
		}

		// Waiting for actor spawn
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
