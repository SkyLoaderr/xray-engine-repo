//----------------------------------------------------
// file: BuilderGame.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "Scene.h"
#include "LevelGameDef.h"

BOOL SceneBuilder::BuildGame()
{
	SExportStreams F;
    for(ObjectPairIt it=Scene.FirstClass(); it!=Scene.LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt it = lst.begin();it!=lst.end();it++)
            (*it)->ExportGame(F);
    }

    // save spawn
    AnsiString lev_spawn = m_LevelPath+"level.spawn";
    EFS.MarkFile	(lev_spawn,true);
    if (F.spawn.chunk)
	    F.spawn.stream.save_to(lev_spawn.c_str());

    // save game
	CMemoryWriter GAME;
    GAME.w_chunk(WAY_PATROLPATH_CHUNK,	F.patrolpath.stream.pointer(),	F.patrolpath.stream.size());
    GAME.w_chunk(RPOINT_CHUNK,			F.rpoint.stream.pointer(),		F.rpoint.stream.size());
    GAME.w_chunk(AIPOINT_CHUNK,			F.aipoint.stream.pointer(),		F.aipoint.stream.size());

    AnsiString lev_game	= m_LevelPath+"level.game";
    EFS.MarkFile	(lev_game,true);
    if (GAME.size())
	    GAME.save_to		(lev_game.c_str());

    return TRUE;
}

