// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:14:04 , by user : Oles , from computer : OLES
// Level.cpp: implementation of the CLevel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Level.h"
#include "entity.h"
#include "ai_funcs.h"
#include "hudmanager.h"

// events
#include "..\fdemorecord.h"
#include "..\fdemoplay.h"
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
}

CLevel::~CLevel()
{
	Msg			("- Destroying level");

	Engine.Event.Handler_Detach(eEntitySpawn,	this);

	Engine.Event.Handler_Detach(eEnvironment,	this);
	Engine.Event.Handler_Detach(eChangeTrack,	this);
	Engine.Event.Handler_Detach(eDemoPlay,		this);
	Engine.Event.Handler_Detach(eChangeRP,		this);

	if (ph_world)	ph_world->Destroy		();

	_DELETE			(m_tpAI_DDD);
	_DELETE			(ph_world);
}

// Game interface ////////////////////////////////////////////////////
int	CLevel::get_RPID(LPCSTR name)
{
	/*
	// Gain access to string
	LPCSTR	params = pLevel->ReadSTRING("respawn_point",name);
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

void CLevel::OnFrame	()
{
	if (net_isDisconnected())	
	{
		Engine.Event.Defer			("kernel:disconnect");
		return;
	} else {
		Device.Statistic.netClient.Begin();
		ClientReceive					();
		Device.Statistic.netClient.End	();
	}

	// Inherited update
	inherited::OnFrame					();

	// Physics
	Device.Statistic.Physics.Begin		();
	if (ph_world) ph_world->Step		(Device.fTimeDelta);
	Device.Statistic.Physics.End		();

	// Merge visibility data from all units in the team
	for (u32 T=0; T<Teams.size(); T++)
	{
		CTeam&	TD		= Teams[T];
		for (u32 S=0; S<TD.Squads.size(); S++)
		{
			CSquad&	SD	= TD.Squads[S];
			objVisible& VIS	= SD.KnownEnemys;

			VIS.clear		();
			if (SD.Leader)	{
				CEntityAlive* E	= dynamic_cast<CEntityAlive*>(SD.Leader);
				if (E && E->g_Alive())		E->GetVisible(VIS);
			}

			for (u32 G=0; G<SD.Groups.size(); G++)
			{
				CGroup& GD = SD.Groups[G];
				for (u32 M=0; M<GD.Members.size(); M++)
				{
					CEntityAlive* E	= dynamic_cast<CEntityAlive*>(GD.Members[M]);
					if (E && E->g_Alive())	E->GetVisible(VIS);
				}
			}
		}
	}

	// If we have enought bandwidth - replicate client data on to server
	Device.Statistic.netClient.Begin();
	ClientSend						();
	Device.Statistic.netClient.End	();

	CGameFont* F = ((CHUDManager*)Level().HUD())->pFontDI;
	// If server - perform server-update
	if (Server && CurrentEntity())
	{
		Device.Statistic.netServer.Begin();
		Server->Update					();
		Device.Statistic.netServer.End	();
		if (psDeviceFlags&rsStatistic)
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
		if (psDeviceFlags&rsStatistic)
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
	if (bDebug)			{
		ObjectSpace.dbgRender	();
		AI.Render				();
	}
}

void CLevel::OnEvent(EVENT E, u32 P1, u32 P2)
{
	if (E==eEntitySpawn)	{
		char	Name[128];	Name[0]=0;
		sscanf	(LPCSTR(P1),"%s", Name);
		Level().g_cl_Spawn	(Name,0xff,M_SPAWN_OBJECT_ACTIVE | M_SPAWN_OBJECT_LOCAL);
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
