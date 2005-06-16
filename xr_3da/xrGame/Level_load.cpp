#include "stdafx.h"
#include "HUDmanager.h"
#include "LevelGameDef.h"
#include "ai_space.h"
#include "ParticlesObject.h"
#include "script_process.h"
#include "script_engine.h"
#include "script_engine_space.h"
#include "level.h"
#include "game_cl_base.h"
#include "patrol_path_storage.h"
#include "../x_ray.h"
#include "gamemtllib.h"
#include "PhysicsCommon.h"
#include "level_sounds.h"

//#define	OLES_REMAPPING
//#define	ALEX_REMAPPING
#define	DIMA_REMAPPING

#ifndef OLES_REMAPPING
#	ifndef ALEX_REMAPPING
#		ifndef DIMA_REMAPPING
			STATIC_CHECK(false,You_should_define_one_of_the_three_macroses_OLES_REMAPPING_ALEX_REMAPPING_DIMA_REMAPPING)
#		endif
#	else
#		ifdef DIMA_REMAPPING
			STATIC_CHECK(false,Only_single_of_three_macroses_should_be_defined_OLES_REMAPPING_ALEX_REMAPPING_DIMA_REMAPPING)
#		endif
#	endif
#else
#	ifdef ALEX_REMAPPING
		STATIC_CHECK	(false,Only_single_of_three_macroses_should_be_defined_OLES_REMAPPING_ALEX_REMAPPING_DIMA_REMAPPING)
#	endif
#	ifdef DIMA_REMAPPING
		STATIC_CHECK	(false,Only_single_of_three_macroses_should_be_defined_OLES_REMAPPING_ALEX_REMAPPING_DIMA_REMAPPING)
#	endif
#endif

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
		CParticlesObject* pStaticParticles;
		u32				chunk = 0;
		string256		ref_name;
		Fmatrix			transform;
		Fvector			zero_vel={0.f,0.f,0.f};
		for (IReader *OBJ = F->open_chunk_iterator(chunk); OBJ; OBJ = F->open_chunk_iterator(chunk,OBJ)) {
			OBJ->r_stringZ				(ref_name,sizeof(ref_name));
			OBJ->r						(&transform,sizeof(Fmatrix));transform.c.y+=0.01f;
			pStaticParticles			= xr_new<CParticlesObject>	(ref_name,FALSE);
			pStaticParticles->UpdateParent	(transform,zero_vel);
			pStaticParticles->Play			();
			m_StaticParticles.push_back		(pStaticParticles);
		}
		FS.r_close		(F);
	}
	// loading static sounds
	VERIFY								(m_level_sound_manager);
	m_level_sound_manager->Load			();

	// loading sound environment
	if (FS.exist(fn_game, "$level$", "level.snd_env")) {
		IReader *F				= FS.r_open	(fn_game);
		::Sound->set_geometry_env(F);
		FS.r_close				(F);
	}
	// loading SOM
	if (FS.exist(fn_game, "$level$", "level.som")) {
		IReader *F				= FS.r_open	(fn_game);
		::Sound->set_geometry_som(F);
		FS.r_close				(F);
	}

	// loading random (around player) sounds
	if (pSettings->section_exist("sounds_random")){ 
		CInifile::Sect& S		= pSettings->r_section("sounds_random");
		Sounds_Random.reserve	(S.size());
		for (CInifile::SectIt I=S.begin(); S.end()!=I; ++I) {
			Sounds_Random.push_back	(ref_sound());
			Sound->create			(Sounds_Random.back(),TRUE,*I->first);
		}
		Sounds_Random_dwNextTime= Device.TimerAsync	()	+ 50000;
		Sounds_Random_Enabled	= FALSE;
	}
	
	// loading scripts
	ai().script_engine().remove_script_process(ScriptEngine::eScriptProcessorLevel);

	if (pLevel->section_exist("level_scripts") && pLevel->line_exist("level_scripts","script"))
		ai().script_engine().add_script_process(ScriptEngine::eScriptProcessorLevel,xr_new<CScriptProcess>("level",pLevel->r_string("level_scripts","script")));
	else
		ai().script_engine().add_script_process(ScriptEngine::eScriptProcessorLevel,xr_new<CScriptProcess>("level",""));
		
	BlockCheatLoad();
	return TRUE;
}

#ifdef ALEX_REMAPPING
struct mtl_predicate {
	int					m_material_id;

