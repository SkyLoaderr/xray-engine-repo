#include "stdafx.h"
#pragma hdrstop

#include "gamepersistent.h"
#include "../fmesh.h"
#include "../xr_ioconsole.h"
#include "gamemtllib.h"
#include "../skeletoncustom.h"
#include "profiler.h"
#include "MainUI.h"
#include "UICursor.h"
#include "game_base_space.h"
#include "level.h"
#include "ParticlesObject.h"
#include "actor.h"
//  [7/11/2005]
#include "game_base_space.h"
#include "weaponhud.h"
//  [7/11/2005]
#include "stalker_animation_data_storage.h"
#include "stalker_velocity_holder.h"

#include "../CameraManager.h"
#include "actor.h"

#ifndef _EDITOR
#	include "ai_debug.h"
#endif

#ifdef DEBUG
	static	void *	ode_alloc	(size_t size)								{ return Memory.mem_alloc(size,"ODE");			}
	static	void *	ode_realloc	(void *ptr, size_t oldsize, size_t newsize)	{ return Memory.mem_realloc(ptr,newsize,"ODE");	}
	static	void	ode_free	(void *ptr, size_t size)					{ return xr_free(ptr);							}
#else
	static	void *	ode_alloc	(size_t size)								{ return xr_malloc(size);			}
	static	void *	ode_realloc	(void *ptr, size_t oldsize, size_t newsize)	{ return xr_realloc(ptr,newsize);	}
	static	void	ode_free	(void *ptr, size_t size)					{ return xr_free(ptr);				}
#endif

CGamePersistent::CGamePersistent(void)
{
	ambient_sound_next_time		= 0;
	ambient_effect_next_time	= 0;
	ambient_effect_stop_time	= 0;
	ambient_particles			= 0;

	m_intro						= NULL;
	m_intro_event.bind			(this,&CGamePersistent::start_logo_intro);
#ifdef DEBUG
	m_frame_counter				= 0;
	m_last_stats_frame			= u32(-2);
#endif
	// 
	dSetAllocHandler			(ode_alloc		);
	dSetReallocHandler			(ode_realloc	);
	dSetFreeHandler				(ode_free		);

	// 
	BOOL	bDemoMode	= (0!=strstr(Core.Params,"-demomode "));
	if (bDemoMode)
	{
		string256	fname;
		LPCSTR		name	=	strstr(Core.Params,"-demomode ") + 10;
		sscanf				(name,"%s",fname);
		R_ASSERT2			(fname[0],"Missing filename for 'demomode'");
		Msg					("- playing in demo mode '%s'",fname);
		pDemoFile			=	FS.r_open	(fname);
		Device.seqFrame.Add	(this);
		eDemoStart			=	Engine.Event.Handler_Attach("GAME:demo",this);	
		uTime2Change		=	0;
	} else {
		pDemoFile			=	NULL;
		eDemoStart			=	NULL;
	}

	CWeaponHUD::CreateSharedContainer();
}

CGamePersistent::~CGamePersistent(void)
{	
	CWeaponHUD::DestroySharedContainer();
	FS.r_close					(pDemoFile);
	Device.seqFrame.Remove		(this);
	Engine.Event.Handler_Detach	(eDemoStart,this);
}

void CGamePersistent::RegisterModel(IRender_Visual* V)
{
	// Check types
	switch (V->Type){
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:{
		u16 def_idx		= GMLib.GetMaterialIdx("default_object");
		R_ASSERT2		(GMLib.GetMaterialByIdx(def_idx)->Flags.is(SGameMtl::flDynamic),"'default_object' - must be dynamic");
		CKinematics* K	= smart_cast<CKinematics*>(V); VERIFY(K);
		int cnt = K->LL_BoneCount();
		for (u16 k=0; k<cnt; k++){
			CBoneData& bd	= K->LL_GetData(k); 
			if (*(bd.game_mtl_name)){
				bd.game_mtl_idx	= GMLib.GetMaterialIdx(*bd.game_mtl_name);
				R_ASSERT2(GMLib.GetMaterialByIdx(bd.game_mtl_idx)->Flags.is(SGameMtl::flDynamic),"Required dynamic game material");
			}else{
				bd.game_mtl_idx	= def_idx;
			}
		}
	}break;
	}
}

extern void clean_game_globals	();
extern void init_game_globals	();

void CGamePersistent::OnAppStart()
{
	// load game materials
	GMLib.Load					();
	init_game_globals			();
	__super::OnAppStart			();
	m_pMainUI					= xr_new<CMainUI>();
}


void CGamePersistent::OnAppEnd	()
{
	if(m_pMainUI->IsActive())
		m_pMainUI->Activate(false);

	xr_delete					(m_pMainUI);

	__super::OnAppEnd			();

	clean_game_globals			();

	GMLib.Unload				();

}

