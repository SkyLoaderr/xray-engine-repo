// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:14:04 , by user : Oles , from computer : OLES
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
#include "..\xr_level_controller.h"
#include "HUDmanager.h"
#include "WayPointDef.h"
#include "PHdynamicdata.h"
#include "Physics.h"
#include "AI\\ai_monsters_misc.h"

CPHWorld*	ph_world = 0;

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

	/**/
	for (int i=0; i<tpaPatrolPaths.size(); i++) {
		tpaPatrolPaths[i].tpaWayPoints.clear();
		tpaPatrolPaths[i].tpaWayLinks.clear();
	}
	//tpaPatrolPaths.clear();
	/**/
	if (ph_world)	ph_world->Destroy		();
	_DELETE			(ph_world);
}

// Game interface ////////////////////////////////////////////////////
void CLevel::g_cl_Spawn		(LPCSTR name, int rp, int team, int squad, int group)
{
	Fvector		dummyPos,dummyAngle; dummyPos.set(0,0,0); dummyAngle.set(0,0,0);
	NET_Packet	P;
	P.w_begin	(M_SPAWN);
	P.w_string	(name);
	P.w_string	("");
	P.w_u8		((rp>=0)?u8(rp):0xff);
	P.w_vec3	(dummyPos);
	P.w_vec3	(dummyAngle);
	P.w_u16		(0);		// srv-id	| by server
	P.w_u16		(M_SPAWN_OBJECT_ACTIVE  | M_SPAWN_OBJECT_LOCAL);
	P.w_u16		(3);		// data size
	P.w_u8		(team);
	P.w_u8		(squad);
	P.w_u8		(group);
	Send		(P,net_flags(TRUE));
}

