// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:14:04 , by user : Oles , from computer : OLES
// Level.cpp: implementation of the CLevel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../fdemorecord.h"
#include "../fdemoplay.h"
#include "../environment.h"
#include "ParticlesObject.h"

#include "Level.h"
#include "entity_alive.h"
#include "hudmanager.h"
#include "ai_space.h"
#include "ai_debug.h"

// events
#include "PHdynamicdata.h"
#include "Physics.h"

#include "ShootingObject.h"

#include "LevelFogOfWar.h"
#include "Level_Bullet_Manager.h"

#include "ai_script_processor.h"
#include "script_engine.h"

CPHWorld*	ph_world = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLevel::CLevel()
{
//	XML_DisableStringCaching();
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

	//by Dandy
	m_pFogOfWar					= NULL;
	m_pFogOfWar					= xr_new<CFogOfWar>();

	m_pBulletManager			= xr_new<CBulletManager>();

	//----------------------------------------------------
	//получить материал пули
	CShootingObject::bullet_material_idx  = GMLib.GetMaterialIdx(WEAPON_MATERIAL_NAME);
	BulletManager().bullet_material_idx   = GMLib.GetMaterialIdx(WEAPON_MATERIAL_NAME);;

//----------------------------------------------------
	m_bNeed_CrPr					= false;
	m_dwNumSteps				= 0;
	m_dwDeltaUpdate				= u32(fixed_step*1000);
	m_dwLastNetUpdateTime		= 0;

	physics_step_time_callback	= (PhysicsStepTimeCallback*) &PhisStepsCallback;


#ifdef DEBUG
	m_bSynchronization			= false;
#endif	
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
	for (POIt p_it=m_StaticParticles.begin(); m_StaticParticles.end()!=p_it; ++p_it)
		xr_delete		(*p_it);
	m_StaticParticles.clear();

	// Unload sounds
	for (u32 i=0; i<static_Sounds.size(); ++i){
		static_Sounds[i]->destroy();
		xr_delete		(static_Sounds[i]);
	}
	static_Sounds.clear	();

	//by Dandy
	//destroy fog of war
	xr_delete			(m_pFogOfWar);
	//destroy bullet manager
	xr_delete			(m_pBulletManager);


	{
		SPathPairIt			I = m_PatrolPaths.begin();
		SPathPairIt			E = m_PatrolPaths.end();
		for (int i=0; I != E; ++I, ++i)
			xr_free((char*&)(*I).first);
	}
	m_PatrolPaths.clear();
	
	ai().script_engine().remove_script_processor("level");

	xr_free				(m_caServerOptions);
	xr_free				(m_caClientOptions);
}

// Game interface ////////////////////////////////////////////////////
int	CLevel::get_RPID(LPCSTR /**name/**/)
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
	for (int i=0; i<(int)(rp.size()); ++i)
		if (pos.similar(rp[i],EPS_L))	return i;
	*/
	return -1;
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
	//Net sync
	if (m_bNeed_CrPr)					make_NetCorrectionPrediction();

	// Inherited update
	inherited::OnFrame					();

	// Draw client/server stats
	CGameFont* F = HUD().pFontDI;
	if (Server)
	{
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
			for (u32 I=0; I<Server->client_Count(); ++I)	{
				IClient*	C = Server->client_Get(I);
				F->OutNext("%10s: P(%d), BPS(%2.1fK), MRR(%2d), MSR(%2d)",
					Server->game->get_option_s(C->Name,"name",C->Name),
//					C->Name,
					C->stats.getPing(),
					float(C->stats.getBPS())/1024,
					C->stats.getMPS_Receive	(),
					C->stats.getMPS_Send	()
					);
			}
		}
	} else {
		if (psDeviceFlags.test(rsStatistic))
		{
			F->SetSize	(0.015f);
			F->OutSet	(0.0f,0.5f);
			F->SetColor	(D3DCOLOR_XRGB(0,255,0));
			F->OutNext	("client_2_sever ping: %d",	net_Statistic.getPing());
		}
	}
	
	g_pGamePersistent->Environment.SetGameTime	(GetGameDayTimeSec(),GetGameTimeFactor());

	ai().script_engine().script_processor("level")->update();

	//просчитать полет пуль
	BulletManager().Update	();
}

void CLevel::OnRender()
{
	inherited::OnRender	();
	Tracers.Render		();
//	ph_world->Render	();
#ifdef DEBUG
	if (ai().get_level_graph() && (bDebug || psAI_Flags.test(aiMotion)))
		ai().level_graph().render();
	if (bDebug)	{
		ObjectSpace.dbgRender	();
	}
#endif
}

