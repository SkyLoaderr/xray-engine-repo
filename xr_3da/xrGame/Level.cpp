// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:14:04 , by user : Oles , from computer : OLES
// Level.cpp: implementation of the CLevel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\fdemorecord.h"
#include "..\fdemoplay.h"
#include "..\environment.h"
#include "ParticlesObject.h"

#include "Level.h"
#include "entity.h"
#include "hudmanager.h"
#include "ai_space.h"
#include "ai_console.h"

// events
#include "PHdynamicdata.h"
#include "Physics.h"

CPHWorld*	ph_world = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLevel::CLevel()
{
	Server						= NULL;

	game_configured				= FALSE;

	eChangeRP					= Engine.Event.Handler_Attach	("LEVEL:ChangeRP",this);
	eDemoPlay					= Engine.Event.Handler_Attach	("LEVEL:PlayDEMO",this);
	eChangeTrack				= Engine.Event.Handler_Attach	("LEVEL:PlayMusic",this);
	eEnvironment				= Engine.Event.Handler_Attach	("LEVEL:Environment",this);

	eEntitySpawn				= Engine.Event.Handler_Attach	("LEVEL:spawn",this);

	m_caServerOptions			= (LPSTR)xr_malloc(256*sizeof(char));
	m_caClientOptions			= (LPSTR)xr_malloc(256*sizeof(char));
	m_caServerOptions[0]		= 0;
	m_caClientOptions[0]		= 0;
}

CLevel::~CLevel()
{
	Msg					("- Destroying level");

	Engine.Event.Handler_Detach(eEntitySpawn,	this);

	Engine.Event.Handler_Detach(eEnvironment,	this);
	Engine.Event.Handler_Detach(eChangeTrack,	this);
	Engine.Event.Handler_Detach(eDemoPlay,		this);
	Engine.Event.Handler_Detach(eChangeRP,		this);

	if (ph_world)
	{
		ph_world->Destroy	();
		xr_delete			(ph_world);
	}

	// destroy PSs
	for (POIt p_it=m_StaticParticles.begin(); p_it!=m_StaticParticles.end(); p_it++)
		xr_delete		(*p_it);
	m_StaticParticles.clear();

	// Unload sounds
	for (u32 i=0; i<static_Sounds.size(); i++){
		static_Sounds[i]->destroy();
		xr_delete		(static_Sounds[i]);
	}
	static_Sounds.clear	();

	xr_delete			(tpAI_Space);
	xr_free				(m_caServerOptions);
	xr_free				(m_caClientOptions);
}

// Game interface ////////////////////////////////////////////////////
int	CLevel::get_RPID(LPCSTR name)
{
	/*
	// Gain access to string
	LPCSTR	params = pLevel->r_string("respawn_point",name);
	if (0==params)	return -1;

	// Read data
	Fvector4	pos;
	int			team;
	sscanf		(params,"%f,%f,%f,%d,%f",&pos.x,&pos.y,&pos.z,&team,&pos.w); pos.y += 0.1f;

	// Search respawn point
	svector<Fvector4,maxRP>	&rp = Level().get_team(team).RespawnPoints;
	for (int i=0; i<(int)(rp.size()); i++)
		if (pos.similar(rp[i],EPS_L))	return i;
	*/
	return -1;
}

void CLevel::vfMergeKnownEnemies()
{
	// Merge visibility data from all units in the team
	for (u32 T=0; T<Teams.size(); T++)
	{
		CTeam&	TD		= Teams[T];
		for (u32 S=0; S<TD.Squads.size(); S++)
		{
			CSquad&	SD		= TD.Squads[S];
			objVisible& VIS	= SD.KnownEnemys;

			VIS.clear		();
			for (u32 G=0; G<SD.Groups.size(); G++)
			{
				CGroup& GD = SD.Groups[G];
				for (u32 M=0; M<GD.Members.size(); M++)
				{
					CEntityAlive* E	= dynamic_cast<CEntityAlive*>(GD.Members[M]);
					if (E && E->g_Alive()&& !E->getDestroy())	E->GetVisible(VIS);
				}
			}
		}
	}
}