void CLevel::g_sv_Spawn		(NET_Packet* Packet)
{
	// Begin analysis
	NET_Packet&	P = *Packet;
	u16			type;
	P.r_begin	(type);
	R_ASSERT	(type==M_SPAWN);

	// Read definition
	char		s_name[128],s_replace[128];
	u8			s_rp;
	u16			s_server_id,s_data_size,s_flags;
	Fvector		o_pos,o_angle;
	P.r_string	(s_name);
	P.r_string	(s_replace);
	P.r_u8		(s_rp);
	P.r_vec3	(o_pos);
	P.r_vec3	(o_angle);
	P.r_u16		(s_server_id);
	P.r_u16		(s_flags);
	P.r_u16		(s_data_size);

	// Real spawn
	CObject*	O = Objects.LoadOne	(s_name);
	if (O)	
	{
		O->cName_set		(s_name);
		O->cNameSect_set	(s_name);
		if (s_replace[0])	O->cName_set		(s_replace);
	}
	if (0==O || (!O->Spawn(s_flags&M_SPAWN_OBJECT_LOCAL, s_server_id, o_pos, o_angle, P, s_flags))) 
	{
		Objects.DestroyObject(O);
		Msg("! Failed to spawn entity '%s'",s_name);
	} else {
		if ((s_flags&M_SPAWN_OBJECT_LOCAL) && (0==CurrentEntity()))	SetEntity		(O);
		if (s_flags&M_SPAWN_OBJECT_ACTIVE)							O->OnActivate	( );
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
		if (FALSE && Engine.FS.Exist(fn_spawn, Path.Current, "level.spawn"))
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

#include "testphysics.h"
BOOL CLevel::Load_GameSpecific_Before()
{
	// AI space
	pApp->LoadTitle("Loading AI space...");
	AI.Load(Path.Current);
	
	// Load RPoints
	CInifile::Sect& S = pLevel->ReadSection("respawn_point");
	for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) {
		Fvector4	pos;
		int			team;
		const char*	sVal = I->second;
		sscanf(sVal,"%f,%f,%f,%d,%f",&pos.x,&pos.y,&pos.z,&team,&pos.w); pos.y += 0.1f;
		Level().get_team(team).RespawnPoints.push_back(pos);
	}
	
	tpaPatrolPaths.clear();
	FILE_NAME	fn_game;
	if (Engine.FS.Exist(fn_game, Path.Current, "level.game")) {
		CStream *F = Engine.FS.Open(fn_game);
		CStream *O = F->OpenChunk(WAY_PATH_CHUNK);
		if (O) {
			int chunk = 0;
			for (CStream *OBJ = O->OpenChunk(chunk++); OBJ; OBJ = O->OpenChunk(chunk++)) {
				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_VERSION));
				DWORD dw = OBJ->Rword();
				R_ASSERT(dw == WAYOBJECT_VERSION);

				tpaPatrolPaths.resize(tpaPatrolPaths.size() + 1);
				SPatrolPath &tPatrolPath = tpaPatrolPaths[tpaPatrolPaths.size() - 1];
				
				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_NAME));
				OBJ->RstringZ(tPatrolPath.sName);
				
				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_TYPE));
				tPatrolPath.dwType = OBJ->Rdword();
				
				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_POINTS));
				DWORD dwCount = OBJ->Rword();
				tPatrolPath.tpaWayPoints.resize(dwCount);
				for (int i=0; i<dwCount; i++){
					OBJ->Rvector(tPatrolPath.tpaWayPoints[i].tWayPoint);
					tPatrolPath.tpaWayPoints[i].dwFlags = OBJ->Rdword();
					if (tPatrolPath.tpaWayPoints[i].dwFlags & START_WAYPOINT)
						tPatrolPath.dwStartNode = Level().AI.q_LoadSearch(tPatrolPath.tpaWayPoints[i].tWayPoint);
				}
				
				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_LINKS));
				DWORD dwCountL = OBJ->Rword();
				tPatrolPath.tpaWayLinks.resize(dwCountL);
				for ( i=0; i<dwCountL; i++){
					tPatrolPath.tpaWayLinks[i].wFrom = OBJ->Rword();
					tPatrolPath.tpaWayLinks[i].wTo = OBJ->Rword();
				}

				OBJ->Close();
				
				// sorting links
				bool bOk;
				do {
					bOk = true;
					for ( i=1; i<dwCountL; i++)
						if ((tPatrolPath.tpaWayLinks[i - 1].wFrom > tPatrolPath.tpaWayLinks[i].wFrom) || ((tPatrolPath.tpaWayLinks[i - 1].wFrom == tPatrolPath.tpaWayLinks[i].wFrom) && (tPatrolPath.tpaWayLinks[i - 1].wTo > tPatrolPath.tpaWayLinks[i].wTo))) {
							WORD wTemp = tPatrolPath.tpaWayLinks[i - 1].wFrom;
							tPatrolPath.tpaWayLinks[i - 1].wFrom = tPatrolPath.tpaWayLinks[i].wFrom;
							tPatrolPath.tpaWayLinks[i].wFrom = wTemp;
							wTemp = tPatrolPath.tpaWayLinks[i - 1].wTo;
							tPatrolPath.tpaWayLinks[i - 1].wTo = tPatrolPath.tpaWayLinks[i].wTo;
							tPatrolPath.tpaWayLinks[i].wTo = wTemp;
							bOk = false;
						}
				}
				while (!bOk);
			}
			O->Close();
		}
		Engine.FS.Close(F);
	}

	vfSetStartSeed(Level().timeServer());

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
		u16			ID;
		DWORD		cl_time	= P->r_begin(m_type);
		switch (m_type)
		{
		case M_SPAWN:
			g_sv_Spawn(P);
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

void CLevel::OnFrame	()
{
	// Client-message parsing
	Device.Statistic.netClient.Begin();
	ClientReceive		();
	Device.Statistic.netClient.End	();

	// Inherited update
	inherited::OnFrame	();

	// Physics
	Device.Statistic.Physics.Begin		();
	if (ph_world) ph_world->Step		(Device.fTimeDelta);
	Device.Statistic.Physics.End		();

	// Merge visibility data from all units in the team
	for (DWORD T=0; T<Teams.size(); T++)
	{
		CTeam&	TD		= Teams[T];
		objVisible& VIS	= TD.KnownEnemys;

		VIS.clear		();
		for (DWORD S=0; S<TD.Squads.size(); S++)
		{
			CSquad&	SD	= TD.Squads[S];
			if (SD.Leader)	{
				CEntityAlive* E	= dynamic_cast<CEntityAlive*>(SD.Leader);
				if (E)		E->GetVisible(VIS);
			}

			for (DWORD G=0; G<SD.Groups.size(); G++)
			{
				CGroup& GD = SD.Groups[G];
				for (DWORD M=0; M<GD.Members.size(); M++)
				{
					CEntityAlive* E	= dynamic_cast<CEntityAlive*>(GD.Members[M]);
					if (E)	E->GetVisible(VIS);
				}
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
}

void CLevel::OnRender()
{
	inherited::OnRender	();
	Tracers.Render		();
	eff_Rain.Render		();
/*
	ph_world->Render	();
	if (bDebug)			{
		ObjectSpace.dbgRender	();
		AI.Render				();
	}
*/
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
ENGINE_API extern BOOL	ShowLM;
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
		Render->Screenshot			();
		return;
	case DIK_LALT:
		pHUD->UIActivate			();
		return;
	case DIK_RALT:
		ShowLM	= TRUE;
		return;
	case DIK_BACK:
		HW.Caps.bShowOverdraw		= !HW.Caps.bShowOverdraw;
		return;
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

	switch (key)
	{
	case DIK_RALT:
		ShowLM	= FALSE;
		return;
	}
	if (CurrentEntity())	CurrentEntity()->OnKeyboardRelease(key_binding[key]);
}

void CLevel::OnKeyboardHold(int key)
{
	if (CurrentEntity())	CurrentEntity()->OnKeyboardHold(key_binding[key]);
}

static int mouse_button_2_key []={MOUSE_1,MOUSE_2,MOUSE_3};
void CLevel::OnMousePress(int btn)
{	OnKeyboardPress(mouse_button_2_key[btn]);}
void CLevel::OnMouseRelease(int btn)
{	OnKeyboardRelease(mouse_button_2_key[btn]);}
void CLevel::OnMouseHold(int btn)
{	OnKeyboardHold(mouse_button_2_key[btn]);}

void CLevel::OnMouseMove( int dx, int dy )
{
	if (pHUD->IsUIActive()) if (pHUD->GetUI()->OnMouseMove(dx,dy)) return;

	if (CurrentEntity())	CurrentEntity()->OnMouseMove(dx,dy);
}

void CLevel::OnMouseStop( int axis, int value)
{
}

