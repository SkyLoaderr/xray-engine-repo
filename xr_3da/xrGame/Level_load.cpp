#include "stdafx.h"
#include "HUDmanager.h"
#include "LevelGameDef.h"
#include "ai_space.h"
#include "ParticlesObject.h"
#include "ai_script_processor.h"
#include "script_engine.h"

void CLevel::vfCreateAllPossiblePaths(LPCSTR sName, SPath &tpPatrolPath)
{
	xr_vector<Fvector>		tpaPoints;
	xr_vector<Fvector>		tpaDeviations;
	xr_vector<u32>			tpaNodes;

	int i;
	int iCurPoint = 0, iPrevPoint = -1;
	u32 N = (u32)tpPatrolPath.tpaWayPoints.size();

	// building point sequencies and path
	tpPatrolPath.tpaWayPointIndexes.resize(N);
	for ( i=0; i<(int)N; ++i) {			
		tpPatrolPath.tpaWayPointIndexes[i] = iCurPoint;
		for (int j=0; j<(int)tpPatrolPath.tpaWayLinks.size(); ++j)
			if ((tpPatrolPath.tpaWayLinks[j].wFrom == iCurPoint) && (tpPatrolPath.tpaWayLinks[j].wTo != iPrevPoint)) {
				iPrevPoint = iCurPoint;
				iCurPoint = tpPatrolPath.tpaWayLinks[j].wTo;
				break;
			}
	}
}

BOOL CLevel::Load_GameSpecific_Before()
{
	// AI space
	pApp->LoadTitle	("Loading AI objects...");
	string256		fn_game;
	
	if (!ai().get_alife() && FS.exist(fn_game,"$level$","level.ai"))
		ai().load	(net_SessionName());

	if (FS.exist(fn_game, "$level$", "level.game")) {
		IReader *F = FS.r_open	(fn_game);
		IReader *O = 0;

		// Load WayPoints
		if (ai().get_level_graph() && (0!=(O = F->open_chunk	(WAY_PATROLPATH_CHUNK)))) { 
			int chunk = 0;
			for (IReader *OBJ = O->open_chunk(chunk++); OBJ; OBJ = O->open_chunk(chunk++)) {
				R_ASSERT			(OBJ->find_chunk(WAYOBJECT_CHUNK_VERSION));
				u32					dw = OBJ->r_u16();
				R_ASSERT			(dw == WAYOBJECT_VERSION);

				SPath tPatrolPath;

				LPSTR				sName = (LPSTR)xr_malloc(64*sizeof(char));
				R_ASSERT			(OBJ->find_chunk(WAYOBJECT_CHUNK_NAME));
				OBJ->r_stringZ		(sName);

				R_ASSERT			(OBJ->find_chunk(WAYOBJECT_CHUNK_POINTS));
				u32					dwCount = OBJ->r_u16();
				tPatrolPath.tpaWayPoints.resize(dwCount);
				for (int i=0; i<(int)dwCount; ++i){
					OBJ->r_fvector3	(tPatrolPath.tpaWayPoints[i].tWayPoint);
					tPatrolPath.tpaWayPoints[i].dwFlags = OBJ->r_u32();
					tPatrolPath.tpaWayPoints[i].dwNodeID = ai().level_graph().vertex(u32(-1),tPatrolPath.tpaWayPoints[i].tWayPoint);
					string256		S;
					OBJ->r_stringZ	(S);
					tPatrolPath.tpaWayPoints[i].name = S;
				}

				R_ASSERT(OBJ->find_chunk(WAYOBJECT_CHUNK_LINKS));
				u32 dwCountL = OBJ->r_u16();
				tPatrolPath.tpaWayLinks.resize(dwCountL);
				for ( i=0; i<(int)dwCountL; ++i){
					tPatrolPath.tpaWayLinks[i].wFrom		= OBJ->r_u16();
					tPatrolPath.tpaWayLinks[i].wTo			= OBJ->r_u16();
					tPatrolPath.tpaWayLinks[i].fProbability = OBJ->r_float();
				}

				OBJ->close();

				// sorting links
				bool bOk;
				do {
					bOk = true;
					for ( i=1; i<(int)dwCountL; ++i)
						if ((tPatrolPath.tpaWayLinks[i - 1].wFrom > tPatrolPath.tpaWayLinks[i].wFrom) || ((tPatrolPath.tpaWayLinks[i - 1].wFrom == tPatrolPath.tpaWayLinks[i].wFrom) && (tPatrolPath.tpaWayLinks[i - 1].wTo > tPatrolPath.tpaWayLinks[i].wTo))) {
							u16 wTemp = tPatrolPath.tpaWayLinks[i - 1].wFrom;
							tPatrolPath.tpaWayLinks[i - 1].wFrom = tPatrolPath.tpaWayLinks[i].wFrom;
							tPatrolPath.tpaWayLinks[i].wFrom = wTemp;
							wTemp = tPatrolPath.tpaWayLinks[i - 1].wTo;
							tPatrolPath.tpaWayLinks[i - 1].wTo = tPatrolPath.tpaWayLinks[i].wTo;
							tPatrolPath.tpaWayLinks[i].wTo = wTemp;
							bOk = false;
						}
				}
				while (!bOk);

				m_PatrolPaths.insert(mk_pair(sName,tPatrolPath));
				
				vfCreateAllPossiblePaths(sName, m_PatrolPaths[sName]);
			}
			O->close();
		}
		FS.r_close(F);
	}

	return TRUE;
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
	if (pLevel->section_exist("random_sounds"))	
	{
		CInifile::Sect& S		= pLevel->r_section("random_sounds");
		Sounds_Random.reserve	(S.size());
		for (CInifile::SectIt I=S.begin(); S.end()!=I; ++I) {
			Sounds_Random.push_back	(ref_sound());
			Sound->create			(Sounds_Random.back(),TRUE,*I->second);
		}
		Sounds_dwNextTime		= Device.TimerAsync	()	+ 5000;
	}
	
	// loading scripts
	ai().script_engine().remove_script_processor("level");

	if (pLevel->section_exist("level_scripts") && pLevel->line_exist("level_scripts","script"))
		ai().script_engine().add_script_processor("level",xr_new<CScriptProcessor>("level",pLevel->r_string("level_scripts","script")));
	else
		ai().script_engine().add_script_processor("level",xr_new<CScriptProcessor>("level",""));

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