void CGamePersistent::Start		(LPCSTR op)
{
	__super::Start				(op);
}

void CGamePersistent::Disconnect()
{
	CWeaponHUD::CleanSharedContainer();

	// destroy ambient particles
	CParticlesObject::Destroy(ambient_particles);

	__super::Disconnect			();
	// stop all played emitters
	::Sound->stop_emitters		();
}

void CGamePersistent::OnGameStart()
{
	__super::OnGameStart		();
	//  [7/11/2005]
	if (!xr_strcmp(m_game_params.m_game_type, "single")) m_eGameType = GAME_SINGLE;
	else
		if (!xr_strcmp(m_game_params.m_game_type, "deathmatch")) m_eGameType = GAME_DEATHMATCH;
		else
			if (!xr_strcmp(m_game_params.m_game_type, "teamdeathmatch")) m_eGameType = GAME_TEAMDEATHMATCH;
			else
				if (!xr_strcmp(m_game_params.m_game_type, "artfacthunt")) m_eGameType = GAME_ARTEFACTHUNT;
				else m_eGameType = GAME_ANY;
	//  [7/11/2005]
	m_intro_event.bind			(this,&CGamePersistent::start_game_intro);
}

void CGamePersistent::OnGameEnd	()
{
	__super::OnGameEnd					();

	xr_delete							(g_stalker_animation_data_storage);
	xr_delete							(g_stalker_velocity_holder);

	CWeaponHUD::CleanSharedContainer	();
}

void CGamePersistent::WeathersUpdate()
{
	if (g_pGameLevel){
		CActor* actor				= smart_cast<CActor*>(Level().CurrentViewEntity());
		BOOL bIndoor				= TRUE;
		if (actor) bIndoor			= actor->renderable_ROS()->get_luminocity_hemi()<0.05f;
//.		Log("ros",actor->renderable.ROS->get_luminocity_hemi());

		int data_set				= (Random.randF()<(1.f-Environment.CurrentEnv.weight))?0:1; 
		CEnvDescriptor* _env		= Environment.Current[data_set]; VERIFY(_env);
		CEnvAmbient* env_amb		= _env->env_ambient;
		if (env_amb){
			// start sound
			if (Device.dwTimeGlobal > ambient_sound_next_time){
				ref_sound* snd			= env_amb->get_rnd_sound();
				ambient_sound_next_time	= Device.dwTimeGlobal + env_amb->get_rnd_sound_time();
				if (snd){
					Fvector	pos;
					pos.random_dir		(Fvector().set(0,1,0),PI).normalize().mul(::Random.randF(25,75)).add(Device.vCameraPosition);
					snd->play_at_pos	(0,pos);
				}
			}
			// start effect
			if ((FALSE==bIndoor) && (0==ambient_particles) && Device.dwTimeGlobal>ambient_effect_next_time){
				CEnvAmbient::SEffect* eff			= env_amb->get_rnd_effect(); 
				Environment.wind_gust_factor		= eff->wind_gust_factor;
				ambient_effect_next_time			= Device.dwTimeGlobal + env_amb->get_rnd_effect_time();
				ambient_effect_stop_time			= Device.dwTimeGlobal + eff->life_time;
				if (eff){
					ambient_particles				= CParticlesObject::Create(eff->particles.c_str(),FALSE);
					Fvector pos; pos.add			(Device.vCameraPosition,eff->offset); 
					ambient_particles->play_at_pos	(pos);
					if (eff->sound._handle()){
						eff->sound.play_at_pos(0,pos);
					}
				}
			}
		}
		// stop if time exceed or indoor
		if (bIndoor || Device.dwTimeGlobal>=ambient_effect_stop_time){
			if (ambient_particles)					ambient_particles->Stop();
			Environment.wind_gust_factor			= 0.f;
		}
		// if particles not playing - destroy
		if (ambient_particles&&!ambient_particles->IsPlaying())
			CParticlesObject::Destroy(ambient_particles);
	}
}

#include "UI/UIGameTutorial.h"