	IC					mtl_predicate	(const u16 &material_id)
	{
		m_material_id	= material_id;
	}

	IC	bool			operator()		(SGameMtl *material) const
	{
		return			(material->GetID() == m_material_id);
	}
};
#endif // ALEX_REMAPPING

#ifdef DIMA_REMAPPING
struct translation_pair {
	u32			m_id;
	u16			m_index;

	IC			translation_pair	(u32 id, u16 index)
	{
		m_id	= id;
		m_index	= index;
	}

	IC	bool	operator==	(u16 id) const
	{
		return	(m_id == id);
	}

	IC	bool	operator<	(const translation_pair &pair) const
	{
		return	(m_id < pair.m_id);
	}

	IC	bool	operator<	(u16 id) const
	{
		return	(m_id < id);
	}
};
#endif // DIMA_REMAPPING

void CLevel::Load_GameSpecific_CFORM	( CDB::TRI* tris, u32 count )
{
/*
#ifdef OLES_REMAPPING
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
#endif

#ifdef ALEX_REMAPPING
	for (u32 it=0; it<count; ++it) {
		CDB::TRI			*T = tris + it;
		GameMtlIt			I = std::find_if(GMLib.FirstMaterial(),GMLib.LastMaterial(),mtl_predicate(T->material));
		if (I==GMLib.LastMaterial())
			Debug.fatal		("Game material '%d' not found",T->material);
		T->material			= u16(I - GMLib.FirstMaterial());
	}
#endif
*/
#ifdef DIMA_REMAPPING
	typedef xr_vector<translation_pair>	ID_INDEX_PAIRS;
	ID_INDEX_PAIRS						translator;
	translator.reserve					(GMLib.CountMaterial());
	u16									default_id = (u16)GMLib.GetMaterialIdx("default");
	translator.push_back				(translation_pair(u32(-1),default_id));

	u16									index = 0, static_mtl_count = 1;
	int max_ID							= 0;
	int max_static_ID					= 0;
	for (GameMtlIt I=GMLib.FirstMaterial(); GMLib.LastMaterial()!=I; ++I, ++index) {
		if (!(*I)->Flags.test(SGameMtl::flDynamic)) {
			++static_mtl_count;
			translator.push_back		(translation_pair((*I)->GetID(),index));
			if ((*I)->GetID()>max_static_ID)	max_static_ID	= (*I)->GetID(); 
		}
		if ((*I)->GetID()>max_ID)				max_ID			= (*I)->GetID(); 
	}
	// Msg("* Material remapping ID: [Max:%d, StaticMax:%d]",max_ID,max_static_ID);
	VERIFY(max_static_ID<0xFFFF);
	
	if (static_mtl_count < 128) {
		CDB::TRI						*I = tris;
		CDB::TRI						*E = tris + count;
		for ( ; I != E; ++I) {
			ID_INDEX_PAIRS::iterator	i = std::find(translator.begin(),translator.end(),(*I).material);
			if (i != translator.end()) {
				(*I).material			= (*i).m_index;
				SGameMtl* mtl			= GMLib.GetMaterialByIdx	((*i).m_index);
				(*I).suppress_shadows	= mtl->Flags.is(SGameMtl::flSuppressShadows);
				(*I).suppress_wm		= mtl->Flags.is(SGameMtl::flSuppressWallmarks);
				continue;
			}

			Debug.fatal					("Game material '%d' not found",(*I).material);
		}
		return;
	}

	std::sort							(translator.begin(),translator.end());
	{
		CDB::TRI						*I = tris;
		CDB::TRI						*E = tris + count;
		for ( ; I != E; ++I) {
			ID_INDEX_PAIRS::iterator	i = std::lower_bound(translator.begin(),translator.end(),(*I).material);
			if ((i != translator.end()) && ((*i).m_id == (*I).material)) {
				(*I).material			= (*i).m_index;
				SGameMtl* mtl			= GMLib.GetMaterialByIdx	((*i).m_index);
				(*I).suppress_shadows	= mtl->Flags.is(SGameMtl::flSuppressShadows);
				(*I).suppress_wm		= mtl->Flags.is(SGameMtl::flSuppressWallmarks);
				continue;
			}

			Debug.fatal					("Game material '%d' not found",(*I).material);
		}
	}
#endif
}

void CLevel::BlockCheatLoad()
{
	if( game && (GameID() != GAME_SINGLE) ) phTimefactor=1.f;
}
