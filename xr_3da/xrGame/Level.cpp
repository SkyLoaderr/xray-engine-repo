// Level.cpp: implementation of the CLevel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Level.h"
#include "entity.h"

// events
#include "..\fdemorecord.h"
#include "..\fdemoplay.h"
#include "..\xr_ioconsole.h"
#include "..\fstaticrender.h"
#include "..\xr_level_controller.h"
#include "HUDmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLevel::CLevel()
{
	Server						= NULL;

	eChangeRP					= Engine.Event.Handler_Attach	("LEVEL:ChangeRP",this);
	eDemoPlay					= Engine.Event.Handler_Attach	("LEVEL:PlayDEMO",this);
	eChangeTrack				= Engine.Event.Handler_Attach	("LEVEL:PlayMusic",this);
	eDoorOpen					= Engine.Event.Handler_Attach	("LEVEL:DoorOpen",this);
	eDoorClose					= Engine.Event.Handler_Attach	("LEVEL:DoorClose",this);
	eDA_PlayMotion				= Engine.Event.Handler_Attach	("LEVEL:DA_PlayMotion",this);
	eDA_StopMotion				= Engine.Event.Handler_Attach	("LEVEL:DA_StopMotion",this);
	eEnvironment				= Engine.Event.Handler_Attach	("LEVEL:Environment",this);

	eEntitySpawn				= Engine.Event.Handler_Attach	("LEVEL:spawn",this);
}

CLevel::~CLevel()
{
	Msg			("- Destroying level");

	Engine.Event.Handler_Detach(eEntitySpawn,	this);

	Engine.Event.Handler_Detach(eEnvironment,	this);
	Engine.Event.Handler_Detach(eDA_StopMotion,	this);
	Engine.Event.Handler_Detach(eDA_PlayMotion,	this);
	Engine.Event.Handler_Detach(eDoorClose,		this);
	Engine.Event.Handler_Detach(eDoorOpen,		this);
	Engine.Event.Handler_Detach(eChangeTrack,	this);
	Engine.Event.Handler_Detach(eDemoPlay,		this);
	Engine.Event.Handler_Detach(eChangeRP,		this);
}

// Game interface ////////////////////////////////////////////////////
void CLevel::g_cl_Spawn		(LPCSTR name, int rp, int team, int squad, int group)
{
	NET_Packet	P;
	P.w_begin	(M_CL_SPAWN);
	P.w_string	(name);
	P.w_u8		(team);
	P.w_u8		(squad);
	P.w_u8		(group);
	P.w_u8		((rp>=0)?u8(rp):0xff);

	Send		(P,net_flags(TRUE));
}

void CLevel::g_sv_Spawn		(NET_Packet* Packet)
{
	// Begin analysis
	NET_Packet&	P = *Packet;
	u16			type;
	P.r_begin	(type);
	R_ASSERT	(type==M_SV_SPAWN);

	// Read definition
	char		s_name[128],s_replace[128];
	DWORD		s_team,s_squad,s_group,s_server_id;
	BOOL		s_local;
	Fvector4	o_pos;
	P.r_string	(s_name);
	P.r_string	(s_replace);
	s_team		= P.r_u8	();
	s_squad		= P.r_u8	();
	s_group		= P.r_u8	();
	s_server_id	= P.r_u8	();
	s_local		= P.r_u8	();
	P.r			(&o_pos,3*sizeof(float));
	o_pos.w		= P.r_angle8();
	
	// Real spawn
	CEntity* E = (CEntity*) Objects.LoadOne(pSettings,s_name);
	if (0==E || (!E->Spawn(s_local,s_server_id,s_team,s_squad,s_group,o_pos))) 
	{
		Objects.DestroyObject(E);
		Msg("! Failed to spawn ai entity '%s'",s_name);
	} else {
		if (s_replace[0])	E->cNameSET(s_replace);
		if (s_local	&& (0==CurrentEntity()))	SetEntity(E);
		CSquad& S			= get_squad	(s_team,s_squad);
		if (S.Leader==0)	S.Leader=E;
		else				get_group(s_team,s_squad,s_group).Members.push_back(E);
	}
}