void CLevel::OnEvent(EVENT E, u64 P1, u64 /**P2/**/)
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
		// int id = atoi((char*)P1);
		// Environment->Music_Play(id);
	} else if (E==eEnvironment) {
		// int id=0; float s=1;
		// sscanf((char*)P1,"%d,%f",&id,&s);
		// Environment->set_EnvMode(id,s);
	} else return;
}


int	lvInterp = 0;
u32	lvInterpSteps = 0;

void CLevel::make_NetCorrectionPrediction	()
{
	m_bNeed_CrPr = false;
	u64 NumPhSteps = ph_world->m_steps_num;
	ph_world->m_steps_num -= m_dwNumSteps;
	
//////////////////////////////////////////////////////////////////////////////////
	ph_world->Freeze();

	//setting UpdateData and determining number of PH steps from last received update
	for (xr_vector<CObject*>::iterator O=Objects.objects.begin(); O!=Objects.objects.end(); O++) 
	{
		CGameObject* P = dynamic_cast<CGameObject*>(*O);
		if (!P) continue;
		P->PH_B_CrPr();
	};
//////////////////////////////////////////////////////////////////////////////////
	//first prediction from "delivered" to "real current" position
	//making enought PH steps to calculate current objects position based on their updated state
	for (u32 i =0; i<m_dwNumSteps; i++)	
	{
		ph_world->Step();
		
		CActor* pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
		if (!pActor) continue;
		pActor->PH_I_CrPr();
	};
//////////////////////////////////////////////////////////////////////////////////
	for (xr_vector<CObject*>::iterator O=Objects.objects.begin(); O!=Objects.objects.end(); O++) 
	{
		CGameObject* P = dynamic_cast<CGameObject*>(*O);
		if (!P) continue;
		P->PH_I_CrPr();
	}
//////////////////////////////////////////////////////////////////////////////////
	if (!InterpolationDisabled())
	{
		for (u32 i =0; i<lvInterpSteps; i++)	//second prediction "real current" to "future" position
			ph_world->Step();
		//////////////////////////////////////////////////////////////////////////////////
		for (xr_vector<CObject*>::iterator O=Objects.objects.begin(); O!=Objects.objects.end(); O++) 
		{
			CGameObject* P = dynamic_cast<CGameObject*>(*O);
			if (!P) continue;
			P->PH_A_CrPr();
		}
	};
	ph_world->UnFreeze();

	ph_world->m_steps_num = NumPhSteps;
	m_dwNumSteps = 0;
};

u32			CLevel::GetInterpolationSteps	()
{
	return lvInterpSteps;
};

void		CLevel::SetInterpolationSteps	(u32 InterpSteps)
{
	lvInterpSteps = InterpSteps;
};

void		CLevel::UpdateDeltaUpd	( u32 LastTime )
{
	u32 CurrentDelta = iFloor(float(m_dwDeltaUpdate * 10 + (LastTime - m_dwLastNetUpdateTime)) / 11);
	m_dwLastNetUpdateTime = LastTime;
	m_dwDeltaUpdate = CurrentDelta;

	if (0 == lvInterp) ReculcInterpolationSteps();
};

void		CLevel::ReculcInterpolationSteps ()
{
	lvInterpSteps			= iFloor(float(m_dwDeltaUpdate) / (fixed_step*1000));
	if (lvInterpSteps > 50) lvInterpSteps = 50;
	if (lvInterpSteps < 3)	lvInterpSteps = 3;
};

bool				CLevel::InterpolationDisabled	()
{
	return lvInterp < 0; 
};

void __stdcall		CLevel::PhisStepsCallback	( u32 Time0, u32 Time1 )
{
	if (Game().type == 1)	return;

#pragma todo("Oles to all: highly inefficient and slow!!!")

	for (xr_vector<CObject*>::iterator O=Level().Objects.objects.begin(); O!=Level().Objects.objects.end(); O++) 
	{
		CActor* pActor = dynamic_cast<CActor*>(*O);
		if (!pActor || pActor->Remote()) continue;
		pActor->UpdatePosStack(Time0, Time1);
	};
};

void				CLevel::SetNumCrSteps		( u32 NumSteps )
{
	m_bNeed_CrPr = true;
	R_ASSERT2(NumSteps != 0xffffffff, "Invalid Steps Num");
	if (m_dwNumSteps > NumSteps) return;
	m_dwNumSteps = NumSteps;
};