void CGamePersistent::start_logo_intro		()
{
#ifdef DEBUG
	if (0!=strstr(Core.Params,"-nointro")){
		m_intro_event			= 0;
		Console->Show			();
		Console->Execute		("main_menu on");
		return;
	}
#endif
	if (Device.dwPrecacheFrame==0){
		m_intro_event.bind		(this,&CGamePersistent::update_logo_intro);
		if (0==xr_strlen(m_game_params.m_game_or_spawn) && NULL==g_pGameLevel){
			VERIFY				(NULL==m_intro);
			m_intro				= xr_new<CUISequencer>();
			m_intro->Start		("intro_logo");
			Console->Hide		();
		}
	}
}
void CGamePersistent::update_logo_intro			()
{
	if(m_intro && (false==m_intro->IsActive())){
		m_intro_event			= 0;
		xr_delete				(m_intro);
		//.Console->Show			();
		Console->Execute		("main_menu on");
	}
}
static float save_music_vol=psSoundVMusic;
void CGamePersistent::start_game_intro		()
{
#ifdef DEBUG
	if (0!=strstr(Core.Params,"-nointro")){
		m_intro_event			= 0;
		return;
	}
#endif
	if (g_pGameLevel && g_pGameLevel->bReady && Device.dwPrecacheFrame==0){
		m_intro_event.bind		(this,&CGamePersistent::update_game_intro);
		if (0==stricmp(m_game_params.m_new_or_load,"new")){
			VERIFY				(NULL==m_intro);
			m_intro				= xr_new<CUISequencer>();
			m_intro->Start		("intro_game");
			save_music_vol		= psSoundVMusic;
			psSoundVMusic		= 0.f;
		}
	}
}
void CGamePersistent::update_game_intro			()
{
	if(m_intro && (false==m_intro->IsActive())){
		xr_delete				(m_intro);
		m_intro_event			= 0;
		psSoundVMusic			= save_music_vol;
	}
}

void CGamePersistent::OnFrame	()
{
#ifdef DEBUG
	++m_frame_counter;
#endif
	if (!m_intro_event.empty())	m_intro_event();

	if(!g_pGameLevel)			return;
	if(!g_pGameLevel->bReady)	return;

	if(Device.Pause()){
		if(g_actor)
		Level().Cameras().Update(Actor()->cam_Active());
	}
	__super::OnFrame			();

	Engine.Sheduler.Update		();

	// update weathers ambient
	WeathersUpdate				();

	if	(0!=pDemoFile){
		if	(Device.dwTimeGlobal>uTime2Change){
			// Change level + play demo
			if			(pDemoFile->elapsed()<3)	pDemoFile->seek(0);		// cycle

			// Read params
			string512			params;
			pDemoFile->r_string	(params,sizeof(params));
			string256			o_server, o_client, o_demo;	u32 o_time;
			sscanf				(params,"%[^,],%[^,],%[^,],%d",o_server,o_client,o_demo,&o_time);

			// Start _new level + demo
			Engine.Event.Defer	("KERNEL:disconnect");
			Engine.Event.Defer	("KERNEL:start",size_t(xr_strdup(_Trim(o_server))),size_t(xr_strdup(_Trim(o_client))));
			Engine.Event.Defer	("GAME:demo",	size_t(xr_strdup(_Trim(o_demo))), u64(o_time));
			uTime2Change		= 0xffffffff;	// Block changer until Event received
		}
	}

#ifdef DEBUG
	if ((m_last_stats_frame + 1) < m_frame_counter)
		profiler().clear		();
#endif
}

void CGamePersistent::OnEvent(EVENT E, u64 P1, u64 P2)
{
	string256			cmd;
	LPCSTR				demo	= LPCSTR(P1);
	sprintf				(cmd,"demo_play %s",demo);
	Console->Execute	(cmd);
	xr_free				(demo);
	uTime2Change		= Device.TimerAsync() + u32(P2)*1000;
}

void CGamePersistent::Statistics	(CGameFont* F)
{
#ifdef DEBUG
#	ifndef _EDITOR
		m_last_stats_frame		= m_frame_counter;
		profiler().show_stats	(F,!!psAI_Flags.test(aiStats));
#	endif
#endif
}

float CGamePersistent::MtlTransparent(u32 mtl_idx)
{
	return GMLib.GetMaterialByIdx((u16)mtl_idx)->fVisTransparencyFactor;
}
static BOOL bRestorePause	= FALSE;
static BOOL bEntryFlag		= TRUE;

void CGamePersistent::OnAppActivate		()
{
	if(!bRestorePause)
		Device.Pause(FALSE);

	bEntryFlag = TRUE;
//.	Level().Cameras().Update(Actor()->cam_Active());
}

void CGamePersistent::OnAppDeactivate	()
{
	if(!bEntryFlag) return;

	bRestorePause = FALSE;
	if (!g_pGameLevel || (g_pGameLevel && Level().game && GameID()== GAME_SINGLE) )
	{
		bRestorePause = Device.Pause();
		Device.Pause(TRUE);
	}
	bEntryFlag = FALSE;
}

bool CGamePersistent::OnRenderPPUI_query()
{
	return UI()->OnRenderPPUI_query();
	// enable PP or not
}

void CGamePersistent::OnRenderPPUI_main()
{
	// always
	UI()->OnRenderPPUI_main();
}

void CGamePersistent::OnRenderPPUI_PP()
{
	UI()->OnRenderPPUI_PP();
}
