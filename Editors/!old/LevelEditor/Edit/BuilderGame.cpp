//----------------------------------------------------
// file: BuilderGame.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "Scene.h"
#include "LevelGameDef.h"
#include "SoundManager.h"

BOOL SceneBuilder::BuildGame()
{
	SExportStreams F;
    Scene.ExportGame(F);

    // save spawn
    AnsiString lev_spawn = m_LevelPath+"level.spawn";
    EFS.MarkFile	(lev_spawn,true);
    if (F.spawn.chunk)
	    F.spawn.stream.save_to(lev_spawn.c_str());

    // save game
	CMemoryWriter GAME;
    GAME.w_chunk(WAY_PATROLPATH_CHUNK,	F.patrolpath.stream.pointer(),	F.patrolpath.stream.size());
    GAME.w_chunk(RPOINT_CHUNK,			F.rpoint.stream.pointer(),		F.rpoint.stream.size());

    AnsiString lev_game	= m_LevelPath+"level.game";
    EFS.MarkFile	(lev_game,true);
    if (GAME.size())
	    GAME.save_to		(lev_game.c_str());

    // save static sounds
    AnsiString lev_sound_static = m_LevelPath+"level.sound_static";
    EFS.MarkFile	(lev_sound_static,true);
    if (F.sound_static.chunk)    	
	    F.sound_static.stream.save_to(lev_sound_static.c_str());

    // save sound envs
    AnsiString lev_sound_env = m_LevelPath+"level.sound_environment";
    EFS.MarkFile			(lev_sound_env,true);
	if (SndLib.MakeEnvGeometry(F.sound_env_geom.stream,false))
    	F.sound_env_geom.stream.save_to(lev_sound_env.c_str());

    // save static PG
    AnsiString lev_pe_static = m_LevelPath+"level.ps_static";
    EFS.MarkFile	(lev_pe_static,true);
    if (F.pe_static.chunk)    	
	    F.pe_static.stream.save_to(lev_pe_static.c_str());
    return TRUE;
}

