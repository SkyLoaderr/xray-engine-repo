#include "stdafx.h"
#include "HUDmanager.h"
#include "LevelGameDef.h"
#include "ai_space.h"
#include "ParticlesObject.h"
#include "script_process.h"
#include "script_engine.h"
#include "level.h"
#include "game_cl_base.h"
#include "patrol_path_storage.h"

BOOL CLevel::Load_GameSpecific_Before()
{
	// AI space
	pApp->LoadTitle						("Loading AI objects...");
	string256							fn_game;
	
	if (!ai().get_alife() && FS.exist(fn_game,"$level$","level.ai"))
		ai().load						(net_SessionName());

	if (FS.exist(fn_game, "$level$", "level.game")) {
		IReader							*stream = FS.r_open		(fn_game);
		VERIFY							(m_patrol_path_storage);
		m_patrol_path_storage->load		(*stream);
		FS.r_close						(stream);
	}

	return								(TRUE);
}

BOOL CLevel::Load_GameSpecific_After()
{
	// loading static particles
	string256		fn_game;
	if (FS.exist(fn_game, "$level$", "level.ps_static")) {
		IReader *F = FS.r_open	(fn_game);
		IRender_Sector* S;
		CParticlesObject* pStaticParticles;
		int				chunk = 0;
		string256		ref_name;
		Fmatrix			transform;
		Fvector			zero_vel={0.f,0.f,0.f};
		for (IReader *OBJ = F->open_chunk(chunk++); OBJ; OBJ = F->open_chunk(chunk++)){
			OBJ->r_stringZ				(ref_name);
			OBJ->r						(&transform,sizeof(Fmatrix));transform.c.y+=0.01f;
			S							= ::Render->detectSector	(transform.c);
			pStaticParticles			= xr_new<CParticlesObject>	(ref_name,S,false);
			pStaticParticles->UpdateParent	(transform,zero_vel);
			pStaticParticles->Play			();
			m_StaticParticles.push_back		(pStaticParticles);
			OBJ->close	();
		}
		FS.r_close		(F);
	}
	// loading static sounds
	if (FS.exist(fn_game, "$level$", "level.sound_static")) {
		IReader *F		= FS.r_open	(fn_game);
		int				chunk = 0;
		string256		wav_name;
		CSound_params	params;
		for (IReader *OBJ = F->open_chunk(chunk++); OBJ; OBJ = F->open_chunk(chunk++)){
			static_Sounds.push_back	(xr_new<ref_sound>());
			ref_sound* S			= static_Sounds.back();

			OBJ->r_stringZ		(wav_name);
			S->create			(TRUE,wav_name);
			OBJ->r_fvector3		(params.position);
			params.volume		= OBJ->r_float();
			params.freq			= OBJ->r_float();
			params.min_distance = OBJ->r_float();
			params.max_distance	= OBJ->r_float();
			S->play				(0,true);
			S->set_params		(&params);
			OBJ->close			();
		}
		FS.r_close				(F);
	}
	// loading static sounds
	if (FS.exist(fn_game, "$level$", "level.sound_environment")) {
		IReader *F				= FS.r_open	(fn_game);
		::Sound->set_geometry_env(F);
		FS.r_close				(F);
	}

	// loading random (around player) sounds
	if (pSettings->section_exist("sounds_random"))	
	{ 
		CInifile::Sect& S		= pSettings->r_section("sounds_random");
		Sounds_Random.reserve	(S.size());
		for (CInifile::SectIt I=S.begin(); S.end()!=I; ++I) {
			Sounds_Random.push_back	(ref_sound());
			Sound->create			(Sounds_Random.back(),TRUE,*I->second);
		}
		Sounds_dwNextTime		= Device.TimerAsync	()	+ 5000;
	}
	
	// loading scripts
	ai().script_engine().remove_script_process("level");

	if (pLevel->section_exist("level_scripts") && pLevel->line_exist("level_scripts","script"))
		ai().script_engine().add_script_process("level",xr_new<CScriptProcess>("level",pLevel->r_string("level_scripts","script")));
	else
		ai().script_engine().add_script_process("level",xr_new<CScriptProcess>("level",""));
		
	BlockCheatLoad();
	return TRUE;
}

void CLevel::Load_GameSpecific_CFORM	( CDB::TRI* tris, u32 count )
{
	// 1.
	u16		default_id	= (u16)GMLib.GetMaterialIdx("default");

	// 2. Build mapping
	xr_map<u32,u16>		translator;
	translator.insert	(mk_pair(u32(-1),default_id));
	u16 idx				= 0;
	for (GameMtlIt I=GMLib.FirstMaterial(); GMLib.LastMaterial()!=I; ++I)
		translator.insert(mk_pair((*I)->GetID(),idx++));

	// 3.
	for (u32 it=0; it<count; ++it)
	{
		CDB::TRI* T						= tris + it;
		xr_map<u32,u16>::iterator index	= translator.find(T->material);
		if (index==translator.end())
			Debug.fatal	("Game material '%d' not found",T->material);
		T->material						= index->second;
	}
}

void CLevel::BlockCheatLoad()
{
	if( game && (GameID() != GAME_SINGLE) ) phTimefactor=1.f;
}