void CLevel::OnFrame	()
{
	// Client receive
	if (net_isDisconnected())	
	{
		Engine.Event.Defer			("kernel:disconnect");
		return;
	} else {
		Device.Statistic.netClient.Begin();
		ClientReceive					();
		Device.Statistic.netClient.End	();
	}

	// Game events
	{
		NET_Packet			P;
		u32 svT				= timeServer()-NET_Latency;

		/*
		if (!game_events.queue.empty())	
			Msg("- d[%d],ts[%d] -- E[svT=%d],[evT=%d]",Device.dwTimeGlobal,timeServer(),svT,game_events.queue.begin()->timestamp);
		*/

		while	(game_events.available(svT))
		{
			u16 dest,type;
			game_events.get	(dest,type,P);

			// Msg				("--- event[%d] for [%d]",type,dest);
			CObject*	 O	= Objects.net_Find	(dest);
			if (0==O)		{
				Msg("! c_EVENT[%d] : unknown dest",dest);
				continue;
			}
			CGameObject* GO = dynamic_cast<CGameObject*>(O);
			if (!GO)		{
				Msg("! c_EVENT[%d] : non-game-object",dest);
				continue;
			}
			GO->OnEvent		(P,type);
		}
	}

	// Inherited update
	inherited::OnFrame					();

	// Physics
	/*
	if (!psDeviceFlags.test(mtPhysics))
	{
		// Physics in single-threaded mode
		Device.Statistic.Physics.Begin		();
		if (ph_world) ph_world->Step		(Device.fTimeDelta);
		Device.Statistic.Physics.End		();
	}
	*/

	// If we have enought bandwidth - replicate client data on to server
	Device.Statistic.netClient.Begin();
	ClientSend						();
	Device.Statistic.netClient.End	();

	vfMergeKnownEnemies				();

	CGameFont* F = HUD().pFontDI;
	// If server - perform server-update
	if (Server)
	{
		Device.Statistic.netServer.Begin();
		Server->Update					();
		Device.Statistic.netServer.End	();
		if (psDeviceFlags.test(rsStatistic))
		{
			const IServerStatistic* S = Server->GetStatistic();
			F->SetSize	(0.015f);
			F->OutSet	(0.0f,0.5f);
			F->SetColor	(D3DCOLOR_XRGB(0,255,0));
			F->OutNext	("IN:  %4d/%4d (%2.1f%%)",	S->bytes_in_real,	S->bytes_in,	100.f*float(S->bytes_in_real)/float(S->bytes_in));
			F->OutNext	("OUT: %4d/%4d (%2.1f%%)",	S->bytes_out_real,	S->bytes_out,	100.f*float(S->bytes_out_real)/float(S->bytes_out));
			F->OutNext	("client_2_sever ping: %d",	net_Statistic.getPing());
			
			F->SetColor	(D3DCOLOR_XRGB(255,255,255));
			for (u32 I=0; I<Server->client_Count(); I++)
			{
				IClient*	C = Server->client_Get(I);
				F->OutNext("%10s: P(%d), BPS(%2.1fK), MRR(%2d), MSR(%2d)",
					C->Name,
					C->stats.getPing(),
					float(C->stats.getBPS())/1024,
					C->stats.getMPS_Receive(),
					C->stats.getMPS_Send()
					);
			}
		}
	} else {
		if (psDeviceFlags.test(rsStatistic))
		{
			F->SetSize	(0.015f);
			F->OutSet	(0.0f,0.5f);
			F->SetColor	(D3DCOLOR_XRGB(0,255,0));
			F->OutNext("client_2_sever ping: %d",	net_Statistic.getPing());
		}
	}
}

void CLevel::OnRender()
{
	inherited::OnRender	();
	Tracers.Render		();
	eff_Rain.Render		();
//	ph_world->Render	();
	if (bDebug || psAI_Flags.test(aiMotion))
		getAI().Render		();
	if (bDebug)	{
		ObjectSpace.dbgRender	();
	}
}

void CLevel::OnEvent(EVENT E, u64 P1, u64 P2)
{
	if (E==eEntitySpawn)	{
		char	Name[128];	Name[0]=0;
		sscanf	(LPCSTR(P1),"%s", Name);
		Level().g_cl_Spawn	(Name,0xff, M_SPAWN_OBJECT_LOCAL);
	} else if (E==eChangeRP && P1) {
	} else if (E==eDemoPlay && P1) {
		char* name = (char*)P1;
		char RealName [256];
		strcpy(RealName,name);
		strcat(RealName,".xrdemo");
		Cameras.AddEffector(xr_new<CDemoPlay> (RealName,1.3f));
	} else if (E==eChangeTrack && P1) {
		int id = atoi((char*)P1);
		Environment->Music_Play(id);
	} else if (E==eEnvironment) {
		int id=0; float s=1;
		sscanf((char*)P1,"%d,%f",&id,&s);
		Environment->set_EnvMode(id,s);
	} else return;
}