// Loading ///////////////////////////////////////////////////////////
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
		g_cl_Spawn	("actor", -1, 0, 0, 0);

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
				case M_SV_SPAWN:			g_sv_Spawn(P);		break;
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
				if			(M_SV_SPAWN==m_type) g_sv_Spawn(P);
			}
			Sleep	(1);
		}

		//
#pragma todo("spawning monsters")
		char		Parsed	[128];
		{
			int id		= Level().get_RPID	("RP_04");
			R_ASSERT	(id>=0);
			sprintf		(Parsed,"m_enemy,%d,1,0,0",id);
			Engine.Event.Signal	("LEVEL:spawn",DWORD(LPCSTR(Parsed)));
		}
		{
			int id		= Level().get_RPID	("RP_F01");
			R_ASSERT	(id>=0);
			sprintf		(Parsed,"m_friend,%d,0,0,0",id);
			Engine.Event.Signal	("LEVEL:spawn",DWORD(LPCSTR(Parsed)));
		}
		{
			int id		= Level().get_RPID	("RP_F02");
			R_ASSERT	(id>=0);
			sprintf		(Parsed,"m_friend,%d,0,0,0",id);
			Engine.Event.Signal	("LEVEL:spawn",DWORD(LPCSTR(Parsed)));
		}
		{
			int id		= Level().get_RPID	("RP_F03");
			R_ASSERT	(id>=0);
			sprintf		(Parsed,"m_friend,%d,0,0,0",id);
			Engine.Event.Signal	("LEVEL:spawn",DWORD(LPCSTR(Parsed)));
		}

		// Textures
		pApp->LoadTitle						("Loading textures...");
		Device.Shader.DeferredLoad			(FALSE);
		Device.Shader.TexturesLoad			();
		Msg	("* %d K used for textures",	Device.Shader._GetMemoryUsage()/1024);

		// Sync
		pApp->LoadTitle						("CLIENT: Syncronising...");
		while (!isCompleted_Sync())			Sleep(5);

		// signal OK
		return TRUE;
	}
	Device.Shader.DeferredLoad	(FALSE);
	Device.Shader.TexturesLoad	();

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

BOOL CLevel::Load_GameSpecific_Before()
{
	// Load RPoints
	CInifile::Sect& S = pLevel->ReadSection("respawn_point");
	for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) {
		Fvector4	pos;
		int			team;
		const char*	sVal = I->second;
		sscanf(sVal,"%f,%f,%f,%d,%f",&pos.x,&pos.y,&pos.z,&team,&pos.w); pos.y += 0.1f;
		Level().get_team(team).RespawnPoints.push_back(pos);
	}
	return TRUE;
}
int	CLevel::get_RPID(LPCSTR name)
{
	// Gain access to string
	LPCSTR	params = pLevel->ReadSTRING("respawn_point",name);
	if (0==params)	return -1;

	// Read data
	Fvector4	pos;
	int			team;
	sscanf		(params,"%f,%f,%f,%d,%f",&pos.x,&pos.y,&pos.z,&team,&pos.w); pos.y += 0.1f;

	// Search respawn point
	svector<Fvector4,maxRP>	&rp = Level().get_team(team).RespawnPoints;
	for (int i=0; i<int(rp.size()); i++)
		if (pos.similar(rp[i],EPS_L))	return i;
	return -1;
}

BOOL CLevel::Load_GameSpecific_After()
{
	return TRUE;
}

