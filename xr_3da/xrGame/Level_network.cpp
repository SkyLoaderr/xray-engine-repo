#include "stdafx.h"
#include "HUDmanager.h"
#include "entity.h"
#include "PHdynamicdata.h"
#include "Physics.h"

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

BOOL CLevel::net_Client		( LPCSTR name_of_server )
{
	pApp->LoadBegin	();

	// Startup client
	char			temp[256];
	sprintf			(temp,"CLIENT: Connecting to '%s'...",name_of_server);
	pApp->LoadTitle	(temp);

	// HUD
	Device.Shader.DeferredLoad	(TRUE);
	pHUD->Load		();

	if (Connect(name_of_server)) 
	{
		// Determine internal level-ID
		LPCSTR	level_name	= SessionName	();
		int		level_id	= pApp->Level_ID(level_name);
		if	(level_id<0)	{
			Disconnect		();
			pApp->LoadEnd	();
			return FALSE;
		}

		// Load level
		R_ASSERT(Load				(level_id));

		// Waiting for connection completition
		pApp->LoadTitle				("CLIENT: Spawning...");
		while (!isCompleted_Connect()) Sleep(5);

		// Signal main actor spawn
		LPCSTR		s_cmd			= GetCommandLine();
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
		BOOL bFinished	= FALSE;
		while (!bFinished) 
		{
			for (NET_Packet* P = Retreive(); P; P=Retreive())
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
			for (NET_Packet* P = Retreive(); P; P=Retreive())
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
		while (!isCompleted_Sync())			Sleep(5);

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

void CLevel::net_Disconnect	( )
{
	Msg			("- Disconnect");
	Disconnect	();
	if (Server)
	{
		Server->Disconnect	();
		_DELETE				(Server);
	}
}


void CLevel::ClientReceive()
{
	for (NET_Packet* P = Retreive(); P; P=Retreive())
	{
		u16			m_type;
		u16			ID;
		DWORD		cl_time	= P->r_begin(m_type);
		switch (m_type)
		{
		case M_SPAWN:
			g_sv_Spawn(P);
			break;
		case M_DESTROY:
			{
				u16 count;
				P->r_u16				(count);
				for (; count; count--)	
				{
					P->r_u16			(ID);
					CObject* O			= Objects.net_Find(u32(ID));
					CEntity* E			= dynamic_cast<CEntity*> (O);
					if (E)	
					{
						CSquad& S			= Level().get_squad	(E->g_Team(),E->g_Squad());
						CGroup& G			= Level().get_group	(E->g_Team(),E->g_Squad(),E->g_Group());
						if (E==S.Leader)	
						{
							S.Leader		= 0;
							if (!G.Members.empty())	{
								S.Leader	= G.Members.back();
								G.Member_Remove(S.Leader);
							}
						}
						else {
							G.Member_Remove	(E);
						}
					}
					Objects.DestroyObject	(O);
				}
			}
			break;
		case M_UPDATE:
			Objects.net_Import	(P);
			break;
		case M_FIRE_HIT:
			{
				P->r_u16		(ID);
				CObject*	O	= Objects.net_Find	(ID);
				if (O)	{
					u16			ID2;
					P->r_u16	(ID2);
					CEntity*	WHO = dynamic_cast<CEntity*>	(Objects.net_Find(ID2));
					u8			perc;	P->r_u8		(perc);
					Fvector		dir;	P->r_dir	(dir);

					CEntity*	E	= dynamic_cast<CEntity*>	(O);
					E->Hit		(u32(perc),dir,WHO);
				}
			}
			break;
		case M_FIRE_BEGIN:
			{
				P->r_u16		(ID);
				CObject*	O	= Objects.net_Find	(ID);
				if (O) {
					CEntity*	E	= dynamic_cast<CEntity*>(O);
					E->g_sv_fireStart	(P);
				}
			}
			break;
		case M_FIRE_END:
			{
				P->r_u16		(ID);
				CObject*	O	= Objects.net_Find	(ID);
				if (O) {
					CEntity*	E	= dynamic_cast<CEntity*>(O);
					E->g_fireEnd	();
				}
			}
			break;
		case M_CHAT:
			{
				char	buffer[256];
				P->r_string(buffer);
				Msg		("- %s",buffer);
			}
			break;
		}
	}
}

void CLevel::ClientSend	()
{
	if (!HasBandwidth())	return;

	NET_Packet				P;
	P.w_begin				(M_UPDATE);
	Objects.net_Export		(&P);
	if (P.B.count>2)		Send(P,net_flags(FALSE));
}