void CLevel::ClientReceive()
{
	for (NET_Packet* P = Retreive(); P; P=Retreive())
	{
		u16			m_type;
		DWORD		cl_time	= P->r_begin(m_type);
		switch (m_type)
		{
		case M_SV_SPAWN:
			g_sv_Spawn(P);
			break;
		case M_UPDATE:
			{
//				DWORD sv_time = P->r_u32();	// server time
				// timeServer_Correct(sv_time,cl_time);
				Objects.net_Import(P);
			}
			break;
		case M_FIRE_HIT:
			{
				DWORD		ID	= P->r_u8			();
				CObject*	O	= Objects.net_Find	(ID);
				if (O)	{
					DWORD		ID2 = P->r_u8		();
					CEntity*	WHO = (CEntity*)Objects.net_Find(ID2);
					DWORD		perc= P->r_u8		();
					Fvector		dir = P->r_dir		();

					CEntity*	E	= (CEntity*)O;
					E->Hit		(perc,dir,WHO);
				}
			}
			break;
		case M_FIRE_BEGIN:
			{
				DWORD		ID	= P->r_u8			();
				CObject*	O	= Objects.net_Find	(ID);
				if (O) {
					CEntity*	E	= (CEntity*)O;
					E->g_sv_fireStart	(P);
				}
			}
			break;
		case M_FIRE_END:
			{
				DWORD		ID	= P->r_u8			();
				CObject*	O	= Objects.net_Find	(ID);
				if (O) {
					CEntity*	E	= (CEntity*)O;
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

void CLevel::OnFrame	()
{
	// Client-message parsing
	Device.Statistic.netClient.Begin();
	ClientReceive		();
	Device.Statistic.netClient.End	();

//	if (!isCompleted_Sync())	return;

	// Inherited update
	inherited::OnFrame	();

	// Merge visibility data from all units in the team
	for (DWORD T=0; T<Teams.size(); T++)
	{
		CTeam&	TD		= Teams[T];
		objVisible& VIS	= TD.KnownEnemys;

		VIS.clear		();
		for (DWORD S=0; S<TD.Squads.size(); S++)
		{
			CSquad&	SD	= TD.Squads[S];
			if (SD.Leader)	SD.Leader->GetVisible(VIS);

			for (DWORD G=0; G<SD.Groups.size(); G++)
			{
				CGroup& GD = SD.Groups[G];
				for (DWORD M=0; M<GD.Members.size(); M++)
					GD.Members[M]->GetVisible(VIS);
			}
		}
	}

	// If we have enought bandwidth - replicate client data on to server
	Device.Statistic.netClient.Begin();
	ClientSend ();
	Device.Statistic.netClient.End	();

	// If server - perform server-update
	if (Server && CurrentEntity())
	{
		Device.Statistic.netServer.Begin();
		Server->Update	();
		Device.Statistic.netServer.End	();
		if (psDeviceFlags&rsStatistic)
		{
			const IServerStatistic* S = Server->GetStatistic();
			pApp->pFont->Size	(0.015f);
			pApp->pFont->OutSet	(0.0f,0.5f);
			pApp->pFont->Color	(D3DCOLOR_XRGB(0,255,0));
			pApp->pFont->OutNext("IN:  %4d/%4d (%2.1f%%)",	S->bytes_in_real,	S->bytes_in,	100.f*float(S->bytes_in_real)/float(S->bytes_in));
			pApp->pFont->OutNext("OUT: %4d/%4d (%2.1f%%)",	S->bytes_out_real,	S->bytes_out,	100.f*float(S->bytes_out_real)/float(S->bytes_out));
			pApp->pFont->OutNext("client_2_sever ping: %d",	net_Statistic.getPing());
			
			pApp->pFont->Color	(D3DCOLOR_XRGB(255,255,255));
			for (DWORD I=0; I<Server->client_Count(); I++)
			{
				IClient*	C = Server->client_Get(I);
				pApp->pFont->OutNext("%10s: P(%d), BPS(%2.1fK), MRR(%2d), MSR(%2d)",
					C->Name,
					C->stats.getPing(),
					float(C->stats.getBPS())/1024,
					C->stats.getMPS_Receive(),
					C->stats.getMPS_Send()
					);
			}
		}
	} else {
		if (psDeviceFlags&rsStatistic)
		{
			pApp->pFont->Size	(0.015f);
			pApp->pFont->OutSet	(0.0f,0.5f);
			pApp->pFont->Color	(D3DCOLOR_XRGB(0,255,0));
			pApp->pFont->OutNext("client_2_sever ping: %d",	net_Statistic.getPing());
		}
	}

	/*
	AI::Path path;
	AI.q_Path(93,1417,path);
	*/
}

void CLevel::OnRender()
{
	inherited::OnRender	();
	Tracers.Render		();
}

void CLevel::OnEvent(EVENT E, DWORD P1, DWORD P2)
{
	if (E==eEntitySpawn)	{
		char	Name[128];	Name[0]=0;
		int		rp=0,team=0,squad=0,group=0;

		sscanf		(LPCSTR(P1),"%[^,],%d,%d,%d,%d", Name, &rp, &team, &squad, &group);
		g_cl_Spawn	(Name,rp,team,squad,group);

	} else if (E==eChangeRP && P1) {
	} else if (E==eDemoPlay && P1) {
		char* name = (char*)P1;
		char RealName [256];
		strcpy(RealName,name);
		strcat(RealName,".xrdemo");
		Cameras.AddEffector(new CDemoPlay(RealName,1.3f));
	} else if (E==eChangeTrack && P1) {
		int id = atoi((char*)P1);
		Environment.Music_Play(id);
	} else if (E==eEnvironment) {
		int id=0; float s=1;
		sscanf((char*)P1,"%d,%f",&id,&s);
		Environment.set_EnvMode(id,s);
	} else return;
}

// Обработка нажатия клавиш
void CLevel::OnKeyboardPress(int key)
{
	if (pHUD->IsUIActive())			if (pHUD->GetUI()->OnKeyboardPress(key)) return;

	switch (key) {
	case DIK_GRAVE:
		Console.Show				();
		return;
	case DIK_RETURN:
		bDebug	= !bDebug;
		return;
	case DIK_F12:
		Render.MakeScreenshot		();
		return;
	case DIK_LALT:
		pHUD->UIActivate			();
		return;
		/*
	case DIK_RALT:
		pCreator->ChangeViewEntity	(CLSID_AI_HUMAN);
		return;
		*/
	}

	switch (key_binding[key]) {
	case kQUIT:	
		{
			Console.Execute			("quit");
			break;
		}
	default:
		if (CurrentEntity())		CurrentEntity()->OnKeyboardPress(key_binding[key]);
		break;
	}
}

void CLevel::OnKeyboardRelease(int key)
{
	if (pHUD->IsUIActive()) if (pHUD->GetUI()->OnKeyboardRelease(key)) return;

	/*
	switch (key)
	{
	case DIK_RALT:	pCreator->ChangeViewEntity(CLSID_OBJECT_ACTOR);		return;
	}
	*/
	if (CurrentEntity())	CurrentEntity()->OnKeyboardRelease(key_binding[key]);
}

void CLevel::OnKeyboardHold(int key)
{
	if (CurrentEntity())	CurrentEntity()->OnKeyboardHold(key_binding[key]);
}

void CLevel::OnMousePress(int btn)
{
	static int m[]={MOUSE_1,MOUSE_2,MOUSE_3};
	OnKeyboardPress(m[btn]);
}

void CLevel::OnMouseRelease(int btn)
{
	static int m[]={MOUSE_1,MOUSE_2,MOUSE_3};
	OnKeyboardRelease(m[btn]);
}

void CLevel::OnMouseHold(int btn)
{
	static int m[]={MOUSE_1,MOUSE_2,MOUSE_3};
	OnKeyboardHold(m[btn]);
}

void CLevel::OnMouseMove( int dx, int dy )
{
	if (pHUD->IsUIActive()) if (pHUD->GetUI()->OnMouseMove(dx,dy)) return;

	if (CurrentEntity())	CurrentEntity()->OnMouseMove(dx,dy);
}

void CLevel::OnMouseStop( int axis, int value)
{
}

